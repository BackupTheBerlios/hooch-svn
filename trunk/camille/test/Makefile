#
# $Id$
#
# Camille testing suite Makefile
#

PROG= test
MAN=
CFLAGS+= -I.. -lgune
LDADD= -L../camille -R../camille -lcamille

# XXX HACK: Clean this up
CFLAGS+= -I../../../gune -R../../../gune -L../../../gune

.include <bsd.prog.mk>
