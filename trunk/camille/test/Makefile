#
# $Id$
#
# Camille testing suite Makefile
#

CAMILLEPATH= ..
GUNEPATH= ../../../gune

PROG= test
MAN=
CFLAGS+= -I${CAMILLEPATH} -I${CAMILLEPATH}/camille
LDADD= -L${CAMILLEPATH} -R${CAMILLEPATH} -L${CAMILLEPATH}/camille -R${CAMILLEPATH}/camille
LDADD+= -lcamille -lgune

# XXX HACK: Clean this up
LDADD+= -L${GUNEPATH} -R${GUNEPATH} -L${GUNEPATH}/gune -R${GUNEPATH}/gune
CFLAGS+= -I${GUNEPATH} -I${GUNEPATH}/gune

.include <bsd.prog.mk>
