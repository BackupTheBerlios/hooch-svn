#
# $Id$
#
# Camille library Makefile
#

LIB= camille
CAMILLESRCS= contacts.c parser.y
GENSRCS= parser.c parser_lex.c parser.h
SRCS= ${CAMILLESRCS} ${GENSRCS}

# Yacc settings
YHEADER=1
YFLAGS+= -v

CFLAGS+= -I.. -lgune
LINTFLAGS+= -I..

# XXX HACK: Clean this up
LINTFLAGS+= -I../../../gune
CFLAGS+= -I../../../gune -R../../../gune -L../../../gune

CLEANFILES+= ${GENSRCS} y.output

.include <bsd.lib.mk>
