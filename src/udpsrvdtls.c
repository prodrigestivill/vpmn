/***************************************************************************
 *            udpsrvdtls.c
 *
 *  Thu Feb  7 10:32:56 2008
 *  Copyright  2008  Pau Rodriguez-Estivill
 *  <prodrigestivill@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <openssl/ssl.h>
#include <sys/socket.h>
#include "config.h"
#include "debug.h"
#include "udpsrvsession.h"
#include "srv.h"

BIO *udpsrvdtls_mbio;

void
udpsrvdtls_init()
{
//  SSL_load_error_strings ();  /* readable error messages */
//  SSL_library_init ();                        /* initialize library */
//actions_to_seed_PRNG();
   udpsrvdtls_mbio = BIO_new (BIO_s_mem()) ;
}

int
udpsrvdtls_write (const char *buffer, const int buffer_len,
                  struct udpsrvsession_t* session)
{
  int len;
  SSL_CTX *clictx;
  BIO *wbio;
  SSL *ssl = session->dtls;
  struct sockaddr_in *peeraddr = session->addr;

  if (ssl == NULL)
    {
      clictx = SSL_CTX_new (DTLSv1_client_method ());
      ssl = SSL_new (clictx);
      SSL_set_connect_state (ssl);
      
      if (!ssl_cert || !ssl_pkey)
         return -2;
      if (!SSL_use_certificate (ssl, ssl_cert) ||
          !SSL_use_PrivateKey (ssl, ssl_pkey))
         return -2;

      wbio = BIO_new_dgram (udpsrv_fd, BIO_NOCLOSE);
      BIO_dgram_set_peer (wbio, peeraddr);
      SSL_set_bio (ssl, udpsrvdtls_mbio, wbio);
      session->dtls = ssl;
    }
   len = SSL_write (ssl, buffer, buffer_len);
   if ( len <= 0 )
   {
      int err = SSL_get_error (ssl, len);
      switch (err)
      {
         case SSL_ERROR_NONE:
            break;
         case SSL_ERROR_SSL:
            break;
         case SSL_ERROR_WANT_READ:
            //retry = 1 ;
            break;
         case SSL_ERROR_WANT_WRITE:         
            //retry = 1 ;
            break;
         case SSL_ERROR_SYSCALL:
            log_error ("SSL_ERROR_SYSCALL\n");
            return -3;
            break;
         case SSL_ERROR_ZERO_RETURN:
            CRYPTO_add (&udpsrvdtls_mbio->references, 1, CRYPTO_LOCK_BIO);
            SSL_free (ssl);
            break ;
         case SSL_ERROR_WANT_CONNECT:
            break;
         case SSL_ERROR_WANT_ACCEPT:
            break;
         default:
            break;
      }
   }
   return len;
}

int
udpsrvdtls_read (const char *buffer, const int buffer_len,
                 char *bufferout, struct udpsrvsession_t* session)
{
  int len, err;
  SSL_CTX *srvctx;
  BIO *wbio, *rbio;
  SSL *ssl = session->dtls;
  struct sockaddr_in *peeraddr = session->addr;
    
  if (ssl == NULL)
   {
      srvctx = SSL_CTX_new (DTLSv1_server_method ());
      ssl = SSL_new (srvctx);
      SSL_set_accept_state (ssl);

      if (!ssl_cert || !ssl_pkey)
         return -2;
      if( !SSL_use_certificate (ssl, ssl_cert) ||
          !SSL_use_PrivateKey (ssl, ssl_pkey))
         return -2;
       
      wbio = BIO_new_dgram (udpsrv_fd, BIO_NOCLOSE);
      BIO_dgram_set_peer (wbio, peeraddr);
      SSL_set_bio (ssl, NULL, wbio);
      session->dtls = ssl;
   }
   rbio = BIO_new_mem_buf ((void *) buffer, buffer_len);
   BIO_set_mem_eof_return (rbio, -1 );
   ssl->rbio = rbio ;
   len = SSL_read (ssl, bufferout, UDPBUFFERSIZE);
   err = SSL_get_error (ssl, len);
   BIO_free (ssl->rbio);
   ssl->rbio = udpsrvdtls_mbio;
   if (len <= 0)
   {
      switch (err)
      {
         case SSL_ERROR_NONE:
            break;
         case SSL_ERROR_SSL:
            break;
         case SSL_ERROR_WANT_READ:
            break;
         case SSL_ERROR_WANT_WRITE:
            break;
         case SSL_ERROR_SYSCALL:
            break;
            /* connection closed */
         case SSL_ERROR_ZERO_RETURN:
            CRYPTO_add (&udpsrvdtls_mbio->references, 1, CRYPTO_LOCK_BIO);
            SSL_free (ssl);
            break;
         case SSL_ERROR_WANT_CONNECT:
            break;
         case SSL_ERROR_WANT_ACCEPT:
            break;
         default:
            break;
      }
      return -5;
   }
   /*if (SSL_in_init(ssl))
      DtlsReceiveTimeout */
   return len;    
}
