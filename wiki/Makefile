.PHONY: all clean

PAGES=index
-include .depend

PANDOC=pandoc -s --template template -V wikiroot=$(ROOT)/html/ -c $(ROOT)/css/style.css --toc
PERL=perl
RM=rm -f

ROOT=$(shell pwd)
HTMLPAGES=$(addprefix html/,$(PAGES))
ALLDIRS=$(sort $(dir $(HTMLPAGES)))

all: .depend $(ALLDIRS) $(addprefix html/,$(PAGES))

include mk/special.mk

$(ALLDIRS):
	mkdir -p $@

html/%: src/%.mkd
	$(PANDOC) $< -t html -o $@

.depend: scripts/links.pl $(shell find src -name \*.mkd)
	$(PERL) $+ > $@

clean:
	$(RM) -r html .depend $(CLEAN) *~
