/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "system_utils.h"

std::string getMachineType ()
{
	std::string machineType = "";
	struct utsname buf;
	int ret;
	ret =  uname(&buf);
	if (!ret) {
		machineType = buf.machine;
	}
	return machineType;
}
void rotatingCursor() {
  static int pos=0;
  char cursor[4]={'/','-','\\','|'};
  fprintf(stderr,"\r [ %c ] ", cursor[pos]);
  fflush(stderr);
  pos = (pos+1) % 4;
}
void assertArgument(char** argv, int argc, int index)
{
	if (argc - 1 < index + 1)
		throw std::runtime_error("option " + std::string(argv[index]) + " requires an argument");
}
