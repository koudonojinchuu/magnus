# General makefile for KBMAG - all executables are put into the bin directory.
SHELL = /bin/sh
SYSTEM = SYSTEMV
RM = /bin/rm

CC = gcc
COPTS = -O

all:
	if [ ! -d ./bin ]; then mkdir ./bin; fi
	cd lib; $(MAKE) CC="$(CC)" COPTS="$(COPTS)"
	cd src; $(MAKE) CC="$(CC)" SYSTEM="$(SYSTEM)" COPTS="$(COPTS)"
	strip bin/f* bin/g* bin/k* bin/p* bin/w*

clean: mostlyclean
	-rm -f bin/*

mostlyclean:
	cd lib; $(MAKE) clean
	cd src; $(MAKE) clean
	cd ag_data; $(RM) -f *.* 
	cd kb_data; $(RM) -f *.* 
	cd fsa_data; $(RM) -f *.* 
	cd doc; $(RM) -f *.aux *.toc *.log *.dvi
	cd gapdoc; $(RM) -f *.aux *.toc *.log *.dvi

test:
	cd kb_data; ../bin/kbprog nilp2; rm nilp2.*
	cd ag_data; ../bin/autgroup 235; ../bin/fsacount 235.wa; rm 235.*

# installation for GAP
GAP_PATH = ../..
PKG_PATH = $(GAP_PATH)/pkg
SRCDISTFILE = kbmag

# for GAP distribution
src_dist: 
	( cd $(PKG_PATH)/kbmag/gap && ./makeinit )
	( cd $(PKG_PATH)/kbmag/ ; $(RM) -f init.g ; ln -s gap/init.g init.g )
	( cd $(PKG_PATH)/kbmag/lib && make clean )
	( cd $(PKG_PATH)/kbmag/src && make clean )
	( cd $(PKG_PATH)/kbmag/ag_data && $(RM) -f *.* )
	( cd $(PKG_PATH)/kbmag/kb_data && $(RM) -f *.* )
	( cd $(PKG_PATH)/kbmag/fsa_data && $(RM) -f *.* )
	( cd $(PKG_PATH)/kbmag/doc && $(RM) -f *.aux *.log *.toc *.dvi )
	( cd $(PKG_PATH)/kbmag/gapdoc && $(RM) -f *.aux *.log *.toc *.dvi )
	@touch $(PKG_PATH)/kbmag/bin/.keepme
	@zoo ah $(SRCDISTFILE).zoo $(PKG_PATH)/kbmag/bin/.keepme \
	    `find ${PKG_PATH}/kbmag -print -name bin -prune`
	@zoo PE $(SRCDISTFILE).zoo
