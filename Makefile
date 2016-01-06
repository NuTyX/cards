include Makefile.inc
DIRSRC = src

DIRMAN = man

DIRTEST = tests

DIRSCRIPTS = scripts

.PHONY: cards

libs:
	$(MAKE) -C $(DIRSRC) libs

test:
	$(MAKE) -C $(DIRTEST) test

all:
	$(MAKE) -C $(DIRSRC) all
	$(MAKE) -C $(DIRSCRIPTS) all
	$(MAKE) -C $(DIRMAN) all

install: all

	$(MAKE) -C $(DIRSRC) install
	$(MAKE) -C $(DIRSCRIPTS) install
	$(MAKE) -C $(DIRMAN) install

pkgadd:

	$(MAKE) -C $(DIRSRC) pkgadd

ports:

	$(MAKE) -C $(DIRSCRIPTS) ports

pkgmk:

	$(MAKE) -C $(DIRSCRIPTS) pkgmk

pkgmkall:

	$(MAKE) -C $(DIRSCRIPTS) pkgmkall

cards:

	$(MAKE) -C $(DIRSRC) cards

man:
	$(MAKE) -C $(DIRMAN) man
clean:

	$(MAKE) -C $(DIRSRC) clean
	$(MAKE) -C $(DIRSCRIPTS) clean
	$(MAKE) -C $(DIRMAN) clean

dist: distclean

	rm -rf $(NAME) $(NAME).tar.gz
	git archive --format=tar --prefix=$(NAME)/ HEAD | tar -x
	git log > $(NAME)/ChangeLog
	tar czvf $(NAME).tar.gz $(NAME)
	rm -rf $(NAME)

distclean:

	$(MAKE) -C $(DIRSRC) distclean
