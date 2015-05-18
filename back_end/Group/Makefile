## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/do/COPYRIGHT for the full notice.

## Contents: GNU Makefile for the Elt hierarchy
##
## Authors: Stephane Collart & Roger Needham
##
## Status: Useable.
##
## Revision History:
## 
## IX/94: StC: Stripped out all specific compilation commands for the
##   test files and standardised them in compile.mk (which is split out
##   of global.mk). Moved INCLUDE to compile.mk and standardised.
##   Automatised linking in of other Back End components. Added
##   mechanism for cumulating -D's in DEFINE and in DEBUG (see
##   compile.mk for details).
##
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


BACKENDCOMPONENTS = libg++ general Elt Map Matrix Polynomial FSA KB

		     
# FILES:
# FILES lists all source files (without .[Cc] suffixes) which are
# part of the Elt hierarchy:

FILES = FGGroup FGGroupRep FPGroup FPGroupRep FreeByCyclic FreeGroup \
	FreeGroupRep Group GroupRep Homology MSCGroup PresentationParser \
        RandomMSCGroup SmithNormalForm TietzeTrekker  \
        ShortenByRelators SymmetricRelators ORWordProblem\
	AbelianInfinitenessProblem EquationParser MSCGConjugacyProblem \
	GeneralWhitehead PrimeNumbers AbelianGroup AbelianGroupRep \
        AbelianSGPresentation GroupFastChecks TTP Products WordEnumerator \
	PowerSeriesWP RipsConstruction EqSystemParser AbelianEquations \
	SmithNormalForm1

# TESTS:
# TESTS lists all source files (without .[Cc] suffixes) in test:

TESTS = debug-MSCGroup test-SymmetricRelators debug-AbelianGroup \
	debug-AbelianInfinitenessProblem debug-EquationParser \
	debug-MSCGConjugacyProblem debug-RandomMSCGroup \
#	debug-RipsConstruction #debug-EqSystemParser


######################################################################
######################################################################
## Do NOT change this:                                               #
#                                                                    #
include ../global/global.mk                                          #
#                                                                    #
##                                                                   #
######################################################################
######################################################################
