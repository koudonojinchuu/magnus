## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Makefile for the Magnus edition of Dion Gildenhuys'
##           one relator word problem program
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Revision History:
##


.PHONY: all clean distclean mostlyclean realclean

all: orwp

orwp : orwp.C
	g++ -DDEBUG -g -fno-operator-names -D_G_NO_NRV -Wno-deprecated -O2 -o orwp orwp.C
	strip orwp*

clean:
	-rm -f orwp

mostlyclean: 