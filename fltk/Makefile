#
#  flcards
#
#  Copyright (c) 2016 by NuTyX team (http://nutyx.org)
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

flcards:
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DVERSION=$(VERSION) -fPIC -c -o mainWindow.o mainWindow.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o flcards.o flcards.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o tableau.o tableau.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o cards_wrapper.o cards_wrapper.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o cards_client.o cards_client.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o cards_event_handler.o cards_event_handler.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o cards_package.o cards_package.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o pack_list.o pack_list.cxx
	$(CXX) -DNDEBUG -std=c++11 -O2 -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o progressbox.o progressbox.cxx

	$(CXX) flcards.o tableau.o mainWindow.o cards_wrapper.o cards_client.o cards_event_handler.o cards_package.o pack_list.o progressbox.o -o $@ -lcards -lcurl -larchive $(LIBFLTK)
	rm flcards.o mainWindow.o tableau.o cards_wrapper.o cards_client.o cards_event_handler.o cards_package.o pack_list.o progressbox.o

clean:
	rm -f flcards flcards.o mainWindow.o tableau.o cards_wrapper.o cards_client.o cards_event_handler.o cards_package.o pack_list.o progressbox.o

install:
	install -D -m0755 flcards $(USRBINDIR)/flcards
	install -D -m0644 pixmaps/flcards.xpm  $(DESTDIR)/usr/share/pixmaps/flcards.xpm
	install -D -m0644 flcards.desktop $(DESTDIR)/usr/share/applications/flcards.desktop

# End of file
