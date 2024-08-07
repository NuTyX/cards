/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "cards_upgrade.h"

cards_upgrade::cards_upgrade(const CardsArgumentParser& argParser,
	const char *configFileName)
	: pkginst("cards upgrade",configFileName), m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
	m_pkgSync.setConfigFile(configFileName);
	if ( ! m_argParser.isSet(CardsArgumentParser::OPT_NO_SYNC))
		m_pkgSync.run();
	parseCollectionPkgRepoFile();
	buildSimpleDatabase();
	std::set<std::string> listOfExistPackages;
	for (auto i:m_listOfPackages) {
		if (checkBinaryExist(i.first)) {
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
	if (!m_config.groups().empty()) {
		std::set<std::string> tmpList;
		for (auto i: m_listOfPackages) {
			for ( auto j :  m_config.groups() ) {
					std::string packageName  = getBasePackageName(i.first) + "." + j;
					if ( i.first == packageName )
						continue;
					if (checkBinaryExist(packageName)) {
						tmpList.insert(packageName);
					}
			}
		}
		if ( tmpList.size() > 0) {
			for ( auto i : tmpList) {
				std::pair<std::string,time_t> packageNameBuildDate;
				packageNameBuildDate.first = i;
				packageNameBuildDate.second = getBinaryBuildTime(i);
				if (checkPackageNameBuildDateSame(packageNameBuildDate))
					continue;
				m_ListOfPackages.insert(packageNameBuildDate);
			}
		}
	}
	for (auto i : m_listOfPackages) {
		if (!checkBinaryExist(i.first)) {
			m_ListOfPackagesToDelete.insert(i.first);
			continue;
		}
		std::pair<std::string,time_t> packageNameBuildDate;
		packageNameBuildDate.first = i.first ;
		packageNameBuildDate.second = getBinaryBuildTime(i.first);
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
					upgrade();
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
void cards_upgrade::size()
{
	std::cout << m_ListOfPackages.size() + m_ListOfPackagesToDelete.size() << std::endl;
}
void cards_upgrade::Isuptodate()
{
	if ( ( m_ListOfPackages.size() == 0 ) && ( m_ListOfPackagesToDelete.size() == 0 ) )
		std::cout << "no" << std::endl;
	else
		std::cout << "yes" << std::endl;
}
int cards_upgrade::Isdownload()
{
	std::string packageNameSignature, packageName, packageFileName;
	for (auto i : m_ListOfPackages) {
		packageFileName = getPackageFileName(i.first);
		packageNameSignature = getPackageFileNameSignature(packageName);
		if ( ! checkFileSignature(packageFileName, packageNameSignature))
			return EXIT_FAILURE;
	}

return EXIT_SUCCESS;
}
void cards_upgrade::dry()
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
void cards_upgrade::upgrade()
{
	for (auto i : m_ListOfPackages) generateDependencies(i);

	if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
		dry();
	else if (! m_argParser.isSet(CardsArgumentParser::OPT_DOWNLOAD_ONLY)) {
		for (auto i : m_dependenciesList) {
			m_packageArchiveName=getPackageFileName(i.first);
			m_force=true;
			if (checkPackageNameExist(i.first)) {
				m_upgrade=true;
			} else {
				m_upgrade=false;
			}
		run();
		std::string p = i.first + " " + getBasePackageVersion(getBasePackageName(i.first)) \
    + "-" + itos(getBasePackageRelease(getBasePackageName(i.first)));
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
		m_pkgSync.purge();
		summary();
	}
}
void cards_upgrade::summary()
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
