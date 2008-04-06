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
BIO *udpsrvdtls_mbio;

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
  udpsrvdtls_srvctx = SSL_CTX_new(DTLSv1_server_method());
  if (SSL_CTX_set_cipher_list(udpsrvdtls_srvctx, ssl_cipherlist) != 1)
    log_error("Error setting cipher list.\n");
  SSL_CTX_set_verify(udpsrvdtls_srvctx, verifymode, NULL);
  SSL_CTX_set_verify_depth(udpsrvdtls_srvctx, ssl_verifydepth);
  udpsrvdtls_mbio = BIO_new(BIO_s_mem());
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

int udpsrvdtls_write(const char *buffer, const int buffer_len,
                     struct udpsrvsession_s *session)
{
  int len, retry = 0;
  unsigned long err;
  BIO *wbio;
  pthread_mutex_lock(&session->dtls_mutex);
  if (session->dtls == NULL)
    {
      session->dtls = SSL_new(udpsrvdtls_clictx);
      SSL_set_connect_state(session->dtls);
      wbio = BIO_new_dgram(udpsrv_fd, BIO_NOCLOSE);
      BIO_dgram_set_peer(wbio, session->addr);
      SSL_set_bio(session->dtls, udpsrvdtls_mbio, wbio);
      log_debug("New session created.\n");
    }
  //-TODO: Need to lock mutex on write?
  do
    {
      if (retry > 0)
        pthread_mutex_lock(&session->dtls_mutex);
      len = SSL_write(session->dtls, buffer, buffer_len);
      pthread_mutex_unlock(&session->dtls_mutex);
      err = SSL_get_error(session->dtls, len);
      if (len <= 0
          && (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE))
        retry++;
      else
        retry = 0;
    }
  while (retry > 0 && retry < 5 && session->dtls != NULL);
  if (len <= 0)
    udpsrvdtls_sessionerr(err, session);
  return len;
}

int udpsrvdtls_read(const char *buffer, const int buffer_len,
                    char *bufferout, const int bufferout_len,
                    struct udpsrvsession_s *session)
{
  int len, retry = 0;
  unsigned long err;
  BIO *wbio, *rbio;
  if (session == NULL)
    return -1;
  pthread_mutex_lock(&session->dtls_mutex);
  if (session->dtls == NULL)
    {
      session->dtls = SSL_new(udpsrvdtls_srvctx);
      SSL_set_accept_state(session->dtls);
      wbio = BIO_new_dgram(udpsrv_fd, BIO_NOCLOSE);
      BIO_dgram_set_peer(wbio, session->addr);
      SSL_set_bio(session->dtls, NULL, wbio);
    }
  rbio = BIO_new_mem_buf((void *) buffer, buffer_len);
  BIO_set_mem_eof_return(rbio, -1);
  session->dtls->rbio = rbio;
  do
    {
      len = SSL_read(session->dtls, bufferout, bufferout_len);
      err = SSL_get_error(session->dtls, len);
      if (len <= 0
          && (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE))
        retry++;
      else
        retry = 0;
    }
  while (retry > 0 && retry < 5);
  BIO_free(session->dtls->rbio);
  session->dtls->rbio = udpsrvdtls_mbio;
  pthread_mutex_unlock(&session->dtls_mutex);
  if (len > 0 && retry == 0)
    timeout_update(&session->timeout);
  else
    udpsrvdtls_sessionerr(err, session);
  return len;
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
        break;
      case SSL_ERROR_WANT_WRITE:
        break;
      case SSL_ERROR_SYSCALL:
        //break;
        /* connection closed */
      case SSL_ERROR_ZERO_RETURN:
        log_error("SSL_ERROR_ZERO_RETURN\n");
        pthread_mutex_lock(&session->dtls_mutex);
        CRYPTO_add(&udpsrvdtls_mbio->references, 1, CRYPTO_LOCK_BIO);
        SSL_free(session->dtls);
        session->dtls = NULL;
        pthread_mutex_unlock(&session->dtls_mutex);
        udpsrvsession_destroy(session);
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
