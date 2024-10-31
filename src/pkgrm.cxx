/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "pkgrm.h"

pkgrm::pkgrm ()
	: pkgdbh("pkgrm")
{
}
void pkgrm::getListOfManInstalledPackages ()
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
void pkgrm::getDirectDependencies(std::string& name)
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
void pkgrm::run()
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
	buildDatabase(false, true);

	readRulesFile();

	if (!checkPackageNameExist(m_packageName)) {
			m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_INSTALL;
			treatErrors(m_packageName);
	}

	// Remove metadata about the package removed
	removePackageFilesRefsFromDB(m_packageName);

	// Remove the files on hd
	removePackageFiles(m_packageName);

}
void pkgrm::printHelp() const
{
	std::cout << HELP_USAGE << m_utilName << " [options] <package>" << std::endl
	     << HELP_OPTIONS << std::endl
	     << "  -r, --root <path>   "
	     << _("specify alternative installation root") << std::endl
	     << "  -v, --version       "
	     << _("print version and exit") << std::endl
	     << "  -h, --help          "
	     << _("print help and exit") << std::endl;
}
