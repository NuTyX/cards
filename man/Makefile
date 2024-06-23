#  SPDX-License-Identifier: LGPL-2.1-or-later
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
#  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

include ../Makefile.inc

MANPAGES = mybld.8 cards.8 pkgadd.8 pkgrm.8 pkginfo.8 pkgmk.8 pkgmk.8.fr cards.conf.5 pkgmk.conf.5 pkgmk.conf.5.fr

all: man mantxt

man: $(MANPAGES)

mantxt: man $(MANPAGES:=.txt)

%.8.txt: %.8
	nroff -mandoc -c $< | col -bx > $@

%.fr.txt: %.fr
	nroff -mandoc -c $< | col -bx > $@

%.5.txt: %.5
	nroff -mandoc -c $< | col -bx > $@

%: %.in
	sed -e "s/#VERSION#/$(VERSION)/" $< > $@


install: all
	install -D -m0644 cards.8 $(MANDIR)/man8/cards.8
	install -D -m0644 mybld.8 $(MANDIR)/man8/mybld.8
	install -D -m0644 cards.conf.5 $(MANDIR)/man5/cards.conf.5
	install -D -m0644 pkgadd.8 $(MANDIR)/man8/pkgadd.8
	install -D -m0644 pkgrm.8 $(MANDIR)/man8/pkgrm.8
	install -D -m0644 pkginfo.8 $(MANDIR)/man8/pkginfo.8
	install -D -m0644 pkgmk.8 $(MANDIR)/man8/pkgmk.8
	install -D -m0644 pkgmk.conf.5 $(MANDIR)/man5/pkgmk.conf.5
	install -D -m0644 pkgmk.conf.5.fr $(MANDIR)/fr/man5/pkgmk.conf.5
	install -D -m0644 pkgmk.8.fr $(MANDIR)/fr/man8/pkgmk.8

clean:
	rm -f $(MANPAGES)
	rm -f $(MANPAGES:=.txt)
