.PHONY: pdf presentations %images clean distclean 

pdf: cleanpdf cleanimages allimages presentations

presentations: allimages scg08_vpmn.pdf

#PDF Output files
%.pdf: %.tex
	pdflatex --draftmode $<
	pdflatex $<

#Clean
cmdcleanpdf = rm -vf *.pdf
cmdclean = rm -vf .*swp *.autosave *.pws *.bak *.aux *.def *.drv *.dvi *.glo *.idx *.log *.lot *.lof *.prv *.toc *~ *.out *.nav *.snm
cleanpdf:
	$(cmdcleanpdf)

clean: cleanimages
	$(cmdclean)

distclean: clean cleanpdf

#Images
%images:
	cd images && $(MAKE) $@
