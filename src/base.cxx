/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "base.h"

namespace cards {

base::base(const CardsArgumentParser& argParser)
        : pkgdbh("cards base"),
        m_argParser(argParser)
{
    if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
	m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);
    else
	m_root="/";

    getListOfPackagesNames(m_root); // Get the list of installed packages
}
void base::run(int argc, char** argv)
{
	cards::conf config;

	if (m_argParser.isSet(CardsArgumentParser::OPT_REMOVE)) {
		if (getuid()) {
			m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
			treatErrors("");
		}
	}

	std::string basePackageName,installPackageName,installFullPackageName;
	std::set<std::string> removePackagesList,basePackagesList;

	// For all the base packages list Directories
	for ( auto i : config.baseDir()) {
		// we get all the packages from the packagesList directory
		if ( findDir(basePackagesList, i) != 0 ) {
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_DIRECTORY;
			treatErrors(i);
		}
	}

	if (basePackagesList.size() == 0) {
		throw std::runtime_error("No package found for the base System" );
	}
	std::set<std::string> installedList = getListOfPackagesNames();
	// For all the installed packages
	for ( auto installFullPackageName : installedList) {
		// We need to compare the base part if it not one
		std::string::size_type pos = installFullPackageName.find('.');
		if (pos != std::string::npos) {
			installPackageName = installFullPackageName.substr(0,pos);
		} else {
			installPackageName = installFullPackageName;
		}
		bool found = false;
		for ( auto val : basePackagesList) {
			std::string::size_type pos = val.find('@');
			if (pos != std::string::npos) {
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

			getListOfPackagesNames(m_root);

			// Retrieve info about all the packages
			buildCompleteDatabase(false);
			// Lock the Database, any interruption forbidden
			cards::lock Lock(m_root, true);

			for (auto i : removePackagesList ) {
				// Remove metadata about the package removed
				removePackageFilesRefsFromDB(i);

				// Remove the files on hd
				removePackageFiles(i);
			}
		} else {
			for (auto i : removePackagesList) std::cout << " SIM: " <<  i << std::endl;
		}
	}
}
void base::printHelp() const
{
	std::cout << std::endl
			<< "  You should have one or severall valid directories " << std::endl
			<< " otherwise you will not be able to return to a " << std::endl
			<< " base system. In thoses directories should be the" << std::endl
			<< " packages you want to keep in the same format as" << std::endl
			<< " the ports" << std::endl << std::endl
			<< " It's made on purpose that a list of packages which" << std::endl
			<< " should be remove comes out by default to avoid any mistakes" << std::endl << std::endl
			<< "usage: cards base [options]" << std::endl
			<< "options:" << std::endl
			<< "   -R, --remove       remove all the packages found, use with care, check first the list without passing any options" << std::endl
			<< "   -H, --info         print this help and exit" << std::endl;
}
}
