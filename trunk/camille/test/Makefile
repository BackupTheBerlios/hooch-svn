#
# $Id$
#
# Camille testing suite Makefile
#

PROG= test
MAN=
CFLAGS+= -I..
LDADD= -L../camille -R../camille
LDADD+= -lcamille -lgune

# XXX HACK: Clean this up
LDADD+= -L../../../gune/gune -R../../../gune/gune
CFLAGS+= -I../../../gune

.include <bsd.prog.mk>
