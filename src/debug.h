/***************************************************************************
 *            debug.h
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


#ifndef _DEBUG_H
#define _DEBUG_H

#define DEBUG 3

#include <stdio.h>
#define log_fail(format, ...) { \
fprintf (stderr, format, ## __VA_ARGS__);\
fflush(stderr);\
exit(128);\
}
#if DEBUG > 0
#define log_print(s, format, ...) { \
fprintf (s, format, ## __VA_ARGS__);\
fflush(s);\
}
#endif
#if DEBUG > 2                   // DEBUG = 3
#define log_debug(format, ...) log_print (stderr, format, ## __VA_ARGS__)
#else
#define log_debug(args...)
#endif
#if DEBUG > 1                   // DEBUG = 2
#define log_info(format, ...)  log_print (stderr, format, ## __VA_ARGS__)
#else
#define log_info(args...)
#endif
#if DEBUG > 0                   // DEBUG = 1
#define log_error(format, ...) log_print (stderr, format, ## __VA_ARGS__)
#else
#define log_error(args...)
#endif

#endif                          /* _DEBUG_H */
