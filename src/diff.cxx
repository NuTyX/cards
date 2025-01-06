/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "diff.h"

namespace cards {

diff::diff(const CardsArgumentParser& argParser,
		const std::string& configFileName)
	: m_pkgrepo(configFileName)
	, m_sync(configFileName)
	, m_argParser(argParser)
	, m_config(configFileName)
	, m_packagesConflict(0)
	, m_packagesReplace(0)
	, m_packagesObsolet(0)
	, m_packagesOutOfDate(0)
	, m_packagesNewBuild(0)
	, m_packagesOK(0)
{

	if (!m_argParser.isSet(CardsArgumentParser::OPT_NO_SYNC)) {
		for (auto collection : m_config.dirUrl()) {
			if (collection.url.size() == 0 )
				continue;

			if (getuid()) {
				m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
				treatErrors("");
			}
		}
		m_sync.run();
	}

    buildDatabase(false);

	m_package packageNameToDeal;
	std::pair<std::string, time_t> packageNameBuild;
	for (auto i:m_listOfPackages) {

		if (m_pkgrepo.checkBinaryExist(i.first)) {

			packageNameBuild.first = i.first;
			packageNameBuild.second = m_pkgrepo.getBinaryBuildTime(i.first);

			if (checkPackageNameBuildDateSame(packageNameBuild)) {
				++m_packagesOK;
				packageNameToDeal.status = STATUS_ENUM_UPG_OK;
			} else {
				packageNameToDeal.installed_version = m_listOfPackages[i.first].version()
					+ "-"
					+ itos(m_listOfPackages[i.first].release());
				packageNameToDeal.available_version = m_pkgrepo.version(i.first)
					+ "-"
					+ itos(m_pkgrepo.release(i.first));
				if (packageNameToDeal.installed_version == packageNameToDeal.available_version) {
					packageNameToDeal.status = STATUS_ENUM_UPG_NEWBUILD;
					++m_packagesNewBuild;
				} else {
					packageNameToDeal.status = STATUS_ENUM_UPG_OUTOFDATE;
					++m_packagesOutOfDate;
				}
				packageNameToDeal.installed_space = i.second.space();
				packageNameToDeal.available_space = m_pkgrepo.getSpace(i.first);

				packageNameToDeal.installed_build = i.second.build();
				packageNameToDeal.available_build = m_pkgrepo.getBinaryBuildTime(i.first);
			}
		} else {
			if(i.second.dependency()) {
				++m_packagesObsolet;
				packageNameToDeal.status = STATUS_ENUM_UPG_OBSOLET;
			}
		}
		m_listOfPackagesToDeal[i.first]= packageNameToDeal;
	}
	if (m_argParser.isSet(CardsArgumentParser::OPT_SIZE))
		std::cout << size()
		<< std::endl;
	else if (m_argParser.isSet(CardsArgumentParser::OPT_CHECK)) {
		if (size() > 0)
			std::cout << "yes\n";
		else
			std::cout << "no\n";
	}
}
const unsigned int diff::ratio()
{
	auto p = ((float)m_packagesOK/m_listOfPackages.size())*100;
	return 100 - (int)p;
}
void diff::summary()
{
	std::cout << std::endl
		<< _("Number of Packages: ")
		<< m_listOfPackages.size()
		<< std::endl;
	std::cout << _("Untouched Packages: ")
		<< m_packagesOK
		<< std::endl;
	std::cout << _("Out of Date Packages (different version): ")
		<< m_packagesOutOfDate
		<< std::endl;
	std::cout << _("Rebuild Packages (newer build date): ")
		<< m_packagesNewBuild
		<< std::endl;
	std::cout << _("Obsolet Packages (will be removed): ")
		<< m_packagesObsolet
		<< std::endl;
	std::cout << _("Conflict Packages (will be removed): ")
		<< m_packagesConflict
		<< std::endl;
}
void diff::showInfo()
{
	if (size() == 0) {
		std::cout << _("Your system is up to date.")
			<< std::endl;
		return;
	}
    unsigned int width1 = 2; // Default width of "Name"
	unsigned int width2 = 4; // Default width of "Version"
	std::string name = _("Name:");
	if (name.length() > width1)
		width1 = name.length();
	std::string installed = _("Installed:");
	if (installed.length() > width2)
		width2 = installed.length();

	for (auto i : m_listOfPackagesToDeal) {
		if (i.first.length() > width1)
			width1 = i.first.length();

		if (i.second.installed_version.length() > width2)
			width2 = i.second.installed_version.length();
	}
	++width1;
	++width2;
	std::cout << std::endl
		<< _("Number of Packages: ")
		<< m_listOfPackages.size()
		<< std::endl;
	std::cout << std::endl
		<< _("Untouched Packages: ")
		<< m_packagesOK
		<< std::endl;

	std::cout << std::endl
		<< _("Out of Date Packages (different version): ")
		<< m_packagesOutOfDate
		<< std::endl;

	std::cout << std::left
		<< std::setw(width1)
		<< _("Name:")
		<< std::setw(width2)
		<< _("Installed:")
		<< _("Available:")
		<< std::endl;

	for (auto i : m_listOfPackagesToDeal) {
		if (i.second.status != STATUS_ENUM_UPG_OUTOFDATE)
			continue;
		std::cout << std::left
		<< std::setw(width1)
		<< i.first 
		<< std::setw(width2)
		<< i.second.installed_version
		<< i.second.available_version
		<< std::endl;
	}
	std::cout << std::endl
		<< _("Rebuild Packages (newer build date): ")
		<< m_packagesNewBuild
		<< std::endl;
	for (auto i : m_listOfPackagesToDeal){
		if (i.second.status != STATUS_ENUM_UPG_NEWBUILD)
			continue;
		std::cout << std::left
			<< std::setw(width1)
			<< i.first
			<< std::endl;
	}
	std::cout << std::endl
		<< _("Obsolet Packages (will be removed): ")
		<< m_packagesObsolet
		<< std::endl;
	for (auto i : m_listOfPackagesToDeal){
		if (i.second.status != STATUS_ENUM_UPG_OBSOLET)
			continue;
		std::cout << std::left
			<< std::setw(width1)
			<< i.first
			<< std::endl;
	}
	if (ratio() > 20) {
		std::cout << std::endl
			<< _("Number of obsolets packages (")
			<< ratio()
			<< " %) "
			<< _("is high !!!\n\n\
Use the command: ")
			<< BLUE
			<< "cards upgrade"
			<< WHITE
			<< " --proceed"
			<< NORMAL
			<< _(" when you're ready to upgrade your system.\n\n");		
	}
}
const unsigned int diff::packagesObsolet() {
	return m_packagesObsolet;
}
const unsigned int diff::packagesConflict() {
	return m_packagesConflict;
}
const unsigned int diff::packagesReplace() {
	return m_packagesReplace;
}
const unsigned int diff::packagesOutOfDate() {
	return m_packagesOutOfDate;
}
const unsigned int diff::packagesNewBuild() {
	return m_packagesNewBuild;
}
const unsigned int diff::packagesOK() {
	return m_packagesOK;
}
const unsigned int diff::size() {
	return m_packagesConflict
		+ m_packagesObsolet
		+ m_packagesReplace
		+ m_packagesOutOfDate
		+ m_packagesNewBuild;
}

}