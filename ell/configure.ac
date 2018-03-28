AC_INIT([D-Bus Object Manager], 1.0)
AM_INIT_AUTOMAKE
AC_PROG_CC

AC_CONFIG_HEADERS(config.h)

AM_MAINTAINER_MODE

PKG_CHECK_MODULES(ELL, ell,
  [AC_DEFINE([HAVE_ELL],[1],[Use ELL])],
  [AC_MSG_ERROR("ell missing")])
AC_SUBST(ELL_CFLAGS)
AC_SUBST(ELL_LIBS)

AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug],
			[enable compiling with debugging information]), [
	if (test "${enableval}" = "yes" &&
				test "${ac_cv_prog_cc_g}" = "yes"); then
		CFLAGS="$CFLAGS -g"
	fi
])

AC_CONFIG_FILES(Makefile)
AC_OUTPUT