/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#if (__GNUC__ < 4)
#error This program requires GCC 4.x to compile.
#endif

#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>

#include <libgen.h>

#include "pkgdbh.h"
#include "pkgadd.h"
#include "pkgrm.h"
#include "pkginfo.h"

pkgdbh* select_utility(const std::string& name)
{
	if (name == "pkgadd")
		return new pkgadd;
	else if (name == "pkgrm")
		return new pkgrm;
	else if (name == "pkginfo")
		return new pkginfo;
	else	
		throw std::runtime_error("command not supported by cards");
}

int main(int argc, char** argv)
{
	std::string name = basename(argv[0]);
	setlocale(LC_ALL,"");
	bindtextdomain(GETTEXT_PACKAGE,PACKAGE_LOCALE_DIR);
	textdomain(GETTEXT_PACKAGE);

	try {
		std::unique_ptr<pkgdbh> util(select_utility(name));

		// Handle common options
		for (int i = 1; i < argc; i++) {
			std::string option(argv[i]);
			if (option == "-v" || option == "--version") {
				util->print_version();
				return EXIT_SUCCESS;
			} else if (option == "-h" || option == "--help") {
				util->printHelp();
				return EXIT_SUCCESS;
			}
		}
		util->run(argc, argv);

	} catch (std::runtime_error& e) {
		std::cerr << name <<  " " << VERSION << ": " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
