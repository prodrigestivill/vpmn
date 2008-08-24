DIAIMAGES=$(addsuffix .pdf,$(basename $(wildcard *.dia)))
PLOTIMAGES=$(addsuffix .pdf,$(basename $(wildcard *.gnu)))

.PHONY: all allimages clean cleanimages diaimages plotimages

all allimages: diaimages plotimages

diaimages: $(DIAIMAGES)
plotimages: $(PLOTIMAGES)

clean cleanimages:
	rm -f $(DIAIMAGES) $(PLOTIMAGES)

%.pdf: %.eps
	epstopdf --outfile=$@ $<

%.eps: %.dia
	dia $< -t eps -e $@

%.eps: %.gnu
	gnuplot $< > $@
