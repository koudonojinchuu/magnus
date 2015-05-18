# Makefile for all homology programs
# Jamie P. Curmi (3/11/94)
#
# NOTE:  You may need to modify lines following a line with #**************
# before running make.  Either comment out (using #'s) the items you don't
# want, or remove #'s for items you do want.  The makefile is initially
# set up for DECstations, using the command line.

#****************
# Remove the '#' from the line below if you want to compile these programs for
# machines/OS's that don't make use of a command line.  Alternatively you
# can remove the commented line indicated in each of the source file.
#NO_CMD_LINE = -DNO_CMD_LINE

#****************
# Use this on Decs and some other machines.  It compiles to ucode object files
# for full optimisation on RISC compiler.
#LIBS = -klhom
#STAT_LIBS = -klhom_s
#CFLAGS= -I/usr/include $(NO_CMD_LINE) -I$(LIBPATH) -kL$(LIBPATH) -O3 -o

#****************
# Use this on Other machines.  It compiles to the usual object files
# with full optimisation.  Your compiler may not allow optimisation
# flag -O3.  You may have to modify this to -O2 or -O.
#
# You may also need to modify CC to represent your compiler.
CC=gcc
LIBS = -lhom
STAT_LIBS = -lhom_s
CFLAGS= -I/usr/include $(NO_CMD_LINE) -I$(LIBPATH) -L$(LIBPATH) -O3 -o


SHELL = /bin/sh
BIN = ../../bin
LIBPATH = ../../lib

all: $(BIN)/cbound $(BIN)/cboundn $(BIN)/chom $(BIN)/chom_stats \
     $(BIN)/cmat $(BIN)/rank $(BIN)/tuples $(BIN)/reduce $(BIN)/calchom 
#clean
$(BIN)/cbound:	cbound.c
	${CC} $(CFLAGS) $(BIN)/cbound cbound.c $(LIBS)
$(BIN)/cboundn:	cboundn.c
	${CC} $(CFLAGS) $(BIN)/cboundn cboundn.c $(LIBS)
$(BIN)/chom:	chom.c
	${CC} $(CFLAGS) $(BIN)/chom chom.c $(LIBS)
$(BIN)/chom_stats:	chom_stats.c  
	${CC} $(CFLAGS) $(BIN)/chom_stats chom_stats.c $(STAT_LIBS)
$(BIN)/cmat:	cmat.c  
	${CC} $(CFLAGS) $(BIN)/cmat cmat.c $(LIBS)
$(BIN)/rank:	rank.c  
	${CC} $(CFLAGS) $(BIN)/rank rank.c $(LIBS)
$(BIN)/tuples:	tuples.c  
	${CC} $(CFLAGS) $(BIN)/tuples tuples.c $(LIBS)
$(BIN)/reduce:	reduce.c  
	${CC} $(CFLAGS) $(BIN)/reduce reduce.c $(LIBS)
$(BIN)/calchom:
	cp calchom $(BIN)/calchom
	chmod u+x $(BIN)/calchom

clean:
	rm -f *.u *.o
