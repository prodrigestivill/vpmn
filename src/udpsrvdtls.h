/***************************************************************************
 *            udpsrvdtls.h
 *
 *  Thu Feb  7 10:33:36 2008
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
int udpsrvdtls_sessionerr (const unsigned long err,
			   struct udpsrvsession_t *session);

#endif /* _UDPSRVDTLS_H */
