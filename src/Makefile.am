## Process this file with automake to produce Makefile.in

## Created by Anjuta

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(VPMN_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = \
	udpsrv\
	vpmnd

vpmnd_SOURCES = 

udpsrv_SOURCES = \
	udpsrv.c \
	debug.c \
	debug.h \
	peer.h \
	udpsrvsession.c \
	udpsrvsession.h \
	udpsrvthread.c \
	udpsrvthread.h

udpsrv_LDFLAGS = \
	-lpthread

