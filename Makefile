include Makefile.inc

DIRSRC = src

DIRMAN = man

.PHONY: cards

all:
	$(MAKE) -C $(DIRSRC) all
	$(MAKE) -C $(DIRMAN) all

install: all

	$(MAKE) -C $(DIRSRC) install
	$(MAKE) -C $(DIRMAN) install

pkgadd:

	$(MAKE) -C $(DIRSRC) pkgadd

cards:

	$(MAKE) -C $(DIRSRC) cards

man:
	$(MAKE) -C $(DIRMAN) man
clean:

	$(MAKE) -C $(DIRSRC) clean
	$(MAKE) -C $(DIRMAN) clean

dist: distclean

	rm -rf $(NAME) $(NAME).tar.gz
	git archive --format=tar --prefix=$(NAME)/ HEAD | tar -x
	git log > $(NAME)/ChangeLog
	tar czvf $(NAME).tar.gz $(NAME)
	rm -rf $(NAME)

distclean:

	$(MAKE) -C $(DIRSRC) distclean
