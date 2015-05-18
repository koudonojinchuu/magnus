## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: GNU Makefile for the Matrix and RingElt hierarchies
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Revision History:
##

##
## How you will need to change this file:
##
## - Add to FILES, and TESTS as you add new files.
##
## - State in BACKENDCOMPONENTS which other components of the Back End are
##   needed for this one, ie. need to be linked into the executables;
##   $(BACKENDROOT)/global/compile.mk will take care of linking them in.
##
## - $(BACKENDROOT)/global/global.mk will take care of remaking the
##   corresponding libSomeComponent if necessary.
##
## - If this component of the Back End needs any particular -D flags
##   to the compiler, add them in DEFINE below.
##
## - Normally, you should not need to set DEBUG and SAFETY here.
##
## All dependencies via #include directives are taken care of automatically.
##


######################################################################
#                                                                    #
# Define the variables describing the contents of this component of  #
# the Back End and the flags specific to it                          #
#                                                                    #
######################################################################

# DEFINE:
# If you need any extra DEFINES, put them here, taking care not to wipe
# out any passed from the command line or a higher level make

override DEFINE +=

# DEBUG:
# If you set DEBUG here, this will wipe out the defaults for the Back
# End; to add individual DEBUG flags when compiling, set DEBUG on
# the command line

# DEBUG =

# SAFETY:
# If you want to override the default for SAFETY, you can either set it
# here or on the command line

# SAFETY =

# BACKENDCOMPONENTS:
# BACKENDCOMPONENTS is all the components of the Back End needed by
# this component and its TESTS (other than the current component)

BACKENDCOMPONENTS = libg++ general Elt Group Map

# FILES:
# FILES lists all source files (without .[Cc] suffixes) which are
# part of the Elt hierarchy:

FILES = Matrix GaussTransformation MatrixComputations \
       RandomMatrix
#      HomomorphismBuilder Polynomial

# TESTS:
# TESTS lists all source files (without .[Cc] suffixes) in test:


TESTS = debug-Matrix proba
#        debug-GaussTransformation debug-RationalMatrix \
#	debug-RationalGT debug-RandomMatrix \
#	debug-HomomorphismBuilder debug-BTree debug-Polynomial \


######################################################################
######################################################################
## Do NOT change this:                                               #
#                                                                    #
include ../global/global.mk                                          #
#                                                                    #
##                                                                   #
######################################################################
######################################################################
