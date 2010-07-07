.placeholder.tmp:
	echo 'WIKI_MK_PLACEHOLDER' > $@

html/calendar: .placeholder.tmp
	$(PANDOC) -c $(ROOT)/css/pal.css $< -t html -o $@
	$(PERL) scripts/generate.pl 'pal --html' $@
	$(RM) $@.bak

html/gtd: $(HOME)/.todo.txt
	echo '~~~~'  > .gtd.tmp
	hsgtd list  >> .gtd.tmp
	echo '~~~~' >> .gtd.tmp
	$(PANDOC) .gtd.tmp -t html -o $@
	$(RM) .gtd.tmp

CLEAN+=.placeholder.tmp
