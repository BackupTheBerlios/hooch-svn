#
# $Id: Makefile.inc 36 2004-06-15 14:55:57Z sjamaan $
#
# Globally included Makefile for Camille.  Contains user configuration.
#

############################
# System dependent options #
############################

# Change if necessary.  pkgsrc, for example, will set this for you.
PREFIX?=		/usr/local
LIBDIR=			${PREFIX}/lib
BINDIR=			${PREFIX}/bin
MANDIR=			${PREFIX}/share/man

# Treat C and lint warnings as errors.
CFLAGS+=	-g -ansi -pedantic-errors -Wall -Werror
LINTFLAGS+=	-w

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable debug code?
DEFS+=		-DDEBUG
