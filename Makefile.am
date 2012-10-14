NULL =

AM_MAKEFLAGS = --no-print-directory

AM_CFLAGS = @GLIB_CFLAGS@

sbin_PROGRAMS = src/wakelockd

src_wakelockd_SOURCES = \
	src/main.c

src_wakelockd_LDADD = \
	@GLIB_LIBS@ \
	-ldl

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 configure config.h.in config.sub config.guess \
	ltmain.sh depcomp compile missing install-sh mkinstalldirs

