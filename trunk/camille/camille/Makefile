#
# $Id$
#
# Camille library Makefile
#

LIB= camille
CAMILLESRCS= addrbook.c groups.c contacts.c parser.y
GENSRCS= parser.c parser_lex.c parser.h
SRCS= ${CAMILLESRCS} ${GENSRCS}

# Yacc settings
YHEADER=1
YFLAGS+= -v

LDADD= -lgune
CFLAGS+= -I.. ${DEFS}
LINTFLAGS+= -I..

# XXX HACK: Clean this up
LINTFLAGS+= -I../../../gune
LDADD+= -R../../../gune -L../../../gune
CFLAGS+= -I../../../gune

CLEANFILES+= ${GENSRCS} y.output

.include <bsd.lib.mk>
