#  SPDX-License-Identifier: LGPL-2.1-or-later
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
#  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

include ../Makefile.inc

all: pkgmk mybld cards resolve-libs libcards.pc

%: %.in
	sed -e "s/#VERSION#/$(VERSION)/" $< > $@


.PHONY:	install clean distclean dist

install: all
	install -D -m0644 ding.wav ${SOUNDDIR}/ding.wav
	install -D -m0755 mybld $(BINDIR)/mybld
	install -D -m0755 resolve-libs $(BINDIR)/resolve-libs
	install -D -m0755 pkgmk $(USRBINDIR)/pkgmk
	install -D -m0644 pkgmk.conf $(ETCDIR)/pkgmk.conf.example
	install -D -m0644 libcards.pc $(LIBDIR)/pkgconfig/libcards.pc


clean:
	rm -f pkgmk
	rm -f mybld
	rm -f resolve-libs
	rm -f cards
	rm -f libcards.pc

distclean: clean
