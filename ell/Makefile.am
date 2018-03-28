bin_PROGRAMS = src/objmgr
src_objmgr_SOURCES = src/main.c
src_objmgr_LDADD = @ELL_LIBS@

src_objmgr_LDFLAGS = $(AM_LDFLAGS)
src_objmgr_CFLAGS = $(AM_CFLAGS) @ELL_CFLAGS@

MAINTAINERCLEANFILES = Makefile.in \
	aclocal.m4 configure config.h.in config.sub config.guess \
	ltmain.sh depcomp compile missing install-sh