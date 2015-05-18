## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: Second level GNU make include file for every hierarchy
##           in the back end, which takes care of actual compilation,
##           automatic updating of dependecies, etc.
##
## Authors: Stephane Collart & Roger Needham
##
## Status: Usable
##         This needs the -MM option of gcc, cc, etc. to work!
##
## Things To Do:
##
## * Need to explain what flags can be passed to the make command in
##   some central readable place. Otherwise older makefiles never get
##   updated on all the newest switches etc. This includes all the
##   DEBUG flag variations, the LDFLAGS, etc.
##
## Revision History:
##
## IX/94: StC: Split this off from global.mk, to avoid remaking of
##   dependencies just before cleaning. Made object files depend on
##   makefile sources. Made test-file targets depend on the actual
##   executable file, to avoid remaking when up to date.
##
## 13.12.94: StC: added $(LDFLAGS) to the load command line
##
## 4/6/95 rn made LDFLAGS = -lm
##
##

# Nothing should need to be changed in this file, except for the
# following list of all back end components from time to time;
# The important thing about this list is that dependent components come
# before components depended on; the list is used for sorting the -l
# flags in the right order:


ALLBACKENDCOMPONENTS = SessionManager SMApps Packages Enumerators Apps GAP AProducts NilpotentGroup Todd-Coxeter Genetic Map Subgroup Equations Group Polynomial Matrix KB FSA Elt general libg++


#
# This is used as a makefile by recursive calls of make from global.mk
# to make anything which involves actual compiling, such as
# updating the library for H, which involves automatically producing
# object and dependency files.
#
# This assumes it imports the environment from global.mk, which
# includes:
# FILES    lists all source files, without .[Cc] suffix,
#          which are part of H and thus are to be compiled
#          and put into the library for H.
# TESTS    lists all source files, without .[Cc] suffix,
#          in the test directory. These are not part of H,
#          and are not put into the library for H.
# BACKENDCOMPONENTS
#          lists all the components of the Back End needed by
#          the executables in H.
# DIR      Name of the current directory
# SRCDIR   .C dir
# OBJDIR   .o dir
# THISLIBDIR
# TESTDIR  test sources
# TESTEXEDIR test executables
# BACKENDROOT root of back end
# GLOBAL   makefile sources
# THISLIB  name of current component, ie. H
# BACKENDLIBS libs from BACKENDCOMPONENTS (not used here)
# 

#
# This provides features to cumulate and override -D flags in DEFINE
# and DEBUG, as well as setting or overriding SAFETY
# A flag of the form DEBUG_All is recognised and expanded to a list
# of flags DEBUG_Class for all classes in the component
# see explanations below for details
#

#
# Additional features:
# * The .o files are made to depend on the Makefile sources, so that if the
#   Makefiles are changed, everything is automatically updated
# * In order to make H/lib/libH.a, you can `make libH' or `make H' as an
#   abbreviation
#

######################################################################
#                                                                    #
# The make, include, and lib files and flags necessary for           #
# compilation of components of the Back End                          #
#                                                                    #
######################################################################

# Names of the makefile sources (to make objects depend on makefiles)
MAKESOURCES = $(GLOBAL)/global.mk $(GLOBAL)/compile.mk Makefile

#
SORTEDOTHERLIBS = $(foreach file,$(ALLBACKENDCOMPONENTS),$(findstring \
	$(file),$(BACKENDCOMPONENTS)))

#
SORTEDLIBS = $(foreach file,$(ALLBACKENDCOMPONENTS),$(findstring \
	$(file),$(BACKENDCOMPONENTS) $(THISLIB)))

#
INCLUDE = -I$(BACKENDROOT)/global -Iinclude \
	$(foreach file,$(SORTEDOTHERLIBS),-I$(BACKENDROOT)/$(file)/include)

#
THISLIBFILE = $(THISLIBDIR)/lib$(THISLIB).a

#
LIBDIRS = $(foreach file,$(SORTEDLIBS),$(BACKENDROOT)/$(file)/lib)

#
OTHERLIBFILES = $(foreach \
	file,$(SORTEDOTHERLIBS),$(BACKENDROOT)/$(file)/lib/lib$(file).a)

# -L Names of the lib directories where the Back End libs are found
LIBPATH = $(foreach file,$(LIBDIRS),-L$(file))

# -l Names of the the various Back End libraries
LIBS = $(foreach file,$(SORTEDLIBS),-l$(file))

LDFLAGS = -lm

######################################################################
#                                                                    #
# Where to find the actual source and object files                   #
#                                                                    #
######################################################################

# The file names of the executables from TEST
TESTEXES = $(foreach file,$(TESTS),$(TESTEXEDIR)/$(file))

# List of object files of H:
OBJFILES = $(foreach file,$(FILES),$(OBJDIR)/$(file).o)

# Canonical relative paths for source files:
vpath %.C $(SRCDIR) $(TESTDIR)
vpath %.o $(OBJDIR)
vpath %.a $(LIBDIRS)

######################################################################
#                                                                    #
# Some default values for compilation flags and options for the Back #
# End                                                                #
#                                                                    #
######################################################################

BACKENDDEFINES = -DHAVE_UNISTD_H

DEFAULTDEBUGS =  -DDEBUG

DEFAULTSAFETY = -DSAFETY=2

######################################################################
#                                                                    #
# Set up the values for the compilation flags and options            #
#                                                                    #
######################################################################

## The philosophy for the variables, which may be passed in the
## environment from another make, either in the back-end or higher
## up, or may be on a command line:

# CC:
# If CC still has its default value, set it:
ifeq (default,$(origin CC))
	CC = g++
endif

# DEFINE:
# $(BACKENDDEFINES) is always added to the defines for something compiled
# in the back end, even if DEFINE is set in the command line

COMMONDEFINES := $(filter $(BACKENDDEFINES),$(DEFINE))
override DEFINE += $(filter-out $(COMMONDEFINES),$(BACKENDDEFINES))

# DEBUG:
# values for DEBUG set on the command line are treated as extra flags
# to the default values.
# NOTE that on the contrary values for DEBUG set in a higher level make
# take precedence, ie. they are left untouched. This makes it possible to
# compile for instance all of magnus from $magnusroot with uniform debug
# flags by setting DEBUG in the Makefile there

ifndef DEBUG
DEBUG = $(DEFAULTDEBUGS)
else
ifeq (command line,$(origin DEBUG))
COMMONDEBUGS := $(filter $(DEFAULTDEBUGS),$(DEBUG))
override DEBUG += $(filter-out $(COMMONDEBUGS),$(DEFAULTDEBUGS))
endif
endif

# The flag -DDEBUG_All in $(DEBUG) causes -DDEBUG_Class to be appended
# for all Classes in this component of the Back End in every case
ifeq (-DDEBUG_All,$(findstring -DDEBUG_All,$(DEBUG)))
override DEBUG += $(patsubst %,-DDEBUG_%,$(FILES))
endif

# SAFETY:
# If safety has been defined already in a higher level make or on the
# command line, that takes precedence; otherwise, set it to its default

ifndef SAFETY
SAFETY = $(DEFAULTSAFETY)
endif

# CFLAGS
# As a last resort, you can override CFLAGS in the make command line;
# but beware to really set the values you want and need.

CFLAGS = $(strip $(DEFINE) $(DEBUG) $(SAFETY))

######################################################################
#                                                                    #
# The targets and how to make them                                   #
#                                                                    #
######################################################################

.PHONY: all lib tests $(TESTS)
.PHONY: $(THISLIB) lib$(THISLIB) $(OTHERLIBS)

all: lib

tests: $(TESTS)

lib lib$(THISLIB): $(THISLIBDIR)/lib$(THISLIB).a

$(OBJDIR) $(TESTEXEDIR):
	mkdir $@

$(THISLIBDIR)/lib$(THISLIB).a : $(OBJFILES)
	if [ ! -d $(THISLIBDIR) ]; then mkdir $(THISLIBDIR); fi
	$(AR) cr $@ $(OBJFILES)
	ranlib $@

# make the test targets depend on the corresponding executable
$(TESTS): % : $(TESTEXEDIR)/%

# make each of the test executables depend on its .o and on
# the current lib THISLIB
$(TESTEXES): $(TESTEXEDIR)/% : $(OBJDIR)/%.o $(TESTEXEDIR) $(THISLIBFILE) \
		$(OTHERLIBFILES)
	$(CC) $(DEBUG) $< $(LIBPATH) $(LIBS) $(LDFLAGS) -o $@
	@echo
	@echo "The executable is ./$@"
	@echo

#@rn $(OBJDIR)/%.o: %.C $(OBJDIR) $(MAKESOURCES)
$(OBJDIR)/%.o: %.C
	${CC} ${CFLAGS} $(INCLUDE) -c $< -o $@

# How to make the dependency files:
# The pipe is put in its own shell to catch error conditions of the compiler
#@rn $(OBJDIR)/%.d: %.C $(OBJDIR) $(MAKESOURCES)
$(OBJDIR)/%.d: %.C
	if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
	$(SHELL) -ec '$(CC) -MM $(CFLAGS) $(INCLUDE) $< | sed "s:$*\.o:$(OBJDIR)/& $@:g"' > $@

# Include all the dependency files:
# Note that this causes Gmake to automatically remake them if they are
# out of date, and then reread them
include $(foreach file,$(FILES) $(TESTS),$(OBJDIR)/$(file).d)
