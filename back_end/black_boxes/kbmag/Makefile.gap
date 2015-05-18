SHELL = /bin/sh
SYSTEM = SYSTEMV
RM = /bin/rm

CC = gcc
COPTS = -O

all: install_gap

# installation for GAP
install_gap:
	test -d bin/`hostname` || mkdir bin/`hostname`
	(cd lib; $(MAKE) CC="$(CC)" COPTS="$(COPTS)" \
	         LIB=../bin/`hostname`/fsalib.a )
	cd lib; $(MAKE) clean
	(cd src; $(MAKE) CC="$(CC)" SYSTEM="$(SYSTEM)" COPTS="$(COPTS)" \
                FSALIB=../bin/`hostname`/fsalib.a BIN=../bin/`hostname` )
	cd src; $(MAKE) clean

