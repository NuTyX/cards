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


DESTDIR =
USRBINDIR = ${DESTDIR}/usr/bin
BINDIR = ${DESTDIR}/bin
SBINDIR = ${DESTDIR}/sbin
MANDIR = ${DESTDIR}/usr/share/man
LIBDIR = ${DESTDIR}/usr/lib
ETCDIR = ${DESTDIR}/etc
VARDIR = ${DESTDIR}/var/lib/pkg
LOCALEDIR = ${DESTDIR}/usr/share/locale
DOCDIR    = ${DESTDIR}/usr/share/doc
APPDIR    = ${DESTDIR}/usr/share/applications

INSTALL   = /usr/bin/install -c
RMDIR     = rm -rf
DOXYDOC   = /usr/bin/doxygen
AR?=ar
RANLIB?=ranlib

INSTALL_BIN  = $(INSTALL) -m 755
INSTALL_DATA = $(INSTALL) -m 644
INSTALL_DIR  = $(INSTALL) -d


VERSION = 2.3.94

NAME = cards-$(VERSION)

CXXFLAGS += -DNDEBUG -std=c++11
CXXFLAGS += -O2 -Werror -pedantic -D_GNU_SOURCE -DVERSION=\"$(VERSION)\" \
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

# End of file
