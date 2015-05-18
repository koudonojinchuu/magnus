# Makefile for kbeqn2 and cfsa

# These programs are hacks of Derek Holt's code, using his auotmata library.
# In order to use these you will need Derek's library files located on
# your system and compiled.  On my system they reside in
# /users/curmi/lib

CC=gcc
SHELL = /bin/sh
BIN = ../../bin
# You will have to arrange these for your own system (the paths used are mine)
LIBPATH = /users/curmi/lib/lib.d
LIBS = $(LIBPATH)/automatalib.a
CFLAGS= -I/usr/include -I$(LIBPATH) -O -o

all: $(BIN)/kbeqn2 $(BIN)/cfsa

$(BIN)/kbeqn2:	kbeqn2.c
	${CC} $(CFLAGS) $(BIN)/kbeqn2 kbeqn2.c $(LIBS)
$(BIN)/cfsa:	cfsa.c
	${CC} $(CFLAGS) $(BIN)/cfsa cfsa.c $(LIBS)

#.c.o:
#	${CC} -c $(CFLAGS) $*.c 
