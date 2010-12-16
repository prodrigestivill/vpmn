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
	routertest \
	protocoltest \
	ptptest

ptptest_SOURCES = \
	ptptest.c\
	../src/config.c \
	../src/peer.c \
	../src/protocol.c \
	../src/router.c \
	../src/timeout.c \
	../src/tundev.c \
	../src/tunsrv.c \
	../src/udpsrv.c \
	../src/udpsrvdtls.c \
	../src/udpsrvsession.c \
	../src/config.h \
	../src/debug.h \
	../src/peer.h \
	../src/protocol.h \
	../src/router.h \
	../src/srv.h \
	../src/timeout.h \
	../src/tundev.h \
	../src/udpsrvdtls.h \
	../src/udpsrvsession.h

ptptest_LDFLAGS = \
	-lpthread \
	-lssl

protocoltest_SOURCES = \
	../src/config.c\
	../src/config.h\
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/timeout.c \
	../src/timeout.h \
	../src/protocol.c \
	../src/protocol.h \
	../src/router.c \
	../src/router.h \
	protocoltest.c \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h

protocoltest_LDFLAGS = \
	-lpthread \
	-lssl

routertest_SOURCES = \
	../src/config.c\
	../src/config.h \
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/timeout.c \
	../src/router.c \
	../src/router.h \
	routertest.c \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h

tundevtest_SOURCES = \
	tundevtest.c\
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
	../src/srv.h \
	../src/udpsrvdtls.h \
	../src/udpsrvsession.h

tundevtest_LDFLAGS = \
	-lpthread \
	-lssl

udpsrvsessiontest_SOURCES = \
	udpsrvsessiontest.c\
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h \
	../src/udpsrvdtls.c \
	../src/udpsrvdtls.h \
	../src/config.c \
	../src/config.h \
	../src/router.c \
	../src/router.h \
	../src/timeout.c \
	../src/timeout.h

udpsrvsessiontest_LDFLAGS = \
	-lpthread            \
	-lssl

udpsrvtest_SOURCES = \
	udpsrvtest.c \
	../src/debug.h \
	../src/peer.c \
	../src/peer.h \
	../src/udpsrv.c \
	../src/udpsrvsession.c \
	../src/udpsrvsession.h \
	../src/config.h \
	../src/config.c \
	../src/router.c \
	../src/router.h \
	../src/udpsrvdtls.c \
	../src/udpsrvdtls.h \
	../src/srv.h \
	../src/protocol.h \
	../src/timeout.c \
	../src/timeout.h

udpsrvtest_LDFLAGS = \
	-lpthread     \
	-lssl

