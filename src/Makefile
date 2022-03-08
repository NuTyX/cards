#
#  cards
#
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (http://crux.nu)
#  Copyright (c) 2013 - 2022 by NuTyX team (http://nutyx.org)
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
		pkg.o \
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
		pkg.o \
		pkgrepo.o \
		file_download.o \
		repodwl.o \
		compile_dependencies_utils.o \
		argument_parser.o \
		cards_argument_parser.o \
		file_utils.o \
		process.o \
		runtime_dependencies_utils.o \
		pkgdbh.o \
		pkgadd.o \
		pkgrm.o \
		pkginfo.o \
		pkginst.o \
		pkgtest.o \
		pkgsync.o \
		cards_create.o \
		cards_sync.o \
		cards_install.o \
		cards_remove.o \
		cards_depends.o \
		cards_base.o \
		cards_info.o \
		cards_upgrade.o \
		cards.o


LIBOBJECTS =  system_utils.o \
	error_treat.o \
	md5.o \
	string_utils.o \
	archive_utils.o \
	pkgrepo.o \
	repodwl.o \
	file_download.o \
	file_utils.o \
	process.o \
	pkg.o \
	runtime_dependencies_utils.o \
	pkgdbh.o \
	pkgadd.o \
	pkgrm.o \
	pkginfo.o \
	pkginst.o \
	pkgsync.o

WEBOBJECTS =  system_utils.o \
	error_treat.o \
	md5.o \
	file_utils.o \
	string_utils.o \
	pkgrepo.o \
	mysql.o \
	pkg.o \
	webcards.o

HEADERS = system_utils.h \
		error_treat.h \
		md5.h \
		string_utils.h \
		file_utils.h \
		archive_utils.h \
		pkg.h \
		pkgrepo.h \
		repodwl.h \
		file_download.h \
		process.h \
		runtime_dependencies_utils.h \
		pkgdbh.h \
		pkgadd.h \
		pkgrm.h \
		pkginfo.h \
		pkginst.h \
		pkgsync.h


CONSOLEKIT_CFLAGS = -I/usr/include/ConsoleKit/ck-connector -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include
CONSOLEKIT_LIBS = -lck-connector -ldbus-1
GLIB_CFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
GLIB_LIBS = -lglib-2.0


libs:	.libs_depend $(LIBOBJECTS)
	$(CXX) -shared -o libcards.so.$(VERSION)  $(LIBOBJECTS) #-Wl,soname=libcards-$(VERSION)
	$(AR) -r libcards.a $(LIBOBJECTS)
	$(RANLIB) libcards.a
	-@cat ../COPYING > libcards.h
	-@echo "#ifndef LIB_CARDS_INCLUDED" >> libcards.h
	-@echo "#define LIB_CARDS_INCLUDED" >> libcards.h
	-@cat $(HEADERS)|grep -v '^#define SYSTEM_UTIL' \
	|grep -v '^#define ERROR'|grep -v '^#define MD5' \
	|grep -v '^#define STRING'|grep -v '^#define ARCHIVE' \
	|grep -v '^#define PKG'|grep -v '^#define RUNTIME' \
	|grep -v '^#define FILE'|grep -v '^#define PROCESS' \
	|grep -v '^#define DEPENDENCIES' \
	|grep -v '^#include \"'|grep -v '^#ifndef' \
	|grep -v '^#endif /*'|grep -v '^//' >> libcards.h
	-@echo "#endif" >> libcards.h
	-@echo "// vim:set ts=2 :" >> libcards.h

all: setup-nutyx-log pkgadd cards conf libs

webcards: .web_depend $(WEBOBJECTS)
	$(CXX) $(WEBOBJECTS) -o index.cgi $(LIBMYSQLCLIENT)
	echo "index.cgi is ready"

setup-nutyx-log:
	${CC} system_log.c -o $@
conf:
	sed -e "s/#ARCH#/`uname -m`/" cards.conf.in > cards.conf

pkgadd: .tools_depend $(TOOLSOBJECTS)
	$(CXX) $(TOOLSOBJECTS) -o $@ -static $(LIBARCHIVELIBS) 
	echo "$@ is ready"

cards:  .cards_depend $(CARDSOBJECTS)
	$(CXX) $(CARDSOBJECTS) -o $@ $(LDFLAGS) $(LIBCURLLIBS)
	echo "$@ is ready"

.libs_depend:
	$(CXX) $(CXXFLAGS) -MM $(LIBOBJECTS:.o=.cxx) > .libs_depend

.tools_depend:
	$(CXX) $(CXXFLAGS) -MM $(TOOLSOBJECTS:.o=.cxx) > .tools_depend

.cards_depend:
	$(CXX) $(CXXFLAGS) -MM $(CARDSOBJECTS:.o=.cxx) > .cards_depend

.web_depend:
	$(CXX) $(CXXFLAGS) -MM $(WEBOBJECTS:.o=.cxx) > .web_depend

ifeq (.cards_depend,$(wildcard .cards_depend))
include .cards_depend
endif

ifeq (.tools_depend,$(wildcard .tools_depend))
include .tools_depend
endif

ifeq (.web_depend,$(wildcard .web_depend))
include .web_depend
endif

%: %.conf.in
	sed -e "s/#ARCH#/`uname -m`/" $< > $@

.PHONY:	install clean distclean dist

install: all install-libs
	install -v -D -m0755 pkgadd $(BINDIR)/pkgadd
	install -v -D -m0755 cards $(BINDIR)/cards
	install -v -D -m0755 setup-nutyx-log $(SBINDIR)/setup-nutyx-log
	install -v -D -m0644 pkgadd.conf $(VARDIR)/pkgadd.conf
	install -v -D -m0644 cards.conf $(ETCDIR)/cards.conf.example
	ln -sfv pkgadd $(BINDIR)/pkgrm
	ln -sfv pkgadd $(BINDIR)/pkginfo

install-libs:
	if [ ! -d $(DESTDIR)/usr/include ]; then mkdir -pv $(DESTDIR)/usr/include;fi
	cp -fv libcards.h $(DESTDIR)/usr/include/
	if [ ! -d $(LIBDIR) ]; then mkdir -pv $(LIBDIR);fi
	if [ -L $(LIBDIR)/libcards.so ]; then rm -v $(LIBDIR)/libcards.so;fi
	if [ -L $(LIBDIR)/libcards.a ]; then rm -v $(LIBDIR)/libcards.a;fi
	cp -fv libcards.a libcards.so.$(VERSION) $(LIBDIR)
	ln -sfv libcards.so.$(VERSION) $(LIBDIR)/libcards.so

install-webcards:
	install -v -D -m0755  index.cgi $(DESTDIR)/index.cgi

clean:
	rm -fv .tools_depend
	rm -fv .cards_depend
	rm -fv .libs_depend
	rm -fv .web_depend
	rm -fv $(TOOLSOBJECTS)
	rm -fv $(CARDSOBJECTS)
	rm -fv $(WEBOBJECTS)
	rm -fv $(MANPAGES)
	rm -fv $(MANPAGES:=.txt)
	rm -fv libcards.so.$(VERSION)
	rm -fv libcards.h
	rm -fv libcards.a
	rm -fv setup-nutyx-log
	rm -fv test
	rm -fv pkgadd
	rm -fv cards
	rm -fv cards.conf
	rm -fv index.cgi

distclean: clean
	rm -fv pkgadd pkginfo pkgrm cards

# End of file
