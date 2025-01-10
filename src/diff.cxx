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
		packageNameToDeal.available_size = 0;
		packageNameToDeal.installed_space = 0;
		packageNameToDeal.available_space = 0;

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

				packageNameToDeal.available_size = m_pkgrepo.getSize(i.first);

				packageNameToDeal.installed_build = i.second.build();
				packageNameToDeal.available_build = m_pkgrepo.getBinaryBuildTime(i.first);
			}
		} else {
			if(i.second.dependency()) {
				++m_packagesObsolet;
				packageNameToDeal.status = STATUS_ENUM_UPG_OBSOLET;
			} else {
				++m_packagesUnMaint;
				packageNameToDeal.status = STATUS_ENUM_UPG_UNMAINT;
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
const unsigned long diff::downloadSize()
{
	unsigned long value = 0;
	for (auto i : m_listOfPackagesToDeal) {
		if ((i.second.status == STATUS_ENUM_UPG_OUTOFDATE) ||
			(i.second.status == STATUS_ENUM_UPG_NEWBUILD)) {
				value = value + (unsigned long)i.second.available_size;
			}
	}
	return value;

}
const unsigned long diff::AddSize()
{
	unsigned long value = 0;
	for (auto i : m_listOfPackagesToDeal) {
		if ((i.second.status == STATUS_ENUM_UPG_OUTOFDATE) ||
			(i.second.status == STATUS_ENUM_UPG_NEWBUILD)) {
				value = value + (unsigned long)i.second.available_space;
			}
	}
	return value;
}
const unsigned long diff::RemoveSize()
{
	unsigned long value = 0;
	for (auto i : m_listOfPackagesToDeal)
		if ((i.second.status == STATUS_ENUM_UPG_OBSOLET) ||
			(i.second.status == STATUS_ENUM_UPG_CONFLICT) ||
			(i.second.status == STATUS_ENUM_UPG_OUTOFDATE) ||
			(i.second.status == STATUS_ENUM_UPG_NEWBUILD))
			value = value + (unsigned long)i.second.installed_space;

	return value;

}
const unsigned int diff::ratio()
{
	auto p = ((float)m_packagesOK/m_listOfPackages.size())*100;
	return 100 - (int)p;
}
void diff::showInfo(bool details)
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
		if (i.second.status == STATUS_ENUM_UPG_OUTOFDATE) {
			if (i.first.length() > width1)
				width1 = i.first.length();

			if (i.second.installed_version.length() > width2)
				width2 = i.second.installed_version.length();
		}
	}
	++width1;
	++width2;
	std::cout << std::endl
		<< _("Number of Packages:                       ")
		<< m_listOfPackages.size()
		<< std::endl;
	std::cout << _("Unchanged Packages:                       ")
		<< m_packagesOK
		<< std::endl;
	std::cout << std::endl
		<< _("Out of Date Packages (different version): ")
		<< m_packagesOutOfDate
		<< std::endl;
	if (details) {
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
	}
	if (m_packagesNewBuild > 0) {
		std::cout << std::endl
			<< _("Rebuild Packages (newer build date):      ")
			<< m_packagesNewBuild
			<< std::endl;
		if (details) {
			for (auto i : m_listOfPackagesToDeal){
				if (i.second.status != STATUS_ENUM_UPG_NEWBUILD)
					continue;
				std::cout << std::left
					<< std::setw(width1)
					<< i.first
					<< std::endl;
			}
		}
	}
	if (m_packagesObsolet > 0) {
		std::cout << std::endl
			<< _("Obsoletes Packages (will be removed):     ")
			<< m_packagesObsolet
			<< std::endl;
		if (details) {
			for (auto i : m_listOfPackagesToDeal){
				if (i.second.status != STATUS_ENUM_UPG_OBSOLET)
					continue;
				std::cout << std::left
					<< std::setw(width1)
					<< i.first
					<< std::endl;
			}
		}
	}
	if (m_packagesConflict > 0) {
		std::cout << std::endl
		<< _("Conflict Packages (will be removed):      ")
		<< m_packagesConflict
		<< std::endl;
		if (details) {
			for (auto i : m_listOfPackagesToDeal){
				if (i.second.status != STATUS_ENUM_UPG_CONFLICT)
					continue;
				std::cout << std::left
					<< std::setw(width1)
					<< i.first
					<< std::endl;
			}
		}
	}
	if (m_packagesUnMaint > 0) {
		std::cout << std::endl
			<< _("Unmaintained Packages (should be remove): ")
			<< m_packagesUnMaint
			<< std::endl;
		if (details) {
			for (auto i : m_listOfPackagesToDeal){
				if (i.second.status != STATUS_ENUM_UPG_UNMAINT)
					continue;
				std::cout << std::left
					<< std::setw(width1)
					<< i.first
					<< std::endl;
			}
		}
	}
	amountInfo();
	if (ratio() > 20) {
		std::cout << std::endl
			<< _("Percentage of obsoletes packages: ")
			<< ratio() << " %"
			<< std::endl << std::endl
			<< _("Use the command: ")
			<< BLUE
			<< "sudo cards upgrade --proceed"
			<< NORMAL
			<< std::endl
			<< _("when you're ready to upgrade your system.")
			<< std::endl << std::endl;		
	}
}
void diff::amountInfo() {
	std::cout << std::endl
		<< _("Available space on your system:           ")
		+ sizeHumanRead(availableSpace())
        + _("bytes")
		<< std::endl;
	std::cout << std::endl
		<< _("Total download size:                      ")
        + sizeHumanRead(downloadSize())
        + _("bytes")
		<< std::endl;
	std::cout << _("Total size of reinstalled packages:       ")
        + sizeHumanRead(AddSize())
        + _("bytes")
		<< std::endl;
	std::cout << _("Total size of remove packages:            ")
        + sizeHumanRead(RemoveSize())
        + _("bytes")
		<< std::endl;
	if (AddSize() > RemoveSize())
		std::cout << _("Total amount of added bytes:              ")
			+ sizeHumanRead(AddSize() - RemoveSize())
            + _("bytes")
			<< std::endl;
	if (RemoveSize() > AddSize())
		std::cout << _("Total amount of removed bytes:            ")
            + sizeHumanRead(RemoveSize() - AddSize())
            + _("bytes")
			<< std::endl;
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
const unsigned int diff::packagesUnMaint() {
	return m_packagesUnMaint;
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
const unsigned long long diff::availableSpace()
{
	struct statvfs buffer;
	statvfs("/", &buffer);
	return buffer.f_bsize * buffer.f_bavail;

}
}