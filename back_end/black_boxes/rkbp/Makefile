## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Makefile for the Magnus edition of
##           the Rutgers Knuth-Bendix Package
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Revision History:
##


.PHONY: all clean distclean mostlyclean realclean

all : bin/rkbp
VPATH = ./source
c_files := $(wildcard ./source/*.C)
h_files := $(wildcard ./source/*.h)

# 20050505 tpd add cflags
bin/rkbp : $(h_files) $(c_files)
	if [ ! -d ./bin ]; then mkdir ./bin; fi
	cd source; g++ -DDEBUG -g -fno-operator-names -D_G_NO_NRV -Wno-deprecated -O -o ../bin/rkbp *.C
	strip bin/rkbp*
clean:
	-rm -f bin/rkbp*


mostlyclean:

