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

void
udpsrvdtls ()
{
//  SSL_load_error_strings ();  /* readable error messages */
//  SSL_library_init ();                        /* initialize library */
//actions_to_seed_PRNG();
/*mClientCtx = SSL_CTX_new (DTLSv1_client_method ());
  mServerCtx = SSL_CTX_new (DTLSv1_server_method ());
  assert (mClientCtx);
  assert (mServerCtx);
  SSL *ssl;
  BIO *wBio;
  int retry = 0;
  ssl = mDtlsConnections[*((struct sockaddr_in *) &peer)];

  if (ssl == NULL)
    {
      ssl = SSL_new (mClientCtx);
      assert (ssl);

      SSL_set_connect_state (ssl);

      wBio = BIO_new_dgram (mFd, BIO_NOCLOSE);
      assert (wBio);

      BIO_dgram_set_peer (wBio, &peer);
*/
      /* the real rbio will be set by _read */
//    SSL_set_bio (ssl, mDummyBio, wBio);

      /* we should be ready to take this out if the 
       * connection fails later */
/*    mDtlsConnections[*((struct sockaddr_in *) &peer)] = ssl;
    }
  expected = sendData->data.size ();

  count = SSL_write (ssl, sendData->data.data (), sendData->data.size ());
*/
}
