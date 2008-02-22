/***************************************************************************
 *            udpsrvdtls.h
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

#ifndef _UDPSRVDTLS_H
#define _UDPSRVDTLS_H

#include "udpsrvsession.h"

void udpsrvdtls_init ();
int udpsrvdtls_loadcerts (const char *cafile, const char *certfile,
			  const char *pkeyfile);
int udpsrvdtls_read (const char *buffer, const int buffer_len,
		     char *bufferout, const int bufferout_len,
		     struct udpsrvsession_t *session);
int udpsrvdtls_write (const char *buffer, const int buffer_len,
		      struct udpsrvsession_t *session);
void udpsrvdtls_sessionerr (const unsigned long err,
			   struct udpsrvsession_t *session);

#endif /* _UDPSRVDTLS_H */
