## Copyright (C) 1996 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Makefile for the magnus back end.
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Revision History:
##

## The following may be defined by a higher-level make:

ifeq (0,$(MAKELEVEL))
  CC = g++
  DEBUG = -DDEBUG -g
  DEFINE = -DHAVE_UNISTD_H $(INCLUDE) -fno-operator-names -D_G_NO_NRV -Wno-deprecated
  CFLAGS = $(DEBUG) $(DEFINE)
endif

export CC
export DEBUG
export DEFINE
export CFLAGS


SRC_DIRS = Elt Group general


.PHONY: all lib clean distclean mostlyclean realclean cleandepend

all: 
	@if [ ! -f global/config.h ]; \
         then echo; echo "You must run ./Configure first"; echo; exit 1; fi

	cd libg++; $(MAKE) all
	cd Elt; $(MAKE) all
	cd Equations; $(MAKE) all
	cd AProducts; $(MAKE) all
	cd FSA; $(MAKE) all
	cd Todd-Coxeter; $(MAKE) all
	cd Group; $(MAKE) all
	cd GAP; $(MAKE) all
	cd Apps; $(MAKE) all
	cd Map; $(MAKE) all
	cd KB; $(MAKE) all
	cd Subgroup; $(MAKE) all
	cd Genetic; $(MAKE) all
	cd NilpotentGroup; $(MAKE) all
	cd Polynomial; $(MAKE) all
	cd Matrix; $(MAKE) all
	cd general; $(MAKE) all
	cd black_boxes; $(MAKE) all
	cd Enumerators; $(MAKE) all
	cd Packages; $(MAKE) all
	cd SMApps; $(MAKE) all
	cd SessionManager; $(MAKE) all
	cd SessionManager; $(MAKE) magnus

lib:
	cd libg++; $(MAKE) lib
	cd Elt; $(MAKE) lib
	cd Equations; $(MAKE) lib
	cd AProducts; $(MAKE) lib
	cd FSA; $(MAKE) lib
	cd Todd-Coxeter; $(MAKE) lib
	cd Packages; $(MAKE) lib
	cd Enumerators; $(MAKE) lib
	cd Group; $(MAKE) lib
	cd GAP; $(MAKE) lib
	cd Apps; $(MAKE) lib  
	cd Map; $(MAKE) lib
	cd KB; $(MAKE) lib
	cd Subgroup; $(MAKE) lib
	cd Genetic; $(MAKE) lib
	cd NilpotentGroup; $(MAKE) lib
	cd Polynomial; $(MAKE) lib
	cd Matrix; $(MAKE) lib
	cd general; $(MAKE) lib
	cd black_boxes; $(MAKE) lib
	cd SMApps; $(MAKE) lib
	cd SessionManager; $(MAKE) lib
#	$(AR) cr lib/lib_back_end.a \
#                 Elt/lib/libElt.a \
#                 Group/lib/libGroup.a \
#                 general/lib/libgeneral.a
#	ranlib lib/lib_back_end.a
#	-cd lib; $(foreach file,$(wildcard ../global/*.h),ln $(file);)
#	-cd lib; $(foreach dir,$(SRC_DIRS),$(foreach file,$(wildcard ../$(dir)/*.h),ln $(file);))

clean:
	cd libg++; $(MAKE) clean
	cd Elt; $(MAKE) clean
	cd Equations; $(MAKE) clean
	cd AProducts; $(MAKE) clean
	cd FSA; $(MAKE) clean
	cd Enumerators; $(MAKE) clean
	cd Todd-Coxeter; $(MAKE) clean
	cd Packages; $(MAKE) clean
	cd Group; $(MAKE) clean
	cd GAP; $(MAKE) clean
	cd Apps; $(MAKE) clean
	cd Map; $(MAKE) clean
	cd KB; $(MAKE) clean
	cd Subgroup; $(MAKE) clean
	cd Genetic; $(MAKE) clean
	cd NilpotentGroup; $(MAKE) clean
	cd Polynomial; $(MAKE) clean
	cd Matrix; $(MAKE) clean
	cd general; $(MAKE) clean
	cd black_boxes; $(MAKE) clean
	cd SMApps; $(MAKE) clean
	cd SessionManager; $(MAKE) clean

distclean:
	-rm -rf libg++
	-rm -rf Elt
	-rm -rf Equations
	-rm -rf AProducts
	-rm -rf FSA
	-rm -rf Todd-Coxeter
	-rm -rf Packages
	-rm -rf Enumerators
	-rm -rf Group
	-rm -rf GAP
	-rm -rf Apps
	-rm -rf Map
	-rm -rf KB
	-rm -rf Subgroup
	-rm -rf Genetic
	-rm -rf NilpotentGroup
	-rm -rf Polynomial
	-rm -rf Matrix
	-rm -rf general
	-rm -rf SMApps
	cd black_boxes; $(MAKE) distclean
	-rm -rf black_boxes/CVS black_boxes/README black_boxes/Makefile
	-rm -fr SessionManager/src
	-rm -fr SessionManager/CVS
	-rm -fr SessionManager/include
	-rm -fr SessionManager/lib
	-rm -f  SessionManager/test/*
	-rm -fr SessionManager/test/CVS
	-rm -fr SessionManager/test/bin/CVS
	-rm -fr SessionManager/Makefile
	-rm -fr global
	-rm -fr lib

mostlyclean:
	cd libg++; $(MAKE) mostlyclean
	cd Elt; $(MAKE) mostlyclean
	cd Equations; $(MAKE) mostlyclean
	cd AProducts; $(MAKE) mostlyclean
	cd FSA; $(MAKE) mostlyclean
	cd Group; $(MAKE) mostlyclean
	cd GAP; $(MAKE) mostlyclean
	cd Apps; $(MAKE) mostlyclean
	cd Todd-Coxeter; $(MAKE) mostlyclean
	cd Packages; $(MAKE) mostlyclean
	cd Enumerators; $(MAKE) mostlyclean
	cd Map; $(MAKE) mostlyclean
	cd KB; $(MAKE) mostlyclean
	cd Subgroup; $(MAKE) mostlyclean
	cd Genetic; $(MAKE) mostlyclean
	cd NilpotentGroup; $(MAKE) mostlyclean
	cd Polynomial; $(MAKE) mostlyclean
	cd Matrix; $(MAKE) mostlyclean
	cd general; $(MAKE) mostlyclean
	cd black_boxes; $(MAKE) mostlyclean
	cd SMApps; $(MAKE) mostlyclean
	cd SessionManager; $(MAKE) mostlyclean

realclean: distclean


cleandepend:
	cd libg++; $(MAKE) cleandepend
	cd Elt; $(MAKE) cleandepend
	cd Equations; $(MAKE) cleandepend
	cd AProducts; $(MAKE) cleandepend
	cd FSA; $(MAKE) cleandepend
	cd Todd-Coxeter; $(MAKE) cleandepend
	cd Packages; $(MAKE) cleandepend
	cd Enumerators; $(MAKE) cleandepend
	cd Group; $(MAKE) cleandepend
	cd GAP; $(MAKE) cleandepend
	cd Apps; $(MAKE) cleandepend
	cd Map; $(MAKE) cleandepend
	cd KB; $(MAKE) cleandepend
	cd Subgroup; $(MAKE) cleandepend
	cd Genetic; $(MAKE) cleandepend
	cd NilpotentGroup; $(MAKE) cleandepend
	cd Polynomial; $(MAKE) cleandepend
	cd Matrix; $(MAKE) cleandepend
	cd general; $(MAKE) cleandepend
	cd SMApps; $(MAKE) cleandepend
	cd SessionManager; $(MAKE) cleandepend
