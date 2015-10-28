#
#  cards
#
#  Copyright (c) 2000-2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
#  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
#  USA.
#

include ../Makefile.inc

MANPAGES = cards.8 pkgadd.8 pkgrm.8 pkginfo.8 pkgmk.8 pkgmk.8.fr cards.conf.5 pkgmk.conf.5 pkgmk.conf.5.fr

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
	install -D -m0644 cards.8 $(DESTDIR)$(MANDIR)/man8/cards.8
	install -D -m0644 cards.conf.5 $(DESTDIR)$(MANDIR)/man5/cards.conf.5
	install -D -m0644 ports.8 $(DESTDIR)$(MANDIR)/man8/ports.8
	install -D -m0644 pkgadd.8 $(DESTDIR)$(MANDIR)/man8/pkgadd.8
	install -D -m0644 pkgrm.8 $(DESTDIR)$(MANDIR)/man8/pkgrm.8
	install -D -m0644 pkginfo.8 $(DESTDIR)$(MANDIR)/man8/pkginfo.8
	install -D -m0644 pkgmk.8 $(DESTDIR)$(MANDIR)/man8/pkgmk.8
	install -D -m0644 pkgmk.conf.5 $(DESTDIR)$(MANDIR)/man5/pkgmk.conf.5
	install -D -m0644 pkgmk.conf.5.fr $(DESTDIR)$(MANDIR)/fr/man5/pkgmk.conf.5
	install -D -m0644 pkgmk.8.fr $(DESTDIR)$(MANDIR)/fr/man8/pkgmk.8

clean:
	rm -f $(MANPAGES)
	rm -f $(MANPAGES:=.txt)

# End of file
