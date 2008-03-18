/***************************************************************************
 *            debug.c
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

#include "config.h"

#ifdef DEBUG
#include <stdio.h>
#endif

void log_debug(const char *format, void **args)
{
#ifdef DEBUG
  fprintf(stdout, format, args);
  fflush(stdout);
#endif
}

void log_info(const char *format, void **args)
{
#ifdef DEBUG
  fprintf(stdout, format, args);
  fflush(stdout);
#endif
}

void log_error(const char *format, void **args)
{
#ifdef DEBUG
  fprintf(stderr, format, args);
  fflush(stderr);
#endif
}
