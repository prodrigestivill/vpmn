all: pdf
pdf: cleanpdf allextra normal

#Default Output PDFs
paper = paper
normal = report
epsc = epsc_vpmn
annex = _annex
withannex = _all

paper: $(paper).pdf

normal: $(normal).pdf $(normal)$(annex).pdf

epsc: $(epsc).pdf $(epsc)$(annex).pdf $(epsc)$(withannex).pdf

#PDF Output files
%.pdf: %.tex
	pdflatex --draftmode $<
	pdflatex $<

#Clean
cmdcleanpdf = rm -vf *.pdf
cmdclean = rm -vf .*swp *.autosave *.pws *.bak *.aux *.def *.drv *.dvi *.glo *.idx *.log *.lot *.lof *.prv *.toc *~
cleanpdf:
	$(cmdcleanpdf)

clean:
	$(cmdclean)
	cd report && $(cmdclean)
	cd paper && $(cmdclean)
	cd epsc && $(cmdclean)

distclean: clean cleanpdf cleanextra

#Extra
%extra:
	cd extra && $(MAKE) $@

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

ci: commit

commit:
	svn ci

up: update

update:
	svn up
