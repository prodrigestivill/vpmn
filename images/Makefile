DIAIMAGES=$(addsuffix .pdf,$(basename $(wildcard *.dia)))

.PHONY: all allimages clean cleanimages

all allimages: diaimages

diaimages: $(DIAIMAGES)

clean cleanimages:
	rm -f $(DIAIMAGES)

%.pdf: %.eps
	epstopdf --outfile=$@ $<

%.eps: %.dia
	dia $< -t eps -e $@
