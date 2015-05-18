
.PHONY: all clean distclean mostlyclean realclean
.PHONY: about-magnus guidelines magnus manual figs

TEX = latex

all: magnus

figs:
	-rm -f ps/booch.ps ps/ObjectOf.ps ps/objects.ps ps/sm.ps ps/sm2.ps
	fig2dev -L ps -p 1 -c figs/booch.fig > ps/booch.ps
	fig2dev -L ps -p 1 -c figs/ObjectOf.fig > ps/ObjectOf.ps
	fig2dev -L ps -p 1 -c figs/objects.fig > ps/objects.ps
	fig2dev -L ps -p 1 -c figs/CMs.fig > ps/CMs.ps
#	fig2dev -L ps -p 1 -c figs/SM1.fig > ps/SM1.ps
#	fig2dev -L ps -p 1 -c figs/SM2.fig > ps/SM2.ps
#	fig2dev -L ps -p 1 -c figs/SM3.fig > ps/SM3.ps
#	fig2dev -L ps -p 1 -c figs/SM4.fig > ps/SM4.ps
#	fig2dev -L ps -p 1 -c figs/SM5.fig > ps/SM5.ps


about-magnus:
	-$(TEX) about-magnus.tex; $(TEX) about-magnus.tex

guidelines: figs
	-$(TEX) guidelines.tex; $(TEX) guidelines.tex

magnus: figs
	-$(TEX) magnus.tex; $(TEX) magnus.tex

manual: figs
	-$(TEX) manual.tex; $(TEX) manual.tex

clean:
	-rm -f *~ *.log

distclean: clean
	-rm -f *.dvi *.toc *.aux *.ps

mostlyclean: distclean

realclean: mostlyclean
	-rm -f ps/*

.DEFAULT:
	@echo "Unknown target: $@"
	@echo "Choose one of: all clean distclean mostlyclean realclean"
	@echo "               about-magnus guidelines magnus manual figs"
