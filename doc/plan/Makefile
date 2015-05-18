##
##   $Id: Makefile 35 2005-05-03 21:48:32Z jzniu $
##

## Copyright (C) 1995 The New York Group Theory Cooperative
## See magnus/doc/COPYRIGHT for the full notice.

## Contents: GNU Makefile for the Magnus plan documentation
##
## Principal Author: Roger Needham
##
## Status: Useable.
##
## Revision History:
##


.PHONY: analysis design implementation plan
.PHONY: figs_dir analysis_figs design_figs implementation_figs
.PHONY: clean distclean mostlyclean realclean


all: plan


##
## Figure targets:
##
## As you add new figures to a document, make the corresponding
## <TeX file name>_figs target depend on the ps file here.

ps/%.ps: figs/%.fig
	-rm -f $@
	fig2dev -L ps -p 1 -c $< > $@

figs_dir:
	if [ ! -d ps ]; then mkdir ps; fi

analysis_figs: figs_dir \
        ps/WPview.ps \
        ps/NQview.ps

design_figs: figs_dir \
        ps/arch.ps \
        ps/sm1.ps \
        ps/sm2.ps \
        ps/sm3.ps \
        ps/gcm.ps \
        ps/subsys.ps \
        ps/wordprob1.ps \
        ps/wordprob2.ps

implementation_figs: figs_dir


##
## User targets:
##

analysis: analysis_figs
	-latex analysis.tex; latex analysis.tex

design: design_figs
	-latex design.tex; latex design.tex

implementation: implementation_figs
	-latex implementation.tex; latex implementation.tex

plan: analysis_figs design_figs implementation_figs
	-latex plan.tex; latex plan.tex


##
## Maintenance targets:
##

clean:
	-rm -f *~ *.log

distclean: clean
	-rm -f *.dvi *.toc *.aux *.ps

mostlyclean: distclean

realclean: mostlyclean
	-rm -f ps/*

.DEFAULT:
	@echo "Unknown target: $@"
	@echo "Choose one of: all analysis design implementation plan"
	@echo "               clean distclean mostlyclean realclean"
