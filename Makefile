include Makefile.inc
DIRSRC = src

DIRMAN = man

DIRPO = po

DIRTEST = tests

DIRSCRIPTS = scripts

DIRFLTK = fltk

DIRDOC = documentation

.PHONY: cards

html:
	$(MAKE) -C $(DIRDOC) html

webcards:
	$(MAKE) -C $(DIRSRC) webcards

flcards:
	$(MAKE) -C $(DIRFLTK) flcards

libs:
	$(MAKE) -C $(DIRSRC) libs

test:
	$(MAKE) -C $(DIRTEST) test

all:
	$(MAKE) -C $(DIRSRC) all
	$(MAKE) -C $(DIRSCRIPTS) all
	$(MAKE) -C $(DIRMAN) all
	$(MAKE) -C $(DIRPO) all

install: all

	$(MAKE) -C $(DIRSRC) install
	$(MAKE) -C $(DIRSCRIPTS) install
	$(MAKE) -C $(DIRMAN) install
	$(MAKE) -C $(DIRPO) install

install-libs:
	$(MAKE) -C $(DIRSRC) install-libs

pkgadd:

	$(MAKE) -C $(DIRSRC) pkgadd

pkgmk:

	$(MAKE) -C $(DIRSCRIPTS) pkgmk

cards:

	$(MAKE) -C $(DIRSRC) cards

man:
	$(MAKE) -C $(DIRMAN) man

clean:

	$(MAKE) -C $(DIRSRC) clean
	$(MAKE) -C $(DIRSCRIPTS) clean
	$(MAKE) -C $(DIRMAN) clean
	$(MAKE) -C $(DIRPO) clean
	$(MAKE) -C $(DIRTEST) clean
	$(MAKE) -C $(DIRFLTK) clean
	$(MAKE) -C $(DIRDOC) clean

dist: distclean

	rm -rf $(NAME) $(NAME).tar.gz
	git archive --format=tar --prefix=$(NAME)/ HEAD | tar -x
	git log > $(NAME)/ChangeLog
	tar czvf $(NAME).tar.gz $(NAME)
	rm -rf $(NAME)

distclean:

	$(MAKE) -C $(DIRSRC) distclean
