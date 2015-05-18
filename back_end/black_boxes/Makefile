## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Makefile for the magnus black boxes.
##           Relies on the makefiles in each black box subdirectory.
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Revision History:
##


.PHONY: all lib clean distclean mostlyclean realclean

all: 
	cd TietzeTrek; $(MAKE) all
	cd kbmag; $(MAKE) all
	cd rkbp; $(MAKE) all
	cd homology; $(MAKE) all
	cd orwp; $(MAKE) all
	cd tc5; $(MAKE) tc
	cd ace; $(MAKE) -f make.darter

lib:

clean:
	cd TietzeTrek; $(MAKE) clean
	cd kbmag; $(MAKE) clean
	cd rkbp; $(MAKE) clean
	cd homology; $(MAKE) clean
	cd orwp; $(MAKE) clean
	cd tc5; $(MAKE) clean
	cd ace; rm -f ace

distclean:
	-rm -rf TietzeTrek
	-cd kbmag; rm -rf CVS Makefile* README *data *doc gap lib src init.g bin/CVS
	-cd rkbp; rm -rf Makefile CVS README* documentation examples source bin/CVS
	-cd homology; rm -rf CVS Makefile README lib man src testfiles bin/CVS
	-cd orwp; rm fr Makefile *.C CVS

mostlyclean:
	cd TietzeTrek; $(MAKE) mostlyclean
	cd kbmag; $(MAKE) mostlyclean
	cd rkbp; $(MAKE) mostlyclean
	cd homology; $(MAKE) mostlyclean
	cd orwp; $(MAKE) mostlyclean
	cd tc5; $(MAKE) mostlyclean
	cd ace; rm ace

realclean: distclean
