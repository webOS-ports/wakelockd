NULL =

AM_MAKEFLAGS = --no-print-directory

sbin_PROGRAMS = src/wakelockd

src_wakelockd_SOURCES = \
	src/main.c

src_wakelockd_LDADD = \
	-ldl

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 configure config.h.in config.sub config.guess \
	ltmain.sh depcomp compile missing install-sh mkinstalldirs

