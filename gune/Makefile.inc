#
# $Id: Makefile.inc,v 1.1 2004/03/16 23:44:40 airhead Exp $
#
# Globally included Makefile for Gune.  Contains user configuration.
#

############################
# System dependent options #
############################

# Uncomment if necessary.  pkgsrc, for example, will set this for you.
# Without pkgsrc, default is /usr
#PREFIX=		/usr/local

CFLAGS+=	-pedantic-errors -Wall

########################
# Compile-time options #
########################

# Comment an option out with a # if you want to disable the option

# Enable bounds checking?
COPTS+=		-DBOUNDS_CHECKING

# Enable debug code?
COPTS+=		-DDEBUG
