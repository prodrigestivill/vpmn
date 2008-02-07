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
	vpmnd

vpmnd_SOURCES = \
	debug.c\
	debug.h \
	peer.c \
	peer.h \
	udpsrv.c \
	udpsrvsession.c \
	udpsrvsession.h \
	udpsrvthread.c \
	udpsrvthread.h \
	udpsrvdtls.c \
	udpsrvdtls.h

vpmnd_LDFLAGS = \
	-lpthread

