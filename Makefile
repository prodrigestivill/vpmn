texjob = Main
uploaduser = prodrigestivill
projectname = vpmn
browser = firefox

texfile = $(texjob).tex
pdffile = $(texjob).pdf

all: cleanpdfs pdf

pdf: $(pdffile)

$(pdffile):
	pdflatex "$(texfile)"

clean: cleanpdfs

cleanpdfs:
	rm -vf "$(pdffile)"

cleanupload:
	$(browser) "http://code.google.com/p/$(projectname)/downloads/delete?filename=$(pdffile)"

upload: cleanupload
	$(browser) "http://code.google.com/hosting/settings"
	googlecode_upload.py -s "$(texjob)" -p $(projectname) -l Featured -l Type-Docs -u $(uploaduser) --config-dir=none "$(pdffile)"

ci: commit

commit:
	svn ci

up: update

update:
	svn up
