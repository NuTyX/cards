#  SPDX-License-Identifier: LGPL-2.1-or-later
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
#  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

include ../Makefile.inc
MSGFMT = msgfmt

.SUFFIXES: .po .mo

all: po

po:
	@echo "$(MSGFMT) fr.po -o fr.mo"; \
	 $(MSGFMT) fr.po -o fr.mo

install: all
	install -v -D -m0644 fr.mo $(LOCALEDIR)/fr/LC_MESSAGES/cards.mo


distclean: clean

clean:
	rm -fv fr.mo

.PHONY: po install

