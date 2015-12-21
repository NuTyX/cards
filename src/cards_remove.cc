// cards_remove.cc
//
//  Copyright (c) 2014-2015 by NuTyX team (http://nutyx.org)
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
#include "cards_remove.h"

using namespace std;

CardsRemove::CardsRemove(const CardsArgumentParser& argParser)
	: Pkgdbh("cards remove"),m_argParser(argParser)
{
}
void CardsRemove::run(int argc, char** argv)
{
	
	m_packageName = m_argParser.otherArguments()[0];

	// TODO give the possibility to do in alternate rootfs
	m_root="/";

	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);

	string basePackageName, basePackageNameToRemove, installPackageName, installFullPackageName;
	set<string> basePackagesList;
	Db_lock lock(m_root, true);

	getListOfPackageNames(m_root);

	if (!checkPackageNameExist(m_packageName)) {
		m_actualError = PACKAGE_NOT_INSTALL;
		treatErrors(m_packageName);
	}


	for (vector<string>::iterator bPF = config.baseDir.begin();bPF != config.baseDir.end();++bPF) {
		string prtDir = *bPF;
		// we get all the packages from the basePackageList directory
		if ( findFile(basePackagesList, prtDir) != 0 ) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
	}
	if (basePackagesList.size() == 0) {
		throw runtime_error("No package found for the base System" );
	}

	buildDatabaseWithDetailsInfos(false);
			
	if (m_argParser.isSet(CardsArgumentParser::OPT_ALL)) {
		string::size_type pos = m_packageName.find('.');
		if (pos != string::npos) {
			basePackageNameToRemove = m_packageName.substr(0,pos);
		} else {
			basePackageNameToRemove = m_packageName;
		}

		set<string> removePackagesList;

	/*
		We will delete all the subpackages found
		For all the installed packages
	*/
		for (set<string>::const_iterator iP = m_packageNamesList.begin(); iP != m_packageNamesList.end(); iP++) {
			installFullPackageName = *iP;
			string::size_type pos = installFullPackageName.find('.');
			if (pos != string::npos) {
				installPackageName = installFullPackageName.substr(0,pos);
			} else {
				installPackageName = installFullPackageName;
			}
			if ( basePackageNameToRemove == installPackageName ) {
				removePackagesList.insert(installFullPackageName);
			}
		}
		if (removePackagesList.size() > 0) {
			for (set<string>::const_iterator iR = removePackagesList.begin();iR != removePackagesList.end();iR++) {
				bool found = false;
				installPackageName = *iR;
				// Lets check if the package is not part of the base system
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
						we keep it and we can go out of the scan of the base package list for this
						install package name
					*/
					if ( basePackageName == installPackageName ) {
						found = true;
						break;
					}
				}
				if ( found == false) {
				// Remove metadata about the package removed
					removePackageFilesRefsFromDB(*iR);

				// Remove the files on hd
					removePackageFiles(*iR);
					runLdConfig();
				}
			}
		}
	} else { // just remove the package it was ask
		bool found = false;
		for(set<string>::const_iterator bP = basePackagesList.begin();bP != basePackagesList.end();bP++) {
			// Lets check if the package is not part of the base system
			string val = *bP;
			/*
				If the install Package is in the list of packages of a base system
				we keep it and we can go out of the scan of the base package list for this
				install package name
			*/
			if ( val == m_packageName ) {
				found = true;
				break;
			}
		}
		if ( found == false) {	
			removePackageFilesRefsFromDB(m_packageName);
			// Remove the files on hd
			removePackageFiles(m_packageName);
			runLdConfig();
		}
	}
}
void CardsRemove::printHelp() const
{
	cout << endl 
			<< "  cards remove can remove just the selected package " << endl
			<< " or it can delete the selected package and " << endl
			<< " it's sub packages if they are some present" << endl
			<< "usage: cards remove [options]" << endl
			<< "options:" << endl
			<< "   -A, --all       remove the  package and it's sub packages if present" << endl
			<< "   -H, --info         print this help and exit" << endl;
}
// vim:set ts=2 :
