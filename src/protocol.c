/***************************************************************************
 *            protocol.c
 *
 *  VPMN  -  Virtual Private Mesh Network
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/x509v3.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include "debug.h"
#include "config.h"
#include "protocol.h"
#include "peer.h"
#include "router.h"
#include "udpsrvdtls.h"
#include "tundev.h"

//-TODO MUEXES
struct protocol_1id_s *protocol_v1id;
int protocol_v1id_len;
struct protocol_1ida_s protocol_v1ida;
#define protocol_v1ida_len sizeof (struct protocol_1ida_s)
struct protocol_1ka_s *protocol_v1ka;
int protocol_v1ka_len;
int protocol_v1ka_maxlen;
int protocol_v1ka_pos;

int protocol_processpeer(struct peer_s *peer,
                         struct protocol_peer_s *fragment, int max_size)
{
  int i;
  struct protocol_addrpair_s *addrpair;
  struct protocol_netpair_s *netpair;
  if (sizeof(struct protocol_peer_s) +
      ((struct protocol_peer_s *) fragment)->len_net *
      sizeof(struct protocol_netpair_s) +
      ((struct protocol_peer_s *) fragment)->len_addr *
      sizeof(struct protocol_addrpair_s) > max_size)
    return -1;
  peer->addrs_len = fragment->len_addr;
  peer->shared_networks_len = fragment->len_net;
  peer->addrs = realloc(peer->addrs,
                        peer->addrs_len * sizeof(struct sockaddr_in));
  peer->shared_networks = realloc(peer->shared_networks,
                                  peer->shared_networks_len *
                                  sizeof(struct in_network));
  for (i = 0; i < peer->shared_networks_len; i++)
    {
      netpair =
        (struct protocol_netpair_s *) ((void *) fragment +
                                       sizeof(struct protocol_peer_s) +
                                       (i *
                                        sizeof(struct
                                               protocol_netpair_s)));
      peer->shared_networks[i].addr.s_addr = netpair->addr;
      peer->shared_networks[i].netmask.s_addr = netpair->netmask;
    }
  for (i = 0; i < peer->addrs_len; i++)
    {
      addrpair =
        (struct protocol_addrpair_s *) ((void *) fragment +
                                        sizeof(struct protocol_peer_s) +
                                        (peer->shared_networks_len *
                                         sizeof(struct protocol_netpair_s))
                                        +
                                        (i *
                                         sizeof(struct
                                                protocol_addrpair_s)));
      peer->addrs[i].sin_family = AF_INET;
      peer->addrs[i].sin_port = addrpair->port;
      peer->addrs[i].sin_addr.s_addr = addrpair->addr;
    }
  return sizeof(struct protocol_peer_s) +
    ((struct protocol_peer_s *) fragment)->len_net *
    sizeof(struct protocol_netpair_s) +
    ((struct protocol_peer_s *) fragment)->len_addr *
    sizeof(struct protocol_addrpair_s);
}

void protocol_recvpacket(const char *buffer, const int buffer_len,
                         struct udpsrvsession_s *session)
{
  uint16_t len;
  int i;
  void *p;
  const struct protocol_ip *ip = (struct protocol_ip *) buffer;
  struct peer_s *peer;
  if (buffer_len < 4)
    return;
  len = ntohs(ip->tot_len);
#if DEBUG > 2
  log_info("UDP: sizes %d = %d.\n", len, buffer_len);

    if (buffer_len > 27)
    {
      uint16_t *ping = (uint16_t*) &buffer[26];
      log_debug("Ping%d id: %d\n", ip->version, ntohs(*ping));
    }
#endif
  if (ip->version == 4)         //IPv4 packet
    {
      if (session->peer == NULL
          || (session->peer->stat & PEER_STAT_ID) == 0 || buffer_len < 20)
        return;
      if (router_checksrc((struct in_addr *) &ip->daddr, &tun_selfpeer) !=
          0)
        {
          log_debug("Not for me: %s\n",
                    inet_ntoa(*(struct in_addr *) &ip->daddr));
          return;
        }
      if (router_checksrc((struct in_addr *) &ip->saddr, session->peer) !=
          0)
        {
          log_debug("Not from there: %s\n",
                    inet_ntoa(*(struct in_addr *) &ip->saddr));
          return;
        }
      if (len > buffer_len)
        {
          log_error("UDP: Invalid size %d > %d.\n", len, buffer_len);
          return;
        }
      tundev_write(buffer, len);
    }
//else if (ip->version == 6) //IPv6 Packet
  else if (ip->version == PROTOCOL1_V && ip->ihl == 1)  //Internal packets v1
    {
      if (((struct protocol_1_s *) ip)->pid == PROTOCOL1_IDA)
        {
          log_debug("ACK...\n");
          if (session->peer != NULL
              && (session->peer->stat & PEER_STAT_IDK) == 0)
            {
              pthread_mutex_lock(&session->peer->modify_mutex);
              session->peer->stat |= PEER_STAT_IDK;
              pthread_mutex_unlock(&session->peer->modify_mutex);
            }
          if ((session->peer->stat & PEER_STAT_ID) != 0)
            log_info("New peer authenticated.\n");
        }
      else if (((struct protocol_1_s *) ip)->pid == PROTOCOL1_ID)
        {
          log_debug("ID...\n");
          if (session->peer == NULL)
            session->peer = peer_create();
          pthread_mutex_lock(&session->peer->modify_mutex);
          peer = session->peer;
          peer->udpsrvsession = session;
          if ((peer->stat & PEER_STAT_ID) == 0)
            {
              if (protocol_processpeer(peer,
                                       &((struct protocol_1id_s *)
                                         buffer)->peer,
                                       buffer_len -
                                       sizeof(struct protocol_1id_s) +
                                       sizeof(struct protocol_peer_s)) < 0)
                {
                  log_error("invalid ID\n");
                  //-TODO: Check
                  pthread_mutex_unlock(&peer->modify_mutex);
                  peer_destroy(peer);
                  return;
                }
              log_debug("Validating id.\n");
              /* Check for valid routes shared */
              if (protocol_checknameconstraints(peer) < 0)
                {
                  log_debug
                    ("Peer trying to request for unallowed network...\n");
                  pthread_mutex_unlock(&peer->modify_mutex);
                  peer_destroy(peer);
                  return;
                }
              peer->stat |= PEER_STAT_ID;
              timeout_update(&peer->timeout);
              if ((session->peer->stat & PEER_STAT_IDK) != 0)
                log_info("New peer authenticated.\n");
              pthread_mutex_unlock(&peer->modify_mutex);
              if (peer_add(peer, session) < 1)
                {
                  peer_destroy(peer);
                  return;
                }
            }
          //-TODO: JOIN packets
          protocol_sendpacket(session, PROTOCOL1_IDA);
          if ((peer->stat & PEER_STAT_IDK) == 0)
            protocol_sendpacket(session, PROTOCOL1_ID);
        }
      else if (((struct protocol_1_s *) ip)->pid == PROTOCOL1_KA)
        {
          if (len > buffer_len)
            return;
          log_debug("KA...\n");
          if (session->peer != NULL
              && (session->peer->stat & PEER_STAT_ID) != 0)
            {
              timeout_update(&session->peer->timeout);
              p = (void *) buffer + sizeof(struct protocol_1ka_s);
              while (p <= (void *) &buffer[len - 1])
                {
                  peer = peer_create();
                  i =
                    protocol_processpeer(peer,
                                         (struct protocol_peer_s *) p,
                                         (int) ((void *)
                                                &buffer[len - 1] - p));
                  if (i < 0)
                    {
                      peer_destroy(peer);
                      return;
                    }
                  p += i;
                  if (router_existpeer
                      (peer->shared_networks,
                       peer->shared_networks_len) == 0)
                    peer_addnew(peer);
                  else
                    peer_destroy(peer);
                }
            }
        }
      else
        {
          log_error("Unknown paquet\n");
          return;
        }
    }
  else
    {
      log_error("Unknown paquet\n");
      return;
    }
  if (len + 4 <= buffer_len)
    protocol_recvpacket(buffer + len, buffer_len - len, session);
}

int protocol_sendframe(const char *buffer, const int buffer_len)
{
  int ret = -1;
  uint16_t len;
  const struct protocol_ip *ip = (struct protocol_ip *) buffer;
  struct peer_s *dstpeer = NULL;
  if (buffer_len < 20)
    return -1;
  //Check for IPv4
  if (ip->version == 4)
    {
      len = ntohs(ip->tot_len);
      if (len > buffer_len)
        {
          log_error("TUN: Invalid size %d > %d.\n", len, buffer_len);
          return -1;
        }
      if (router_checksrc((struct in_addr *) &ip->saddr, &tun_selfpeer) ==
          0)
        {
          //-TODO: CHECK BROADCAST
          dstpeer = router_searchdst((struct in_addr *) &ip->daddr);
          if (dstpeer != NULL && dstpeer->udpsrvsession != NULL)
            {
              log_debug("Sending frame. (%d)\n", len);
              udpsrvdtls_write(buffer, len, dstpeer->udpsrvsession);
              ret = 0;
            }
          else                  //TODO: ICMP
            log_error("Invalid destination: %s\n",
                      inet_ntoa(*(struct in_addr *) &ip->daddr));
        }
      else                      //TODO: ICMP
        log_error("Invalid source.\n");
      if (len + 20 <= buffer_len)
        return protocol_sendframe(buffer + len, buffer_len - len);
      return ret;
    }
  //Check for IPv6
  else if (ip->version == 6)
    {
      log_error("IPv6 not implemented.\n");
      //ROUTING IPv6
    }
  else
    log_error("Unknow protocol not implemented.\n");
  log_debug("Frame lost. (%d)\n", buffer_len);
  return -1;
}

int protocol_sendpacket(struct udpsrvsession_s *session, const int type)
{
  char *packet = NULL;
  int packet_len = -1;
  switch (type)
    {
      case PROTOCOL1_ID:
        log_debug("Sending ID packet.\n");
        packet = (char *) protocol_v1id;
        packet_len = protocol_v1id_len;
        break;
      case PROTOCOL1_IDA:
        log_debug("Sending IDA packet.\n");
        packet = (char *) &protocol_v1ida;
        packet_len = protocol_v1ida_len;
        break;
      case PROTOCOL1_KA:
        log_debug("Sending KA packet.\n");
        packet = (char *) protocol_v1ka;
        packet_len = protocol_v1ka_len;
        break;
    }
  if (packet_len > 0 && session != NULL)
    {
      udpsrvdtls_write(packet, packet_len, session);
      return 0;
    }
  log_debug("not sending packet\n");
  return -1;
}

void protocol_slidekeepalive()
{
/*
	int i;
  if (protocol_v1id_len + protocol_v1ka_len < protocol_v1idka_maxlen)
    protocol_v1ka_pos = 0;
  i = udpsrvsession_dumpsocks (protocol_v1ka + sizeof (struct protocol_1ka_s),
			       protocol_v1idka_maxlen - protocol_v1id_len,
			       protocol_v1ka_pos, MAXKAPEERS);
  protocol_v1ka->base.version = PROTOCOL1_V;
  protocol_v1ka->base.ihl = 1;
  protocol_v1ka->base.pid = PROTOCOL1_KA;
  protocol_v1ka->len = i;
  protocol_v1ka_len =
    sizeof (struct protocol_1ka_s) + i * sizeof (struct protocol_addrpair_s);
  protocol_v1ka_pos += i;
  protocol_v1ka->base.tot_len = htons(protocol_v1ka_len);
*/
}

void protocol_init()
{
  struct protocol_addrpair_s *addrpair;
  struct protocol_netpair_s *netpair;
  int i;
  protocol_v1ida.base.version = PROTOCOL1_V;
  protocol_v1ida.base.ihl = 1;
  protocol_v1ida.base.pid = PROTOCOL1_IDA;
  protocol_v1ida.base.tot_len = htons(protocol_v1ida_len);
  protocol_v1id_len = sizeof(struct protocol_1id_s) +
    tun_selfpeer.shared_networks_len * sizeof(struct protocol_netpair_s) +
    tun_selfpeer.addrs_len * sizeof(struct protocol_addrpair_s);
  protocol_v1id = malloc(protocol_v1id_len);
  protocol_v1id->base.version = PROTOCOL1_V;
  protocol_v1id->base.ihl = 1;
  protocol_v1id->base.pid = PROTOCOL1_ID;
  protocol_v1id->base.tot_len = htons(protocol_v1id_len);
  if (tun_selfpeer.addrs_len < 256)
    protocol_v1id->peer.len_addr = tun_selfpeer.addrs_len;
  else
    protocol_v1id->peer.len_addr = 255;
  if (tun_selfpeer.shared_networks_len < 256)
    protocol_v1id->peer.len_net = tun_selfpeer.shared_networks_len;
  else
    protocol_v1id->peer.len_net = 255;
  for (i = 0; i < protocol_v1id->peer.len_net; i++)
    {
      netpair =
        (struct protocol_netpair_s *) ((void *) protocol_v1id +
                                       sizeof(struct protocol_1id_s) +
                                       (i *
                                        sizeof(struct
                                               protocol_netpair_s)));
      netpair->addr = tun_selfpeer.shared_networks[i].addr.s_addr;
      netpair->netmask = tun_selfpeer.shared_networks[i].netmask.s_addr;
    }
  for (i = 0; i < protocol_v1id->peer.len_addr; i++)
    {
      addrpair =
        (struct protocol_addrpair_s *) ((void *) protocol_v1id +
                                        sizeof(struct protocol_1id_s) +
                                        (protocol_v1id->peer.len_net *
                                         sizeof(struct protocol_netpair_s))
                                        +
                                        (i *
                                         sizeof(struct
                                                protocol_addrpair_s)));
      addrpair->addr = tun_selfpeer.addrs[i].sin_addr.s_addr;
      addrpair->port = tun_selfpeer.addrs[i].sin_port;
    }
  protocol_v1ka_maxlen =
    sizeof(struct protocol_1ka_s) +
    MAXKAPEERS * (sizeof(struct protocol_peer_s) +
                  sizeof(struct protocol_netpair_s) +
                  sizeof(struct protocol_netpair_s));
  protocol_v1ka = malloc(protocol_v1ka_maxlen);
  protocol_v1ka_pos = 0;
  protocol_slidekeepalive();
}

void protocol_maintainerthread()
{
//struct peer_s *dstpeer;
  protocol_init();
/*-TODO
  while (1)
    {
      protocol_slidekeepalive ();

      protocol_sendpacket (dstpeer, PROTOCOL1_KA);
	  sleep(PROTOCOL_HOLEPUNCHINGTIME);
    }*/
}

int protocol_checknameconstraints(const struct peer_s *peer)
{
  struct in_network ncnet, *shnet;
  int i, j, r, ret = 0, valid = 0;
  const unsigned char *p;
  void *ext_str = NULL;
  X509 *cert;
  STACK_OF(X509_EXTENSION) * exts;
  X509_EXTENSION *ext;
  const X509V3_EXT_METHOD *method;
  STACK_OF(GENERAL_SUBTREE) * trees;
  GENERAL_SUBTREE *tree;

  pthread_mutex_lock(&peer->udpsrvsession->dtls_mutex);
  cert = SSL_get_peer_certificate(peer->udpsrvsession->dtls);
  exts = cert->cert_info->extensions;
  for (r = 0; r < peer->shared_networks_len; r++)
    {
      shnet = &peer->shared_networks[r];
      for (i = 0; i < sk_X509_EXTENSION_num(exts); i++)
        {
          ext = sk_X509_EXTENSION_value(exts, i);
          if ((method = X509V3_EXT_get(ext))
              && method->ext_nid == NID_name_constraints)
            {
              p = ext->value->data;
              if (method->it)
                ext_str = ASN1_item_d2i(NULL, &p, ext->value->length,
                                        ASN1_ITEM_ptr(method->it));
              else
                ext_str = method->d2i(NULL, &p, ext->value->length);

              trees = ((NAME_CONSTRAINTS *) ext_str)->permittedSubtrees;
              for (j = 0; j < sk_GENERAL_SUBTREE_num(trees); j++)
                {
                  tree = sk_GENERAL_SUBTREE_value(trees, j);
                  if (tree->base->type == GEN_IPADD)
                    {
                      if (tree->base->d.ip->length == 8)
                        {
                          ncnet.addr.s_addr =
                            *((uint32_t *) tree->base->d.ip->data);
                          ncnet.netmask.s_addr =
                            *((uint32_t *) & tree->base->d.ip->data[4]);
                          if (shnet->netmask.s_addr >= ncnet.netmask.s_addr
                              && (ncnet.addr.s_addr & ncnet.netmask.s_addr)
                              == (shnet->addr.s_addr &
                                  shnet->netmask.s_addr))
                            valid++;
                        }
//else if(tree->base->d.ip->length == 32) //IPv6
//  See openssl/crypto/x509v3/v3_ncons.c:static int print_nc_ipadd()
//} else { //DNS
//  GENERAL_NAME_print(bp, tree->base);
                    }
                }
            }
        }
      if (valid == 0){
        ret = -1;
        break;
      }
      ret += valid;
      valid = 0;
    }
  pthread_mutex_unlock(&peer->udpsrvsession->dtls_mutex);
  return ret;
}
