# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
# Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

include ../Makefile.inc

LIBFLTK  := $(shell fltk-config --ldflags)

FLOBJECTS = cards_event_handler.o \
	    cards_log.o \
	    cards_client.o \
	    cards_wrapper.o \
	    table_base.o \
	    table_package.o \
	    progressbox.o \
	    mainWindow.o \
	    flcards.o

.flcards_depend:
	$(CXX) -g $(CXXFLAGS) -MM $(FLOBJECTS:.o=.cxx) > .flcards_depend

flcards: .flcards_depend $(FLOBJECTS)
	$(CXX) $(FLOBJECTS) -o $@ $(LDFLAGS) $(LIBCURLLIBS) $(LIBFLTK) -lcards
	echo "$@ is ready"

clean:
	rm -fv .flcards_depend
	rm -fv $(FLOBJECTS)
	rm -fv flcards

install:
	install -D -m0755 flcards $(USRBINDIR)/flcards
	install -D -m0644 pixmaps/flcards.xpm  $(DESTDIR)/usr/share/pixmaps/flcards.xpm
	install -D -m0644 flcards.desktop $(DESTDIR)/usr/share/applications/flcards.desktop
