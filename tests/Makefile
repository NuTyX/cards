#
#  cards
#
#  Copyright (c) 2000-2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
#  Copyright (c) 2013-2022 by NuTyX team (http://nutyx.org)
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

all: test1 test2 list1

test1:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o test1.o test1.cxx
	$(CXX) test1.o -o $@ -L"../src" -lcards -lcurl -larchive
	rm test1.o

test2:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o test2.o test2.cxx
	$(CXX) test2.o -o $@ -L"../src" -lcards -lcurl -larchive
	rm test2.o

list1:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o list1.o list1.cxx
	$(CXX) list1.o -o $@ -L"../src" -lcards -lcurl -larchive
	rm list1.o

clean:
	rm -f list1
	rm -f test1
	rm -f test2

# End of file
