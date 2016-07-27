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

all: pkgmk pkgmk2 ports build-port build-collection

%: %.in
	sed -e "s/#VERSION#/$(VERSION)/" $< > $@


.PHONY:	install clean distclean dist

install: all
	install -D -m0755 build-port $(DESTDIR)$(USRBINDIR)/build-port
	install -D -m0755 build-collection $(DESTDIR)$(BINDIR)/build-collection
	install -D -m0755 pkgmk $(DESTDIR)$(USRBINDIR)/pkgmk
	install -D -m0755 pkgmk2 $(DESTDIR)$(USRBINDIR)/pkgmk2
	install -D -m0644 pkgmk.conf $(DESTDIR)$(ETCDIR)/pkgmk.conf.example
	install -D -m0755 ports $(DESTDIR)$(USRBINDIR)/ports
	install -D -m0755 cards.driver $(DESTDIR)/var/lib/pkg/drivers/cards


clean:
	rm -f pkgmk
	rm -f pkgmk2
	rm -f ports
	rm -f build-port
	rm -f build-collection

distclean: clean

# End of file
