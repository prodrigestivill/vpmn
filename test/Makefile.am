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
	udpsrvtest\
	udpsrvsessiontest

udpsrvsessiontest_SOURCES = \
	udpsrvsessiontest.c\
	../src/debug.c \
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h \
	../src/udpsrvdtls.c \
	../src/udpsrvdtls.h

udpsrvsessiontest_LDFLAGS = \
	-lpthread

udpsrvtest_SOURCES = \
	udpsrvtest.c \
	../src/debug.c \
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/udpsrv.c \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h \
	../src/udpsrvthread.c \
	../src/udpsrvthread.h

udpsrvtest_LDFLAGS = \
	-lpthread

