dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(vpmn, 0.1, http://vpmn.googlecode.com/)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC






AM_PROG_LIBTOOL



PKG_CHECK_MODULES(VPMN, [libssl >= 0.9.8f])
AC_SUBST(VPMN_CFLAGS)
AC_SUBST(VPMN_LIBS)


dnl Check and set OS

#AC_CANONICAL_HOST

case $host_os in
  *linux*)
    AC_DEFINE(HAVE_LINUX, 1, [Linux])
    [ rm -f src/tundev.c; ln -sf linux/tundev.c src/tundev.c ]
  ;;
  *)
    AC_MSG_ERROR("Unknown operating system.")
  ;;
esac

AC_OUTPUT([
Makefile
src/Makefile
test/Makefile
])
