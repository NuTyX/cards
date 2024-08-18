#  SPDX-License-Identifier: LGPL-2.1-or-later
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
#  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

include ../Makefile.inc

TOOLSOBJECTS =	system_utils.o \
		error_treat.o \
		string.o \
		vector.o \
		string_utils.o \
		archive.o \
		file_utils.o \
		process.o \
		pkg.o \
		db.o \
		cache.o \
		runtime_dependencies_utils.o \
		lock.o \
		pkgdbh.o \
		pkgadd.o \
		pkgrm.o \
		pkginfo.o \
		tools.o

CARDSOBJECTS =	conf.o \
		system_utils.o \
		error_treat.o \
		string.o \
		vector.o \
		string_utils.o \
		archive.o \
		pkg.o \
		lock.o \
		db.o \
		cache.o \
		pkgfile.o \
		pkgrepo.o \
		repo.o \
		file_download.o \
		repodwl.o \
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
		deptree.o \
		level.o \
		depends.o \
		sync.o \
		create.o \
		cards_install.o \
		cards_remove.o \
		cards_base.o \
		cards_info.o \
		upgrade.o \
		cards.o

LIBOBJECTS =  conf.o \
	system_utils.o \
	error_treat.o \
	string.o \
	vector.o \
	string_utils.o \
	archive.o \
	pkgfile.o \
	pkgrepo.o \
	repodwl.o \
	file_download.o \
	file_utils.o \
	process.o \
	pkg.o \
	lock.o \
	db.o \
	cache.o \
	sync.o \
	runtime_dependencies_utils.o \
	pkgdbh.o \
	pkgadd.o \
	pkgrm.o \
	pkginfo.o \
	pkginst.o

WEBOBJECTS =  conf.o \
	system_utils.o \
	error_treat.o \
	vector.o \
	file_utils.o \
	string_utils.o \
	repo.o \
	mysql.o \
	pkg.o \
	db.o \
	cache.o \
	webcards.o

HEADERS = define.h \
	enum.h \
	system_utils.h \
	error_treat.h \
	string.h \
	vector.h \
	string_utils.h \
	file_utils.h \
	archive.h \
	conf.h \
	pkg.h \
	lock.h \
	db.h  \
	cache.h \
	sync.h \
	pkgfile.h \
	pkgrepo.h \
	repo.h \
	repodwl.h \
	file_download.h \
	process.h \
	runtime_dependencies_utils.h \
	pkgdbh.h \
	pkgadd.h \
	pkgrm.h \
	pkginfo.h \
	pkginst.h

CONSOLEKIT_CFLAGS = -I/usr/include/ConsoleKit/ck-connector -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include
CONSOLEKIT_LIBS = -lck-connector -ldbus-1
GLIB_CFLAGS = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
GLIB_LIBS = -lglib-2.0


libs:	.libs_depend $(LIBOBJECTS)
	$(CXX) -shared -o libcards.so.$(VERSION)  $(LIBOBJECTS) #-Wl,soname=libcards-$(VERSION)
	$(AR) -r libcards.a $(LIBOBJECTS)
	$(RANLIB) libcards.a
	-@echo "/*" > libcards.h
	-@cat ../LICENSE.GPL2 >> libcards.h
	-@echo "*/" >> libcards.h
	-@echo "#ifndef LIB_CARDS_INCLUDED" >> libcards.h
	-@echo "#define LIB_CARDS_INCLUDED" >> libcards.h
	-@cat $(HEADERS)|grep -v '^#define SYSTEM_UTIL' \
	|grep -v 'SPDX-License-Identifier' \
	|grep -v '^#define ERROR'|grep -v '^#define MD5' \
	|grep -v '^#define STRING'|grep -v '^#define ARCHIVE' \
	|grep -v '^#define PKG'|grep -v '^#define RUNTIME' \
	|grep -v '^#pragma'|grep -v '^#define CACHE' \
	|grep -v '^#define FILE'|grep -v '^#define PROCESS' \
	|grep -v '^#define DEPENDENCIES' \
	|grep -v '^#include "'|grep -v '^#ifndef' \
	|grep -v '^#endif /*'|grep -v '^//' >> libcards.h
	-@echo "#endif" >> libcards.h

all: setup-nutyx-log pkgadd cards conf libs

webcards: .web_depend $(WEBOBJECTS)
	$(CXX) $(WEBOBJECTS) -o index.cgi $(LIBMYSQLCLIENT)
	echo "index.cgi is ready"

setup-nutyx-log:
	${CC} system_log.c -o $@
conf:
	sed -e "s/#ARCH#/`uname -m`/" cards.conf.in > cards.conf

pkgadd: .tools_depend $(TOOLSOBJECTS)
	$(CXX) -g $(TOOLSOBJECTS) -o $@ -static $(LIBARCHIVELIBS) 
	echo "$@ is ready"

cards:  .cards_depend $(CARDSOBJECTS)
	$(CXX) -g $(CARDSOBJECTS) -o $@ $(LDFLAGS) $(LIBCURLLIBS)
	echo "$@ is ready"

.libs_depend:
	$(CXX) -g $(CXXFLAGS) -MM $(LIBOBJECTS:.o=.cxx) > .libs_depend

.tools_depend:
	$(CXX) -g $(CXXFLAGS) -MM $(TOOLSOBJECTS:.o=.cxx) > .tools_depend

.cards_depend:
	$(CXX) -g $(CXXFLAGS) -MM $(CARDSOBJECTS:.o=.cxx) > .cards_depend

.web_depend:
	$(CXX) -g $(CXXFLAGS) -MM $(WEBOBJECTS:.o=.cxx) > .web_depend

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
