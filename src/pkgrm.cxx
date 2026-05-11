/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "pkgrm.h"

pkgrm::pkgrm ()
	: pkgdbh("pkgrm")
{
	m_runPrePost = true;
}
void pkgrm::getListOfManInstalledPackages ()
{
	buildSimpleDependenciesDatabase();

	for ( auto i : m_listOfPackages) {
		if ( i.second.dependency() == false )
			m_listOfManInstalledPackages.insert(i.first);
	}

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
void pkgrm::preRun()
{
	std::string s = m_root + PKG_DB_DIR + m_packageName + PKG_PRE;
	if (checkFileExist(s))
	{
		progressInfo(cards::ACTION_ENUM_PKG_PREREMOVE_START);
		process preremove(m_root + SHELL,s, 0 );
		if (preremove.executeShell()) {
			exit(EXIT_FAILURE);
		}
		progressInfo(cards::ACTION_ENUM_PKG_PREREMOVE_END);
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

	// Retrieve info about all the packages
	buildDatabase(true);

	readRulesFile();

	if (!checkPackageNameExist(m_packageName)) {
			m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_INSTALL;
			treatErrors(m_packageName);
	}
	if (m_runPrePost)
		preRun();

	// Remove the files on hd
	removePackageFiles(m_packageName);

	if ( m_runPrePost)
		postRun();

	// Remove metadata about the package removed
	removePackageFilesRefsFromDB(m_packageName);
}
void pkgrm::postRun()
{
	std::string s = m_root + PKG_DB_DIR + m_packageName + PKG_POST;
	if (checkFileExist(s))
	{
		if ( ! m_runPrePost) {
			return;
		}
		progressInfo(cards::ACTION_ENUM_PKG_POSTREMOVE_START);
		process postremove(m_root + SHELL,s, 0 );
		if (postremove.executeShell()) {
			std::cerr << _("WARNING Run post-remove FAILED, continue") << std::endl;
		}
		progressInfo(cards::ACTION_ENUM_PKG_POSTREMOVE_END);
	}
}
void pkgrm::printHelp() const
{
	std::cout << HELP_USAGE << m_utilName << " [options] <package>" << std::endl
	    << HELP_OPTIONS << std::endl
		<< "  -p, --progress      "
		<< _("shows progress info")
		<< std::endl
	    << "  -r, --root <path>   "
	    << _("specify alternative installation root") << std::endl
	    << "  -v, --version       "
	    << _("print version and exit") << std::endl
	    << "  -h, --help          "
	    << _("print help and exit") << std::endl;
}
