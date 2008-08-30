/***************************************************************************
 *            udpsrvdtls.c
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

#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include "config.h"
#include "timeout.h"
#include "debug.h"
#include "udpsrvdtls.h"
#include "udpsrvsession.h"
#include "srv.h"

SSL_CTX *udpsrvdtls_clictx, *udpsrvdtls_srvctx;

void udpsrvdtls_init()
{
  int verifymode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
#ifdef DEBUG
  SSL_load_error_strings();
  ERR_load_crypto_strings();
#endif
  SSL_library_init();
  //-TODO: sactions_to_seed_PRNG();
  udpsrvdtls_clictx = SSL_CTX_new(DTLSv1_client_method());
  if (SSL_CTX_set_cipher_list(udpsrvdtls_clictx, ssl_cipherlist) != 1)
    log_error("Error setting cipher list.\n");
  SSL_CTX_set_verify(udpsrvdtls_clictx, verifymode, NULL);
  SSL_CTX_set_verify_depth(udpsrvdtls_clictx, ssl_verifydepth);
  SSL_CTX_set_options(udpsrvdtls_clictx, SSL_OP_NO_QUERY_MTU);
  SSL_CTX_set_mode(udpsrvdtls_clictx, SSL_MODE_AUTO_RETRY);
  //SSL_CTX_set_read_ahead(udpsrvdtls_clictx, 1);
  udpsrvdtls_srvctx = SSL_CTX_new(DTLSv1_server_method());
  if (SSL_CTX_set_cipher_list(udpsrvdtls_srvctx, ssl_cipherlist) != 1)
    log_error("Error setting cipher list.\n");
  SSL_CTX_set_verify(udpsrvdtls_srvctx, verifymode, NULL);
  SSL_CTX_set_verify_depth(udpsrvdtls_srvctx, ssl_verifydepth);
  SSL_CTX_set_options(udpsrvdtls_srvctx, SSL_OP_NO_QUERY_MTU);
  SSL_CTX_set_mode(udpsrvdtls_srvctx, SSL_MODE_AUTO_RETRY);
  //SSL_CTX_set_read_ahead(udpsrvdtls_srvctx, 1);
}

int
udpsrvdtls_loadcerts(const char *cafile, const char *certfile,
                     const char *pkeyfile)
{
  if (SSL_CTX_use_certificate_file
      (udpsrvdtls_clictx, certfile, SSL_FILETYPE_PEM) != 1)
    return -2;
  if (SSL_CTX_use_PrivateKey_file
      (udpsrvdtls_clictx, pkeyfile, SSL_FILETYPE_PEM) != 1)
    return -2;
  if (SSL_CTX_use_certificate_file
      (udpsrvdtls_srvctx, certfile, SSL_FILETYPE_PEM) != 1)
    return -2;
  if (SSL_CTX_use_PrivateKey_file
      (udpsrvdtls_srvctx, pkeyfile, SSL_FILETYPE_PEM) != 1)
    return -2;
  if (SSL_CTX_load_verify_locations(udpsrvdtls_clictx, cafile, NULL) != 1)
    return -1;
  if (SSL_CTX_load_verify_locations(udpsrvdtls_srvctx, cafile, NULL) != 1)
    return -1;
  STACK_OF(X509_NAME) * calist = SSL_load_client_CA_file(cafile);
  if (calist == NULL)
    return -1;
  SSL_CTX_set_client_CA_list(udpsrvdtls_srvctx, calist);
  return 0;
}

void udpsrvdtls_destroy(struct udpsrvsession_s *session)
{
  pthread_mutex_lock(&session->bioread_mutex);
  pthread_mutex_lock(&session->dtls_mutex);
  SSL_free(session->dtls);
  BIO_free(session->bioread);
  session->dtls = NULL;
  if (session->peer != NULL)
    peer_destroy(session->peer);        //-TODO: Check
  pthread_mutex_unlock(&session->dtls_mutex);
  pthread_mutex_unlock(&session->bioread_mutex);
}

void udpsrvdtls_create(struct udpsrvsession_s *session, int isserver)
{
  BIO *wbio, *rbio;
  if (isserver)
    {
      session->dtls = SSL_new(udpsrvdtls_srvctx);
      SSL_set_accept_state(session->dtls);
    }
  else
    {
      session->dtls = SSL_new(udpsrvdtls_clictx);
      SSL_set_connect_state(session->dtls);
    }
  wbio = BIO_new_dgram(udpsrv_fd, BIO_NOCLOSE);
  BIO_dgram_set_peer(wbio, session->addr);
  pthread_mutex_lock(&session->bioread_mutex);
  BIO_new_bio_pair(&session->bioread, UDPBUFFERSIZE, &rbio, 1);
  pthread_mutex_unlock(&session->bioread_mutex);
  SSL_set_bio(session->dtls, rbio, wbio);
  //SSL_set_options(session->dtls, SSL_OP_NO_QUERY_MTU);
  SSL_set_mtu(session->dtls, SSL3_RT_MAX_PLAIN_LENGTH);
}

int udpsrvdtls_write(const char *buffer, const int buffer_len,
                     struct udpsrvsession_s *session)
{
  int len, retry = 0;
  unsigned long err;

  if (session == NULL)
    return -1;
  pthread_mutex_lock(&session->dtls_mutex);
  if (session->dtls == NULL)
    udpsrvdtls_create(session, 0);
  do
    {
      if (retry > 0)
        pthread_mutex_lock(&session->dtls_mutex);
      len = SSL_write(session->dtls, buffer, buffer_len);
      if (len <= 0)
        err = SSL_get_error(session->dtls, len);
      pthread_mutex_unlock(&session->dtls_mutex);
      if (len <= 0
          && (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE))
        {
          sleep(1);             //-TODO: Improve DTLS1_get_timeout()
          retry++;
        }
      else
        retry = 0;
      if (len != buffer_len)
        log_error("Write %d != %d\n", len, buffer_len);
    }
  while (retry > 0 && retry < 5 && session->dtls != NULL);
  if (len <= 0)
    {
      log_error("err write\n");
      udpsrvdtls_sessionerr(err, session);
    }
  return len;
}


int udpsrvdtls_read(const char *buffer, const int buffer_len,
                    char *bufferout, const int bufferout_len,
                    struct udpsrvsession_s *session)
{
  int len, tlen = 0;
  if (session == NULL)
    return -1;
  if (session->dtls == NULL)
    {
      pthread_mutex_lock(&session->dtls_mutex);
      if (session->dtls == NULL)
        udpsrvdtls_create(session, 1);
      pthread_mutex_unlock(&session->dtls_mutex);
    }

  pthread_mutex_lock(&session->bioread_mutex);
  len = BIO_write(session->bioread, (void *) buffer, buffer_len);
  if (len != buffer_len)
    log_error("BIO_write error %d\n", len);     //TODO-retry?
  BIO_flush(session->bioread);
  //log_debug("BIOPending %d\n", BIO_pending(session->dtls->rbio));
  pthread_mutex_unlock(&session->bioread_mutex);

  pthread_mutex_lock(&session->dtls_mutex);
  do
    {
      len =
        SSL_read(session->dtls, bufferout + tlen, bufferout_len - tlen);
      log_debug("Read: %d\n", len);
      if (len > 0)
        tlen += len;
      /*else
         udpsrvdtls_sessionerr(SSL_get_error(session->dtls, len), session); */
    }
  while (len > 0 && tlen + tunmtu < bufferout_len
         && SSL_pending(session->dtls) > 0);
  pthread_mutex_unlock(&session->dtls_mutex);
  return tlen;
}

void udpsrvdtls_sessionerr(const unsigned long err,
                           struct udpsrvsession_s *session)
{
  switch (err)
    {
      case SSL_ERROR_NONE:
        break;
      case SSL_ERROR_SSL:
        log_error("SSL_ERROR_SSL: %s\n",
                  ERR_reason_error_string(ERR_get_error()));
        break;
      case SSL_ERROR_WANT_READ:
        log_error("SSL_ERROR_WANT_READ\n");
        break;
      case SSL_ERROR_WANT_WRITE:
        log_error("SSL_ERROR_WANT_WRITE\n");
        break;
      case SSL_ERROR_SYSCALL:
        log_error("SSL_ERROR_SYSCALL\n");
        break;
        /* connection closed */
      case SSL_ERROR_ZERO_RETURN:
        log_error("SSL_ERROR_ZERO_RETURN\n");
        udpsrvdtls_destroy(session);
        break;
      case SSL_ERROR_WANT_CONNECT:
        log_error("SSL_ERROR_WANT_CONNECT\n");
        break;
      case SSL_ERROR_WANT_ACCEPT:
        log_error("SSL_ERROR_WANT_ACCEPT\n");
        break;
      default:
        log_error("SSL_ERROR_UNKNOWN\n");
        break;
    }
}
