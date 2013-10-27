//
//  cards
// 
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

#include "pkgrm.h"
#include <unistd.h>
#include <stdio.h>

void pkgrm::run(int argc, char** argv)
{
	//
	// Check command line options
	//
	string o_package;
	string o_root;

	for (int i = 1; i < argc; i++) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option[0] == '-' || !o_package.empty()) {
			actualError = INVALID_OPTION;
			treatErrors(option);
		} else {
			o_package = option;
		}
	}

	if (o_package.empty())
	{
		actualError = OPTION_MISSING;
		treatErrors("o_package");
	}
	
	// Check UID
	if (getuid())
	{
		actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

	// Remove package
	{
		db_lock lock(o_root, true);

		// Get the list of installed packages
		getListOfPackages(o_root);

		// Retrieve info about all the packages
		getInstalledPackages(false);

		if (!checkPackageNameExist(o_package))
		{
			actualError = PACKAGE_NOT_INSTALL;
			treatErrors(o_package);
		}

		// Remove metadata about the package removed 
		removePackageFilesRefsFromDB(o_package);

		// Remove the files on hd
		removePackageFiles(o_package);
		runLdConfig();
	}
}
void pkgrm::printHelp() const
{
	cout << "usage: " << utilName << " [options] <package>" << endl
	     << "options:" << endl
	     << "  -r, --root <path>   specify alternative installation root" << endl
	     << "  -v, --version       print version and exit" << endl
	     << "  -h, --help          print help and exit" << endl;
}
// vim:set ts=2 :
