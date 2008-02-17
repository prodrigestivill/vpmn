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
	udpsrvsessiontest \
	tundevtest \
	routertest

routertest_SOURCES = \
	../src/config.c\
	../src/config.h \
	../src/debug.c \
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/router.c \
	../src/router.h \
	routertest.c \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h

tundevtest_SOURCES = \
	tundevtest.c\
	../src/debug.c \
	../src/debug.h \
	../src/config.h \
	../src/config.c \
	../src/tundev.h \
	../src/tundev.c \
	../src/tunsrv.c \
	../src/router.c \
	../src/router.h \
	../src/peer.c \
	../src/peer.h \
	../src/tunsrvthread.c \
	../src/tunsrvthread.h

tundevtest_LDFLAGS = \
	-lpthread

udpsrvsessiontest_SOURCES = \
	udpsrvsessiontest.c\
	../src/debug.c \
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h \
	../src/udpsrvdtls.c \
	../src/udpsrvdtls.h \
	../src/config.c \
	../src/config.h

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
	../src/udpsrvthread.h \
	../src/config.h \
	../src/config.c

udpsrvtest_LDFLAGS = \
	-lpthread

