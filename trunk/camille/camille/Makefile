#
# $Id$
#
# Camille library Makefile
#

LIB= camille
CAMILLESRCS= contacts.c
GENSRCS= parser.c parser_lex.c parser.h
SRCS= ${CAMILLESRCS} ${GENSRCS}

# Yacc settings
YHEADER=1
YFLAGS+= -v

CFLAGS+= -I..
LINTFLAGS+= -I..
# Clean this up
LINTFLAGS+= -I../../../gune
CFLAGS+= -I../../../gune

CLEANFILES+= ${GENSRCS} y.output

.include <bsd.lib.mk>
