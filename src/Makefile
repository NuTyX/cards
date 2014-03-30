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

TOOLSOBJECTS =	error_treat.o \
		md5.o \
		string_utils.o \
		archive_utils.o \
		file_utils.o \
		process.o \
		runtime_dependencies_utils.o \
		pkgdbh.o \
		pkgadd.o \
		pkgrm.o \
		pkginfo.o \
		tools.o

CARDSOBJECTS =	error_treat.o \
		md5.o \
		string_utils.o \
		archive_utils.o \
		config_parser.o \
		file_download.o \
		compile_dependencies_utils.o \
		argument_parser.o \
		cards_argument_parser.o \
		file_utils.o process.o \
		runtime_dependencies_utils.o \
		pkgdbh.o \
		pkgadd.o \
		pkginfo.o \
		cards_sync.o \
		cards_install.o \
		cards_depends.o \
		cards.o


LIBOBJECTS =  pkgdbh.o pkgadd.o pkgrm.o pkginfo.o

test:
	$(CXX) error_treat.cc \
	string_utils.cc archive_utils.cc \
	test.cc -o ~/bin/au $(LDFLAGS)

libs:
	$(CXX) -shared -o libcards.so.$(VERSION)  $(LIBOBJECTS) #-Wl,soname=libcards-$(VERSION)


all: pkgadd cards

pkgadd: .tools_depend $(TOOLSOBJECTS)
	$(CXX) $(TOOLSOBJECTS) -o $@ $(LDFLAGS)

cards:  .cards_depend $(CARDSOBJECTS)
	$(CXX) $(CARDSOBJECTS) -o $@ $(LDFLAGS) $(LIBCURLLIBS)

.tools_depend:
	$(CXX) $(CXXFLAGS) -MM $(TOOLSOBJECTS:.o=.cc) > .tools_depend

.cards_depend:
	$(CXX) $(CXXFLAGS) -MM $(CARDSOBJECTS:.o=.cc) > .cards_depend

ifeq (.cards_depend,$(wildcard .cards_depend))
include .cards_depend
endif

ifeq (.tools_depend,$(wildcard .tools_depend))
include .tools_depend
endif

.PHONY:	install clean distclean dist

install: all
	install -D -m0755 pkgadd $(DESTDIR)$(BINDIR)/pkgadd
	install -D -m0755 cards $(DESTDIR)$(BINDIR)/cards
	install -D -m0644 pkgadd.conf $(DESTDIR)$(ETCDIR)/pkgadd.conf
	install -D -m0644 cards.conf $(DESTDIR)$(ETCDIR)/cards.conf
	ln -sf pkgadd $(DESTDIR)$(BINDIR)/pkgrm
	ln -sf pkgadd $(DESTDIR)$(BINDIR)/pkginfo

clean:
	rm -f .tools_depend
	rm -f .cards_depend
	rm -f $(TOOLSOBJECTS)
	rm -f $(CARDSOBJECTS)
	rm -f $(MANPAGES)
	rm -f $(MANPAGES:=.txt)
	rm -f libcards.so.$(VERSION)
	rm -f pkgadd
	rm -f pkgcrea
	rm -f cards

distclean: clean
	rm -f pkgadd pkginfo pkgrm cards

# End of file
