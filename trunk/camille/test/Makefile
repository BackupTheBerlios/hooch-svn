#
# $Id$
#
# Camille testing suite Makefile
#

CAMILLEPATH= ..
CAMILLEPATH2= ../camille
GUNEPATH= ../../../gune
GUNEPATH2= ../../../gune/gune

PROG= test
MAN=
CFLAGS+= -I${CAMILLEPATH} -I${CAMILLEPATH2}
LDADD= -L${CAMILLEPATH} -R${CAMILLEPATH} -L${CAMILLEPATH2} -R${CAMILLEPATH2}
LDADD+= -lcamille -lgune

# XXX HACK: Clean this up
LDADD+= -L${GUNEPATH} -R${GUNEPATH} -L${GUNEPATH2} -R${GUNEPATH2}
CFLAGS+= -I${GUNEPATH} -I${GUNEPATH2}

.include <bsd.prog.mk>
