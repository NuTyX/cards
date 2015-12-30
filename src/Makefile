#
#  cards
#
#  Copyright (c) 2000-2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
#  Copyright (c) 2013-2016 by NuTyX team (http://nutyx.org)
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

TOOLSOBJECTS =	system_utils.o \
		error_treat.o \
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

CARDSOBJECTS =	system_utils.o \
		error_treat.o \
		md5.o \
		string_utils.o \
		archive_utils.o \
		pkgrepo.o \
		file_download.o \
		compile_dependencies_utils.o \
		argument_parser.o \
		cards_argument_parser.o \
		file_utils.o process.o \
		runtime_dependencies_utils.o \
		pkgdbh.o \
		pkgadd.o \
		pkgrm.o \
		pkginfo.o \
		pkginst.o \
		pkgtest.o \
		cards_sync.o \
		cards_install.o \
		cards_remove.o \
		cards_depends.o \
		cards_base.o \
		cards_info.o \
		cards_upgrade.o \
		cards.o


LIBOBJECTS =  system_utils.o \
	md5.o \
	string_utils.o \
	archive_utils.o \
	pkgrepo.o \
	file_download.o \
	file_utils.o process.o \
	runtime_dependencies_utils.o \
	pkgdbh.o \
	pkgadd.o \
	pkgrm.o \
	pkginfo.o

CONSOLEKIT_CFLAGS = -I/usr/include/ConsoleKit/ck-connector -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include
CONSOLEKIT_LIBS = -lck-connector -ldbus-1
GLIB_CFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
GLIB_LIBS = -lglib-2.0


test:
	$(CXX) -DNDEBUG system_utils.cpp error_treat.cpp md5.cpp string_utils.cpp \
	file_utils.cpp archive_utils.cpp  pkgrepo.h \
	pkgrepo.cpp file_download.cpp \
	test.cpp -o $@ $(LDFLAGS) $(LIBCURLLIBS)

libs:	.libs_depend $(LIBOBJECTS)
	$(CXX) -shared -o libcards.so.$(VERSION)  $(LIBOBJECTS) #-Wl,soname=libcards-$(VERSION)

all: pkgadd cards conf libs

conf:
	sed -e "s/#ARCH#/`uname -m`/" cards.conf.in > cards.conf

pkgadd: .tools_depend $(TOOLSOBJECTS)
	$(CXX) $(TOOLSOBJECTS) -o $@ -static $(LIBARCHIVELIBS) 

cards:  .cards_depend $(CARDSOBJECTS)
	$(CXX) $(CARDSOBJECTS) -o $@ $(LDFLAGS) $(LIBCURLLIBS)

.libs_depend:
	$(CXX) $(CXXFLAGS) -MM $(LIBOBJECTS:.o=.cpp) > .libs_depend

.tools_depend:
	$(CXX) $(CXXFLAGS) -MM $(TOOLSOBJECTS:.o=.cpp) > .tools_depend

.cards_depend:
	$(CXX) $(CXXFLAGS) -MM $(CARDSOBJECTS:.o=.cpp) > .cards_depend

ifeq (.cards_depend,$(wildcard .cards_depend))
include .cards_depend
endif

ifeq (.tools_depend,$(wildcard .tools_depend))
include .tools_depend
endif

%: %.conf.in
	sed -e "s/#ARCH#/`uname -m`/" $< > $@

.PHONY:	install clean distclean dist

install: all
	install -D -m0755 pkgadd $(DESTDIR)$(USRBINDIR)/pkgadd
	install -D -m0755 cards $(DESTDIR)$(BINDIR)/cards
	install -D -m0644 pkgadd.conf $(DESTDIR)$(ETCDIR)/pkgadd.conf
	install -D -m0644 cards.conf $(DESTDIR)$(ETCDIR)/cards.conf.example
	ln -sf pkgadd $(DESTDIR)$(USRBINDIR)/pkgrm
	ln -sf pkgadd $(DESTDIR)$(USRBINDIR)/pkginfo

clean:
	rm -f .tools_depend
	rm -f .cards_depend
	rm -f .libs_depend
	rm -f $(TOOLSOBJECTS)
	rm -f $(CARDSOBJECTS)
	rm -f $(MANPAGES)
	rm -f $(MANPAGES:=.txt)
	rm -f libcards.so.$(VERSION)
	rm -f test
	rm -f pkgadd
	rm -f pkgcrea
	rm -f cards
	rm -f cards.conf

distclean: clean
	rm -f pkgadd pkginfo pkgrm cards

# End of file
