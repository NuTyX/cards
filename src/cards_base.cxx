// cards_base.cxx
//
//  Copyright (c) 2014-2016 by NuTyX team (http://nutyx.org)
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

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <cstring>
#include <cstdlib>

#include "file_utils.h"
#include "file_download.h"
#include "pkgrepo.h"
#include "cards_base.h"

using namespace std;

Cards_base::Cards_base(const CardsArgumentParser& argParser)
	: Pkgdbh("cards base"), m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);
	else
		m_root="/";

	getListOfPackageNames(m_root); // Get the list of installed packages
}
void Cards_base::run(int argc, char** argv)
{
	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);

	if (m_argParser.isSet(CardsArgumentParser::OPT_REMOVE)) {
		if (getuid()) {
			m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
			treatErrors("");
		}
	}

	string basePackageName,installPackageName,installFullPackageName;
	set<string> removePackagesList,basePackagesList;

	// For all the base packages list Directories
	for ( auto i : config.baseDir) {
		// we get all the packages from the basePackageList directory
		if ( findFile(basePackagesList, i) != 0 ) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(i);
		}
	}

	if (basePackagesList.size() == 0) {
		throw runtime_error("No package found for the base System" );
	}

	// For all the installed packages
	for (set<string>::const_iterator iP = m_packageNamesList.begin(); iP != m_packageNamesList.end(); iP++) {
		installFullPackageName = *iP;
		// We need to compare the base part if it not one
		string::size_type pos = installFullPackageName.find('.');
		if (pos != string::npos) {
			installPackageName = installFullPackageName.substr(0,pos);
		} else {
			installPackageName = installFullPackageName;
		}
		bool found = false;
		for(set<string>::const_iterator bP = basePackagesList.begin();bP != basePackagesList.end();bP++) {
			string val = *bP;
			string::size_type pos = val.find('@');
			if (pos != string::npos) {
				basePackageName = val.substr(0,pos);
			} else {
				basePackageName = val;
			}
/*
			If the install Package is in the list of packages of a base system
			So we keep it and we can go out of the scan of the base package list for this
			install package name
*/
			if ( basePackageName == installPackageName ) {
				found = true;
				break;
			}
		}
		if ( found == false) {
			removePackagesList.insert(installFullPackageName);
		}
	}
	if (removePackagesList.size() > 0) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_REMOVE)) {

			getListOfPackageNames(m_root);

			// Retrieve info about all the packages
			buildCompleteDatabase(false);
			// Lock the Database, any interruption forbidden
			Db_lock lock(m_root, true);

			for (auto i : removePackagesList ) {
				// Remove metadata about the package removed
				removePackageFilesRefsFromDB(i);

				// Remove the files on hd
				removePackageFiles(i);
			}
			runLdConfig();
		} else {
			for (auto i : removePackagesList) cout << i << endl;
		}
	}
}
void Cards_base::printHelp() const
{
	cout << endl 
			<< "  You should have one or severall valid directories " << endl
			<< " otherwise you will not be able to return to a " << endl
			<< " base system. In thoses directories should be the" << endl
			<< " packages you want to keep in the same format as" << endl
			<< " the ports" << endl << endl
			<< " It's made on purpose that a list of packages which" << endl
			<< " should be remove comes out by default to avoid any mistakes" << endl << endl
			<< "usage: cards base [options]" << endl
			<< "options:" << endl
			<< "   -R, --remove       remove all the packages found, use with care, check first the list without passing any options" << endl
			<< "   -H, --info         print this help and exit" << endl;
}
// vim:set ts=2 :
