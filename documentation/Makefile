#
#  cards
#
#  Copyright (c) 2000-2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
#  Copyright (c) 2013-2021 by NuTyX team (http://nutyx.org)
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

.PHONY: html

SRC_DOCDIR      = 	./src

%: %.in
	sed -e "s/#VERSION#/$(VERSION)/" $< > $@


all: html

# you need an installed version of doxygen for this
dist:	all html

# synonym for dist
alldocs: dist

clean:
	$(RM) Doxyfile
	$(RM) cards.desktop
	$(RMDIR) htdocs latex
	$(RM) *~ *.bck *.bak *.log


install: html
	echo "Installing documentation files in $(DOCDIR)/$(NAME) ..."
	-$(INSTALL_DIR) $(DOCDIR)/$(NAME)
	if test -f htdocs/index.html ; then \
	  for file in htdocs/* ; do \
		$(INSTALL_DATA) $$file $(DOCDIR)/$(NAME); \
	  done \
	fi
	install -v -D -m0644 cards.desktop $(APPDIR)/cards.desktop

uninstall:
	$(RMDIR) $(DOCDIR)


# The HTML files are generated using doxygen, and this needs
# an installed doxygen version and may take some time, so this target
# is not made by default.
# Use `make html' or `make dist' to create the html docs

html: clean Doxyfile cards.desktop
	echo "Generating HTML documentation..."
	if test "x$(DOXYDOC)" = "x" ; then \
	  echo "Sorry - doxygen not found. Please install doxygen and run configure."; \
	fi
	-$(DOXYDOC)


