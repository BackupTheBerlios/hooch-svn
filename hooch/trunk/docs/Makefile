# $Id: Makefile,v 1.6 2003/12/01 18:44:15 airhead Exp $
# Hooch documentation Makefile

LATEX = latex
PDFLATEX = pdflatex
PDFVIEW = xpdf
PSVIEW = gv
DVIVIEW = xdvi
RM = rm -f
DVIPS = dvips

###################

# PHONY lists all targets which are not files (see make man page, -t flag)
.PHONY: all show showps showpdf showdvi ps pdf dvi clean

all: pdf ps dvi

show: showdvi

showps: ps
	$(PSVIEW) func_spec.ps

showpdf: pdf
	$(PDFVIEW) func_spec.pdf

showdvi: dvi
	${DVIVIEW} func_spec.dvi

dvi: func_spec.dvi
ps: func_spec.ps

pdf: func_spec.pdf


# .SUFFIXES lists all file suffixes which can be automagically compiled
# according to the '.x.y:' style rules which follow

.SUFFIXES: .tex .pdf .dvi .ps

.tex.pdf:
	${PDFLATEX} $<
	${PDFLATEX} $<

.dvi.ps:
	${DVIPS} -o $@ $<

.tex.dvi:
	${LATEX} $<

clean:
	${RM} *.dvi *.ps *.pdf *.out *.toc *.aux *.log
