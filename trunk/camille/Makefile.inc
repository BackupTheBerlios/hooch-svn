#
# $Id: Makefile.inc 36 2004-06-15 14:55:57Z sjamaan $
#
# Globally included Makefile for Camille.  Contains user configuration.
#

############################
# System dependent options #
############################

# Uncomment if necessary.  pkgsrc, for example, will set this for you.
# Without pkgsrc, default is /usr
#PREFIX=		/usr/local

CFLAGS+=	-ansi -pedantic-errors -Wall

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable debug code?
DEFS+=		-DDEBUG