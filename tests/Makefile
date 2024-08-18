# SPDX-License-Identifier: LGPL-2.1-or-later
#  Copyright (c) 2000 - 2005 by Per Liden <per@fukt.bth.se>
#  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
#  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

include ../Makefile.inc

all: deplist deptree uname pkgdbh pkgrepo pkgfile string vector conf

deplist:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

deptree:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

uname:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

pkgdbh:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

vector:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

string:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

conf:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

pkgrepo:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o

pkgfile:
	$(CXX) -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC -c -o $@.o $@.cxx
	$(CXX) $@.o -o $@ -L"../src" -lcards -lcurl -larchive -lcrypto
	rm $@.o


clean:
	rm -f deptree
	rm -f deplist
	rm -f pkgrepo
	rm -f pkgfile
	rm -f pkgdbh
	rm -f vector
	rm -f uname
	rm -f conf
	rm -f string

# End of file
