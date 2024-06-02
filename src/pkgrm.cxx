//
//  pkgrm.cxx
// 
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013-2021 by NuTyX team (http://nutyx.org)
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

Pkgrm::Pkgrm ()
	: Pkgdbh("pkgrm")
{
	// Checking the rules
	readRulesFile();
}
void Pkgrm::getListOfManInstalledPackages ()
{
	// Get the list of installed packages
	getListOfPackagesNames(m_root);

	for ( auto i : m_listOfPackages) {
		if ( i.second.dependency() == false )
			m_listOfManInstalledPackages.insert(i.first);
	}
	buildSimpleDependenciesDatabase();
	for ( auto i : m_listOfManInstalledPackages ) {
		getDirectDependencies(i);
	}
}
void Pkgrm::getDirectDependencies(std::string& name)
{
	bool found = false;
	for ( auto i : m_listofDependencies ) {
		if ( name == i ) {
			found = true;
			break;
		}
	}
	if ( ! found ) {
		m_listofDependencies.insert(name);
		for ( auto i : m_listOfPackagesWithDeps ) {
			if ( i.first == name ) {
				if ( i.second.size() > 0 ) {
					for ( auto j : i.second) {
						if ( name == j )
							continue;
						getDirectDependencies(j);
					}
				}
				break;
			}
		}
	}
}
void Pkgrm::run()
{
	// Check UID
	if (getuid())
	{
		m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

	cards::lock Lock(m_root, true);

	// Get the list of installed packages
	getListOfPackagesNames(m_root);

	// Retrieve info about all the packages
	buildCompleteDatabase(false);

	if (!checkPackageNameExist(m_packageName)) {
			m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_INSTALL;
			treatErrors(m_packageName);
	}

	// Remove metadata about the package removed
	removePackageFilesRefsFromDB(m_packageName);

	// Remove the files on hd
	removePackageFiles(m_packageName);

}
void Pkgrm::printHelp() const
{
	std::cout << USAGE << m_utilName << " [options] <package>" << std::endl
	     << OPTIONS << std::endl
	     << "  -r, --root <path>   "
	     << _("specify alternative installation root") << std::endl
	     << "  -v, --version       "
	     << _("print version and exit") << std::endl
	     << "  -h, --help          "
	     << _("print help and exit") << std::endl;
}
// vim:set ts=2 :
