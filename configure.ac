AC_PREREQ(2.60)
AC_INIT(wakelockd, 1.11)

AM_INIT_AUTOMAKE([foreign subdir-objects color-tests dist-bzip2])
AM_CONFIG_HEADER(config.h)

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT(/usr/local)

PKG_PROG_PKG_CONFIG

AC_LANG_C

AC_C_RESTRICT

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AM_PROG_MKDIR_P

m4_define([_LT_AC_TAGCONFIG], [])
m4_ifdef([AC_LIBTOOL_TAGS], [AC_LIBTOOL_TAGS([])])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_ARG_ENABLE(optimization, AC_HELP_STRING([--disable-optimization],
			[disable code optimization through compiler]), [
	if (test "${enableval}" = "no"); then
		CFLAGS="$CFLAGS -O0"
	fi
])

AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug],
			[enable compiling with debugging information]), [
	if (test "${enableval}" = "yes" &&
				test "${ac_cv_prog_cc_g}" = "yes"); then
		CFLAGS="$CFLAGS -g"
	fi
])

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.28, dummy=yes,
				AC_MSG_ERROR(GLib >= 2.28 is required))
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(LIBEVDEV, libevdev >= 1.0, dummy=yes,
				AC_MSG_ERROR(libevdev >= 1.0 is required))
AC_SUBST(LIBEVDEV_CFLAGS)
AC_SUBST(LIBEVDEV_LIBS)

AC_OUTPUT([
	Makefile
])
