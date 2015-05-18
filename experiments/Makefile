## Copyright (C) 1994 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: GNU Makefile for the experiments directory
##
## Principal Author: Roger Needham
##
## Status: Obscenely quick-n-dirty
##
## Revision History:
##


# INCLUDE lists the relative pathname of every directory where
# included header files are found:

INCLUDE = -Iinclude -I../back_end/global -I../back_end/general/include -I../back_end/Elt/include -I../back_end/Subgroup/include -I../back_end/Group/include -I../back_end/FSA/include -I../back_end/RKBP/include
SRCDIR = src
OBJDIR = obj

CC = g++
CFLAGS = -g


## The targets:

.PHONY: default
.PHONY: clean distclean cleandepend mostlyclean realclean
.PHONY: H.Neumann_conjecture H.Neumann_conjecture2 subgroupRelators

default:
	@echo select one of: H.Neumann_conjecture H.Neumann_conjecture2 subgroupRelators

clean: mostlyclean

distclean: clean

mostlyclean:
	-rm -f $(OBJDIR)/*

cleandepend:
	-rm -f $(OBJDIR)/*.d

realclean: distclean


H.Neumann_conjecture: obj/H.Neumann_conjecture.o
	cd ../back_end/Elt; $(MAKE) lib
	cd ../back_end/general; $(MAKE) lib
	cd ../back_end/Subgroup; $(MAKE) lib
	${CC} $(OBJDIR)/$@.o ../back_end/Subgroup/lib/libSubgroup.a ../back_end/Elt/lib/libElt.a ../back_end/general/lib/libgeneral.a -o bin/$@
	@echo
	@echo "The executable is ./bin/$@"
	@echo

H.Neumann_conjecture2: obj/H.Neumann_conjecture2.o
	cd ../back_end/Elt; $(MAKE) lib
	cd ../back_end/general; $(MAKE) lib
	cd ../back_end/Subgroup; $(MAKE) lib
	${CC} $(OBJDIR)/$@.o ../back_end/Subgroup/lib/libSubgroup.a ../back_end/Elt/lib/libElt.a ../back_end/general/lib/libgeneral.a -o bin/$@
	@echo
	@echo "The executable is ./bin/$@"
	@echo

subgroupRelators: obj/subgroupRelators.o
	cd ../back_end/Elt; $(MAKE) lib
	cd ../back_end/general; $(MAKE) lib
	cd ../back_end/Group; $(MAKE) lib
	cd ../back_end/FSA; $(MAKE) lib
	cd ../back_end/RKBP; $(MAKE) lib
	cd ../back_end/Subgroup; $(MAKE) lib
	${CC} $(OBJDIR)/$@.o ../back_end/Subgroup/lib/libSubgroup.a ../back_end/Elt/lib/libElt.a ../back_end/Group/lib/libGroup.a ../back_end/FSA/lib/libFSA.a ../back_end/RKBP/lib/libRKBP.a ../back_end/general/lib/libgeneral.a -o bin/$@
	@echo
	@echo "The executable is ./bin/$@"
	@echo


$(OBJDIR)/%.o: $(SRCDIR)/%.C $(OBJDIR)
	${CC} ${CFLAGS} $(INCLUDE) -c $< -o $@
