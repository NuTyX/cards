/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "define.h"

#include <iostream>
#include <iterator>
#include <string>

#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <syslog.h>

/**
 Return the same as 'uname -m' commands
 */
std::string getMachineType();

void assertArgument(char** argv, int argc, int index);
void rotatingCursor();
