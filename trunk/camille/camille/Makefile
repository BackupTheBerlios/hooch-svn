#
# $Id$
#
# Camille library Makefile
#

SCSH=scsh
SCSH_FLAGS=-lel exceptions/load.scm
LOAD_FLAGS=-lm camille.scm
OPEN_FLAGS=-o addressbook -o parser

.PHONY: test

test:
	${SCSH} ${SCSH_FLAGS} ${LOAD_FLAGS} ${OPEN_FLAGS} -lm test.scm -o test
