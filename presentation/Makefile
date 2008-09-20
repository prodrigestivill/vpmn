.PHONY: pdf presentations %images clean distclean 

pdf: cleanpdf allimages presentations

presentations: allimages epsc_vpmn_slides.pdf #scg08_vpmn.pdf

#PDF Output files
%.pdf: %.tex
	pdflatex --draftmode $<
	pdflatex $<

#Clean
cmdcleanpdf = rm -vf *.pdf
cmdclean = rm -vf .*swp *.autosave *.pws *.bak *.aux *.def *.drv *.dvi *.glo *.idx *.log *.lot *.lof *.prv *.toc *~ *.out *.nav *.snm
cleanpdf:
	$(cmdcleanpdf)

clean:
	$(cmdclean)

distclean: clean cleanimages cleanpdf

#Images
%images:
	cd images && $(MAKE) $@
