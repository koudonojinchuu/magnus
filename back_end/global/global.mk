## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: First level GNU make include file for every hierarchy in
##           the back end
##
## Authors: Stephane Collart & Roger Needham
##
## Status: Usable
##         This needs the -M option of gcc, cc, etc. to work!
##
## Revision History:
##
## IX/94: StC: Stripped this makefile down to the commands necessary for
##   dealing with clean and the such. Hived all other commands involving
##   compilation and reading in dependencies to a seperate makefile
##   source compile.mk. Provided facilities for cumulating and
##   overriding -D flags in DEFINE and DEBUG, and automatic DEBUG_All
##   flag expansion (see compile.mk for details).

# Nothing should need to be changed in this file.

# This makefile is the first part of the common makefile source of all
# the back-end subdirectories. It is equipped only to deal with
# cleaning-like goals. Any other targets, such as targets involving
# compilation, will cause it to recursively call make with the makefile
# global/compile.mk. This is to guard against the GNU makefile `feature'
# which automatically remakes all makefile sources, even when cleaning.
# It is to be included by the GNU Makefile in each of the class
# hierarchy directories (such as Elt) of the back end. Let H be the
# current class hierarchy.

# This file assumes that the following variables are defined:
# 
# FILES    lists all source files, without .[Cc] suffix,
#          which are part of H and thus are to be compiled
#          and put into the library for H.
# 
# TESTS    lists all source files, without .[Cc] suffix,
#          in the test directory. These are not part of H,
#          and are not put into the library for H.
# 
# BACKENDCOMPONENTS
#          lists all the components of the Back End needed by
#          the executables in H.
#

# Aside from these, this file hard-wires the relative pathnames of the
# canonical directories of a hierarchy directory but delegates any
# compiling, including updating the library for H, which involves
# automatically producing object and dependency files,
# through a recursive make call to the makefile compile.mk provides for

# This exports all its variables to sub-makes.

# Name of current directory:
DIR = $(shell basename `pwd`)

# Relative path of directory in which to put .C's
SRCDIR = src

# Relative path of directory in which to put .o's and .d's:
OBJDIR = $(SRCDIR)/obj

# Relative pathname where to put this library
THISLIBDIR = lib

# Relative path of directory in which to put all the sources and files
# related to the tests
TESTDIR = test
 
# Relative path of directory in which to put the executables of the test files
TESTEXEDIR = $(TESTDIR)/bin
 
# Relative path of the Back End root directory
BACKENDROOT = ..

# Relative path of the global sub-dir of Back End root dir
GLOBAL = $(BACKENDROOT)/global

# Name of the library of H is H
THISLIB = $(DIR)
 
# Names of the Back End libs
BACKENDLIBS = $(foreach file,$(BACKENDCOMPONENTS),lib$(file))
 
## The targets:

.PHONY: all lib tests default $(TESTS)
.PHONY: clean distclean cleandepend cleanlib mostlyclean realclean
.PHONY: $(THISLIB) lib$(THISLIB) $(BACKENDCOMPONENTS) $(BACKENDLIBS)

MAINTARGETS = all lib tests mostlyclean cleandepend cleanlib clean \
	realclean $(TESTS)
 
# this is the default target
# whenever changing the order of anything, including reading in of
# makefile sources via include, make sure this is the first target to
# be read
default:
	@echo select one of: $(MAINTARGETS)

# the targets to be actually made here:

clean: mostlyclean
	-rm $(TESTEXEDIR)/*

mostlyclean:
	-rm $(THISLIBDIR)/lib$(THISLIB).a
	-rm $(OBJDIR)/*

cleandepend:
	-rm $(OBJDIR)/*.d

cleanlib:
	-rm $(THISLIBDIR)/lib$(THISLIB).a

# the targets to be recursively made:

export

# makes all the phony Blabla targets depend on phony targets libBlabla
$(BACKENDCOMPONENTS): % : lib%
 
# this forces targets of the form libBlabla to be made in their own
# Back End component directory, since a libBlabla target's actual
# dependencies are only known there
$(BACKENDLIBS): lib%:
	cd $(BACKENDROOT)/$*; $(MAKE) -f Makefile -f $(GLOBAL)/compile.mk lib

# make sure the test targets get made checking all their dependencies by
# forcing their remake: (if they were mentionned as a target before in
# Makefile this would fool make into thinking there is nothing to be made)

# to make any or all tests, first make all libs, then make tests
tests $(TESTS): $(BACKENDLIBS)
	$(MAKE) -f $(GLOBAL)/compile.mk $@

# Anything not makable here gets delegated:
.DEFAULT:
	$(MAKE) -f $(GLOBAL)/compile.mk $@
