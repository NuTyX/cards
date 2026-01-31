#  SPDX-License-Identifier: LGPL-2.1-or-later
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
#  Copyright (c) 2013 - 2026 by NuTyX team (https://nutyx.org)

DESTDIR =
PREFIX = /usr
USRBINDIR = ${DESTDIR}/${PREFIX}/bin
BINDIR = ${DESTDIR}/${PREFIX}/bin
SBINDIR = ${DESTDIR}/${PREFIX}/sbin
MANDIR = ${DESTDIR}/${PREFIX}/share/man
SOUNDDIR = ${DESTDIR}/${PREFIX}/share/sounds/cards
LIBDIR = ${DESTDIR}/${PREFIX}/lib
ETCDIR = ${DESTDIR}/etc
VARDIR = ${DESTDIR}/var/lib/pkg
LOCALEDIR = ${DESTDIR}/${PREFIX}/share/locale
DOCDIR    = ${DESTDIR}/${PREFIX}/share/doc
APPDIR    = ${DESTDIR}/${PREFIX}/share/applications

INSTALL   = /usr/bin/install -c
RMDIR     = rm -rf
DOXYDOC   = /usr/bin/doxygen
AR?=ar
RANLIB?=ranlib

INSTALL_BIN  = $(INSTALL) -m 755
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_DIR  = $(INSTALL) -d


VERSION = 3.4.1

NAME = cards-$(VERSION)

CXXFLAGS += -DNDEBUG -std=c++17
CXXFLAGS += -O2 -Werror -pedantic -D_GNU_SOURCE -DPACKAGE_VERSION=\"$(VERSION)\" \
	    -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC
CFLAGS += -DNDEBUG -std=gnu11
CFLAGS += -D_GNU_SOURCE

LIBARCHIVELIBS := $(shell pkg-config --libs --static libarchive)
LIBCURLLIBS := $(shell pkg-config --libs --static libcurl)

LIBMYSQLCLIENT := -lmysqlclient

LDFLAGS += $(LIBARCHIVELIBS)

.SUFFIXES:	.cpp .cc .cxx .c

# Be quiet when building...
.SILENT:

.cxx.o:
	echo compiling $< ...
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
