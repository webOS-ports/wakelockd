NULL =

AM_MAKEFLAGS = --no-print-directory

AM_CFLAGS = @GLIB_CFLAGS@ @LIBEVDEV_CFLAGS@

sbin_PROGRAMS = src/wakelockd

src_wakelockd_SOURCES = \
	src/main.c \
	src/power_key_resume_handler.c \
	src/rtc_resume_handler.c

src_wakelockd_LDADD = \
	@GLIB_LIBS@ \
	@LIBEVDEV_LIBS@ \
	-ldl \
	-lsuspend

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 configure config.h.in config.sub config.guess \
	ltmain.sh depcomp compile missing install-sh mkinstalldirs

