#
# $Id$
#
# Camille library Makefile
#

LIB= camille
CAMILLESRCS= contacts.c
GENSRCS= parser.c
SRCS= ${CAMILLESRCS} ${GENSRCS}

# Yacc settings
#YHEADER=	1
YFLAGS+= -v -d

CLEANFILES+= ${GENSRCS} y.output y.tab.h parser.h

.include <bsd.lib.mk>
