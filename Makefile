.PHONY: all pdf paper report epsc %extra %images clean distclean cleanupload upload 

all: pdf
pdf: cleanpdf cleanimages allextra allimages epsc

#Default Output PDFs
paper = paper
report = report
epsc = epsc_vpmn
annex = _annex
withannex = _all

paper: $(paper).pdf

report: $(report).pdf $(report)$(annex).pdf

epsc: $(epsc).pdf $(epsc)$(annex).pdf # $(epsc)$(withannex).pdf

#PDF Output files
%.pdf: %.tex
	pdflatex --draftmode $<
	pdflatex $<

#Clean
cmdcleanpdf = rm -vf *.pdf
cmdclean = rm -vf .*swp *.autosave *.pws *.bak *.aux *.def *.drv *.dvi *.glo *.idx *.log *.lot *.lof *.prv *.toc *~ *.out
cleanpdf:
	$(cmdcleanpdf)

clean: cleanimages
	$(cmdclean)
	cd report && $(cmdclean)
	cd paper && $(cmdclean)
	cd epsc && $(cmdclean)

distclean: clean cleanpdf cleanextra

#Extra
%extra:
	cd extra && $(MAKE) $@

#Images
%images:
	cd images && $(MAKE) $@

#Upload and Subversion Features
file1 = $(normal).pdf
file2 = $(normal)$(annex).pdf
file3 = $(paper).pdf
uploaduser = prodrigestivill
projectname = vpmn
browser = firefox

cleanupload:
	$(browser) "http://code.google.com/p/$(projectname)/downloads/delete?filename=$(file1)"
	$(browser) "http://code.google.com/p/$(projectname)/downloads/delete?filename=$(file2)"
	$(browser) "http://code.google.com/p/$(projectname)/downloads/delete?filename=$(file3)"

upload: cleanupload
	$(browser) "http://code.google.com/hosting/settings"
	googlecode_upload.py -s "$(file1)" -p $(projectname) -l Featured -l Type-Docs -u $(uploaduser) --config-dir=none "$(file1)"
	googlecode_upload.py -s "$(file2)" -p $(projectname) -l Featured -l Type-Docs -u $(uploaduser) --config-dir=none "$(file2)"
	googlecode_upload.py -s "$(file3)" -p $(projectname) -l Featured -l Type-Docs -u $(uploaduser) --config-dir=none "$(file3)"
