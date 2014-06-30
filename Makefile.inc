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

DESTDIR =
BINDIR = /bin
MANDIR = /usr/share/man
LIBDIR = /usr/lib
ETCDIR = /etc

VERSION = 0.9.80.0
NAME = cards-$(VERSION)

CXXFLAGS += -DNDEBUG
CXXFLAGS += -O2 -Wall -pedantic -D_GNU_SOURCE -DVERSION=\"$(VERSION)\" \
	    -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC
CFLAGS += -DNDEBUG
CFLAGS += -D_GNU_SOURCE

LIBARCHIVELIBS := $(shell pkg-config --libs --static libarchive)
LIBCURLLIBS := $(shell pkg-config --libs --static libcurl)

LDFLAGS += $(LIBARCHIVELIBS)


# End of file
