## Copyright (C) 1996 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Makefile for the magnus project.
##
## Principal Author: Roger Needham
##
## Status:  Useable.
##          @rn Doesn't clean magnus/doc/manual/ yet.
##
## Revision History:
##

## Things you may need to change for your system:

##########################################################
# 20020430000 tpd add -fno-operator-names
#  which means do not treat the operator name keywords 'and', 'bitand',
#  'bitor', 'compl', 'not', 'or' and 'xor' as synonyms as keywords
# 20020430001 tpd add -D_G_NO_NRV
#  named return values are deprecated
# 20050505 tpd re-enable the cflags for gcc-3.4 and above
export DEBUG = #-DDEBUG -g
##########################################################

#@njz
#put an absolute path here or properly set PATH
export CC = g++
#

#@njz
#export DEFINE = -DHAVE_UNISTD_H -fno-operator-names -D_G_NO_NRV
export DEFINE = -fno-operator-names -D_G_NO_NRV -Wno-deprecated
#
export CFLAGS = $(DEBUG) $(DEFINE)


## You shouldn't need to change the following:

.PHONY: all clean distclean mostlyclean realclean junkclean auxclean

all:
	@if [ ! -f back_end/global/config.h ]; \
         then echo; echo "You must run ./Configure first"; echo; exit 1; fi
	cd back_end; $(MAKE) all
	@echo
	@echo "The executable is ./front_end/magnus"
	@echo

## Deletes all executable, obj and library files
clean: junkclean
	cd back_end; $(MAKE) clean

## Deletes all files except executables
distclean: auxclean
	cd back_end; $(MAKE) distclean
	-rm -rf back_end/CVS back_end/Makefile back_end/canonical
	mv -f aConfigure Configure
	-rm -rf bin experiments misc CVS Makefile 

## Deletes only library and obj files
mostlyclean: junkclean
	cd back_end; $(MAKE) mostlyclean

realclean: distclean

junkclean:
	-find . \( -name "*[%~]"  -a -exec rm {} \; \) -o \
                \( -name "*.BAK"  -a -exec rm {} \; \) -o \
                \( -name "*.bak"  -a -exec rm {} \; \)

auxclean: junkclean
	-find . \( -name "*.aux"  -a -exec rm {} \; \) -o \
                \( -name "*.toc"  -a -exec rm {} \; \) -o \
                \( -name "*.dvi"  -a -exec rm {} \; \) -o \
                \( -name "#*#"    -a -exec rm {} \; \) -o \
                \( -name "*.ps"   -a -exec rm {} \; \)
