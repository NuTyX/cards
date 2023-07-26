#
#  flcards
#
#  Copyright (c) 2016 - 2023 by NuTyX team (http://nutyx.org)
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

LIBFLTK  := $(shell fltk-config --ldflags)

FLOBJECTS = cards_event_handler.o \
	    cards_log.o \
	    cards_client.o \
	    cards_wrapper.o \
	    table_base.o \
	    table_package.o \
	    table_collection.o \
	    progressbox.o \
	    mainWindow.o \
	    flcards.o

.flcards_depend:
	$(CXX) -g $(CXXFLAGS) -MM $(FLOBJECTS:.o=.cxx) > .flcards_depend

flcards: .flcards_depend $(FLOBJECTS)
	$(CXX) $(FLOBJECTS) -o $@ $(LDFLAGS) $(LIBCURLLIBS) $(LIBFLTK) -lcards
	echo "$@ is ready"

	rm flcards.o mainWindow.o table_base.o table_package.o table_collection.o cards_wrapper.o cards_client.o cards_event_handler.o cards_log.o progressbox.o

clean:
	rm -fv .flcards_depend
	rm -fv $(FLOBJECTS)
	rm -fv flcards

install:
	install -D -m0755 flcards $(USRBINDIR)/flcards
	install -D -m0644 pixmaps/flcards.xpm  $(DESTDIR)/usr/share/pixmaps/flcards.xpm
	install -D -m0644 flcards.desktop $(DESTDIR)/usr/share/applications/flcards.desktop

# End of file
