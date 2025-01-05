/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "upgrade.h"

namespace cards {

upgrade::upgrade(const CardsArgumentParser& argParser,
	const std::string& configFileName)
	: m_argParser(argParser)
	, m_sync(configFileName)
	, m_pkgrepo(configFileName)
	, m_config(configFileName)
	, m_diff(argParser,configFileName)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root = m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_argParser.isSet(CardsArgumentParser::OPT_CHECK) ||
		m_argParser.isSet(CardsArgumentParser::OPT_SIZE))
		return;

	m_progress = true;
	
	buildDatabase(true);

	std::set<std::string> listOfExistPackages;
	for (auto i:m_listOfPackages) {
		if (m_pkgrepo.checkBinaryExist(i.first)) {
			listOfExistPackages.insert(i.first);
		}
	}
	if (listOfExistPackages.empty()) {
		m_actualError = cards::ERROR_ENUM_CANNOT_FIND_DEPOT;
		treatErrors("");
	}

	if (!m_config.groups().empty()) {
		std::set<std::string> tmpList;
		for (auto i : m_listOfPackages) {
			if (!i.second.group().empty())
				continue;

			for ( auto j : m_config.groups() ) {
				std::string packageName  = i.first + "." + j;
				if (m_pkgrepo.checkBinaryExist(packageName))
					tmpList.insert(packageName);
			}
		}
		if (tmpList.size() > 0) {
			for (auto i : tmpList) {
				std::pair<std::string,time_t> packageNameBuildDate;
				packageNameBuildDate.first = i;
				packageNameBuildDate.second = m_pkgrepo.getBinaryBuildTime(i);
				if (checkPackageNameBuildDateSame(packageNameBuildDate))
					continue;
				m_ListOfPackagesToUpdate.insert(packageNameBuildDate);
			}
		}
	}

	for (auto i : m_listOfPackages) {
		if (!m_pkgrepo.checkBinaryExist(i.first)) {
			// If it's not automatically installed
			// We don't need to remove it automatically
			if(i.second.dependency()) {
				m_ListOfPackagesToDelete.insert(i.first);
			}
			continue;
		}
		std::pair<std::string,time_t> packageNameBuildDate;
		m_package packageNameToDeal;
		packageNameBuildDate.first = i.first ;

		if (checkPackageNameBuildDateSame(packageNameBuildDate)) {
			continue;
		}
		m_ListOfPackagesToUpdate.insert(packageNameBuildDate);
	}
	if (m_ListOfPackagesToUpdate.size() == 0  &&
			(m_ListOfPackagesToDelete.size() == 0)) {
				std::cout << _("Your system is up to date.") << std::endl;
	} else {
		if (!m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_ONLY)) {
			if (getuid()) {
				m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
				treatErrors("");
			}
		}
		if (!m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_READY))
			upgradePackages();
	}
}
int upgrade::Isdownload()
{
	std::string packageFileName;
	for (auto i : m_ListOfPackagesToUpdate) {
		packageFileName = m_pkgrepo.fileName(i.first);
		m_pkgrepo.checkHash(packageFileName);
		if ( ! m_pkgrepo.checkHash(i.first))
			return EXIT_FAILURE;
	}

return EXIT_SUCCESS;
}
void upgrade::upgradePackages()
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_ONLY))
		return;

	for (auto i : m_ListOfPackagesToUpdate)
		m_pkgrepo.generateDependencies(i);

	for (auto i : m_pkgrepo.getDependenciesList()) {
		m_packageArchiveName = m_pkgrepo.dirName(i.first)
				+ "/"
				+ m_pkgrepo.fileName(i.first);

		m_force=true;
		if (checkPackageNameExist(i.first)) {
			m_upgrade=true;
		} else {
			m_upgrade=false;
		}
		run();
		std::string p = i.first
			+ " "
			+ m_pkgrepo.version(i.first);
		syslog(LOG_INFO,"%s upgraded",p.c_str());
	}
	for (auto i : m_ListOfPackagesToDelete) {
		cards::lock Lock(m_root,true);
		buildDatabase(true);
		removePackageFilesRefsFromDB(i);
		removePackageFiles(i);
		syslog(LOG_INFO,"%s removed",i.c_str());
	}

	m_sync.purge();
}
}
