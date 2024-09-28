/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "upgrade.h"

namespace cards {

upgrade::upgrade(const CardsArgumentParser& argParser,
	const char *configFileName)
	: m_pkgrepo(configFileName)
	, m_argParser(argParser)
	, m_sync(configFileName)
{
	if ( ! m_argParser.isSet(CardsArgumentParser::OPT_NO_SYNC))
		m_sync.run();
	
	buildSimpleDatabase();
	std::set<std::string> listOfExistPackages;
	for (auto i:m_pkgrepo.getListOfPackages()) {
		if (m_pkgrepo.checkBinaryExist(i.first)) {
			listOfExistPackages.insert(i.first);
		}
	}
	if ( listOfExistPackages.empty() ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_FIND_DEPOT;
		treatErrors("");
	}
#ifdef DEBUG
	for ( auto j :  m_config.groups() ) {
		std::cout << j << std::endl;
	}
#endif
	if (!m_pkgrepo.getListofGroups().empty()) {
		std::set<std::string> tmpList;
		for (auto i: m_pkgrepo.getListOfPackages()) {
			for ( auto j :  m_pkgrepo.getListofGroups() ) {
					std::string packageName  = i.first + "." + j;
					if ( i.first == packageName )
						continue;
					if (m_pkgrepo.checkBinaryExist(packageName)) {
						tmpList.insert(packageName);
					}
			}
		}
		if ( tmpList.size() > 0) {
			for ( auto i : tmpList) {
				std::pair<std::string,time_t> packageNameBuildDate;
				packageNameBuildDate.first = i;
				packageNameBuildDate.second = m_pkgrepo.getBinaryBuildTime(i);
				if (checkPackageNameBuildDateSame(packageNameBuildDate))
					continue;
				m_ListOfPackages.insert(packageNameBuildDate);
			}
		}
	}
	for (auto i : m_pkgrepo.getListOfPackages()) {
		if (!m_pkgrepo.checkBinaryExist(i.first)) {
			m_ListOfPackagesToDelete.insert(i.first);
			continue;
		}
		std::pair<std::string,time_t> packageNameBuildDate;
		packageNameBuildDate.first = i.first ;
		packageNameBuildDate.second = m_pkgrepo.getBinaryBuildTime(i.first);
		if ( checkPackageNameBuildDateSame(packageNameBuildDate)) {
			continue;
		}
		m_ListOfPackages.insert(packageNameBuildDate);
	}
	if ( m_argParser.getCmdValue() == ArgParser::CMD_UPGRADE) {
		if ( m_argParser.isSet(CardsArgumentParser::OPT_CHECK))
			Isuptodate();
		if ( m_argParser.isSet(CardsArgumentParser::OPT_SIZE))
			size();
		if ( (! m_argParser.isSet(CardsArgumentParser::OPT_SIZE)) &&
				(! m_argParser.isSet(CardsArgumentParser::OPT_CHECK)) ) {
			if ( m_ListOfPackages.size() == 0  && ( m_ListOfPackagesToDelete.size() == 0 ) ) {
				std::cout << _("Your system is up to date.") << std::endl;
			} else {
				if (! m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_ONLY)) {
					if (getuid()) {
						m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
						treatErrors("");
					}
				}
				if ( ! m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_READY))
					run();
			}
		}
	}
	if ( m_argParser.getCmdValue() == ArgParser::CMD_DIFF) {
		if ( ( m_ListOfPackages.size() == 0 ) && ( m_ListOfPackagesToDelete.size() == 0 ) ) {
			std::cout << _("Your system is up to date.") << std::endl;
		} else {
			dry();
		}
	}
}
void upgrade::size()
{
	std::cout << m_ListOfPackages.size() + m_ListOfPackagesToDelete.size() << std::endl;
}
void upgrade::Isuptodate()
{
	if ( ( m_ListOfPackages.size() == 0 ) && ( m_ListOfPackagesToDelete.size() == 0 ) )
		std::cout << "no" << std::endl;
	else
		std::cout << "yes" << std::endl;
}
int upgrade::Isdownload()
{
	std::string packageNameSignature, packageName, packageFileName;
	for (auto i : m_ListOfPackages) {
		packageFileName = m_pkgrepo.fileName(i.first);
		packageNameSignature = m_pkgrepo.fileSignature(packageName);
		if ( ! checkFileHash(packageFileName, packageNameSignature))
			return EXIT_FAILURE;
	}

return EXIT_SUCCESS;
}
void upgrade::dry()
{
	if (m_ListOfPackages.size() > 1 )
				std::cout << _("Packages")
					<< ": ";
	if (m_ListOfPackages.size() == 1 )
				std::cout << _("Package")
					<< " : ";
	for (auto i : m_ListOfPackages )
		std::cout << "'"
			<< i.first
			<< "' ";
	if (m_ListOfPackages.size() > 0 )
				std::cout << _("will be replaced when you upgrade your NuTyX.")
					<< std::endl;

	if (m_ListOfPackagesToDelete.size() > 1 )
				std::cout << _("Packages") << ": ";
	if (m_ListOfPackagesToDelete.size() == 1 )
				std::cout << _("Package") << " : ";
	for (auto i: m_ListOfPackagesToDelete)
			std::cout << "'" << i << "' ";
	if (m_ListOfPackagesToDelete.size() > 0 )
				std::cout << _("will be removed when you upgrade your NuTyX.")
					<< std::endl;

}
void upgrade::run()
{
	for (auto i : m_ListOfPackages)
		m_pkgrepo.generateDependencies(i);

	if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
		dry();
	else if (! m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_ONLY)) {
		for (auto i : m_pkgrepo.getDependenciesList()) {
			m_packageArchiveName = m_pkgrepo.fileName(i.first);
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
			getListOfPackagesNames(m_root);
			buildCompleteDatabase(false);
			removePackageFilesRefsFromDB(i);
			removePackageFiles(i);
			syslog(LOG_INFO,"%s removed",i.c_str());
		}
		m_sync.purge();
		summary();
	}
}
void upgrade::summary()
{
	if (m_ListOfPackages.size() > 1 ) {
		std::cout << std::endl;
		std::cout << _("Packages")
			<< ": ";
		for (auto i : m_ListOfPackages )
			std::cout << "'"
				<< i.first
				<< "' ";
		std::cout << _("have been replaced on your NuTyX.")
			<< std::endl;
	}
	if (m_ListOfPackagesToDelete.size() > 1 ) {
		std::cout << _("Packages")
			<< ": ";
		for (auto i: m_ListOfPackagesToDelete)
			std::cout << "'"
				<< i
				<< "' ";
		std::cout << _("have been removed from your NuTyX.") << std::endl;
	}
	if (m_ListOfPackages.size() > 1 || m_ListOfPackagesToDelete.size() > 1 )
		std::cout << std::endl;
}
}
