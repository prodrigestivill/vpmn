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
struct protocol_1_s protocol_v1ida;
#define protocol_v1ida_len sizeof (struct protocol_1_s);
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
  int i;
  void *p;
  unsigned int begin = 0;
  struct iphdr *ip = NULL;
  struct peer_s *peer;

  if (buffer_len < 4)
    return;
  ip = (struct iphdr *) &buffer[begin];
  if (ip->version == 4)         //IPv4 packet
    {
      if (session->peer == NULL || buffer_len < 20)
        return;
      if (router_checksrc((struct in_addr *) &ip->daddr, &tun_selfpeer) !=
          0)
        return;
      if (router_checksrc((struct in_addr *) &ip->saddr, session->peer) !=
          0)
        return;
      tundev_write(buffer, buffer_len);
      return;
    }
//else if (ip->version == 6) //IPv6 Packet
  else if (ip->version == PROTOCOL1_V)  //Internal packets v1
    {
      if (ip->ihl == PROTOCOL1_IDA && session->peer != NULL)
        {
          session->peer->stat |= PEER_STAT_IDK;
          //Packets are combinable IDACK+ID
          begin = begin + sizeof(struct protocol_1_s);
          if (begin + 4 > buffer_len)
            return;
          ip = (struct iphdr *) &buffer[begin];
        }

      if (ip->ihl == PROTOCOL1_ID)
        {
          if (session->peer == NULL)
            session->peer = peer_create();
          peer = session->peer;
          peer->udpsrvsession = session;
          if ((peer->stat & PEER_STAT_ID) != 0)
            {
              if (protocol_processpeer(peer,
                                       &((struct protocol_1id_s *)
                                         &buffer[begin])->peer,
                                       buffer_len - begin -
                                       sizeof(struct protocol_1id_s) +
                                       sizeof(struct protocol_peer_s)) < 0)
                {
                  //-TODO: Check
                  peer_destroy(peer);
                  return;
                }
              /* Check for valid routes shared */
              if (protocol_checknameconstraints(peer) < 0)
                {
                  peer_destroy(peer);
                  return;
                }
              peer->stat |= PEER_STAT_ID;
              if (peer_add(peer, session) < 1)
                peer_destroy(peer);
            }
          //-TODO: JOIN packets
          protocol_sendpacket(session, PROTOCOL1_IDA);
          if ((peer->stat & PEER_STAT_IDK) == 0)
            protocol_sendpacket(session, PROTOCOL1_ID);
          //Packets are combinable ID+KA
          begin = begin + sizeof(struct protocol_1id_s) +
            ((struct protocol_1id_s *) &buffer[begin])->peer.len_addr *
            sizeof(struct protocol_netpair_s) +
            ((struct protocol_1id_s *) &buffer[begin])->peer.len_addr *
            sizeof(struct protocol_addrpair_s);
          if (begin + 4 > buffer_len)
            return;
          ip = (struct iphdr *) &buffer[begin];
        }
      if (ip->ihl == PROTOCOL1_KA && session->peer != NULL &&
          (session->peer->stat & PEER_STAT_ID) != 0)
        {
          p = (void *) &buffer[begin] + sizeof(struct protocol_1ka_s);
          while (p <= (void *) &buffer[buffer_len - 1])
            {
              peer = peer_create();
              i = protocol_processpeer(peer, (struct protocol_peer_s *) p,
                                       (int) ((void *)
                                              &buffer[buffer_len - 1] -
                                              p));
              if (i < 0)
                {
                  peer_destroy(peer);
                  return;
                }
              p += i;
              //if (router_exist () == 0)
              //-TODO Init conn
              //else
              peer_destroy(peer);
            }
        }
    }
}

int protocol_sendframe(const char *buffer, const int buffer_len)
{
  struct iphdr *ip = NULL;
  struct peer_s *dstpeer = NULL;
  if (buffer_len < 20)
    return -1;
  ip = (struct iphdr *) buffer;
  //Check for IPv4
  if (ip->version == 4)
    {
      if (router_checksrc((struct in_addr *) &ip->saddr, &tun_selfpeer) ==
          0)
        {
          //-TODO: CHECK BROADCAST
          dstpeer = router_searchdst((struct in_addr *) &ip->daddr);
        }
      else
        log_error("Invalid source.\n");
    }
  //Check for IPv6
  else if (ip->version == 6)
    {
      log_error("IPv6 not implemented.\n");
      //ROUTING IPv6
    }
  else
    log_error("Unknow protocol not implemented.\n");
  if (buffer_len > 0 && dstpeer->udpsrvsession != NULL)
    {
      udpsrvdtls_write(buffer, buffer_len, dstpeer->udpsrvsession);
      return 0;
    }
  return -1;
}

int protocol_sendpacket(struct udpsrvsession_s *session, const int type)
{
  char *packet = NULL;
  int packet_len = -1;
  switch (type)
    {
      case PROTOCOL1_ID:
        packet = (char *) protocol_v1id;
        packet_len = protocol_v1id_len;
        break;
      case PROTOCOL1_IDA:
        packet = (char *) &protocol_v1ida;
        packet_len = protocol_v1ida_len;
        break;
      case PROTOCOL1_KA:
        packet = (char *) protocol_v1ka;
        packet_len = protocol_v1ka_len;
        break;
    }
  if (packet_len < 1 && session != NULL)
    {
      udpsrvdtls_write(packet, packet_len, session);
      return 0;
    }
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
  protocol_v1ka->packetid = PROTOCOL1_KA;
  protocol_v1ka->len = i;
  protocol_v1ka_len =
    sizeof (struct protocol_1ka_s) + i * sizeof (struct protocol_addrpair_s);
  protocol_v1ka_pos += i;
*/
}

void protocol_init()
{
  struct protocol_addrpair_s *addrpair;
  struct protocol_netpair_s *netpair;
  int i;
  protocol_v1ida.version = PROTOCOL1_V;
  protocol_v1ida.ihl = PROTOCOL1_IDA;
  protocol_v1id_len = sizeof(struct protocol_1id_s) +
    tun_selfpeer.shared_networks_len * sizeof(struct protocol_netpair_s) +
    tun_selfpeer.addrs_len * sizeof(struct protocol_addrpair_s);
  protocol_v1id = malloc(protocol_v1id_len);
  protocol_v1id->base.ihl = PROTOCOL1_ID;
  protocol_v1id->base.version = PROTOCOL1_V;
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
  struct in_network net;
  int i, j;
  const unsigned char *p;
  void *ext_str = NULL;
  X509 *cert;
  STACK_OF(X509_EXTENSION) * exts;
  X509_EXTENSION *ext;
  X509V3_EXT_METHOD *method;
  STACK_OF(GENERAL_SUBTREE) * trees;
  GENERAL_SUBTREE *tree;

  cert = SSL_get_peer_certificate(peer->udpsrvsession->dtls);
  exts = cert->cert_info->extensions;
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
                      net.addr.s_addr =
                        *((uint32_t *) tree->base->d.ip->data);
                      net.netmask.s_addr =
                        *((uint32_t *) & tree->base->d.ip->data[4]);
                      //-TODO
                    }
//else if(len == 32) //IPv6
//  See openssl/crypto/x509v3/v3_ncons.c:static int print_nc_ipadd()
//else //DNS
//  GENERAL_NAME_print(bp, tree->base);
                }
            }
        }
    }
  return 0;
}
