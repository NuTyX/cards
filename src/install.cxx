/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "install.h"

namespace cards {

install::install(const CardsArgumentParser& argParser,
		const char *configFileName)
	: pkgrepo(configFileName)
	, m_argParser(argParser)
	, m_configFileName(configFileName)
{
	parseArguments();
	for( auto i : m_argParser.otherArguments() ) {
		if (checkFileExist(i)) {
			continue;
		}
		if ( getListOfPackagesFromCollection(i).empty() &&
			(! checkBinaryExist(i) )  && 
			getListOfPackagesFromSet(i).empty() ){
				m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
				treatErrors(i);
		}
	}
	cards::conf config(configFileName);

	buildSimpleDatabase();
	for( auto i : m_argParser.otherArguments() ) {
		std::set<std::string> ListOfPackage = getListOfPackagesFromSet(i);
		if (ListOfPackage.empty() )
			ListOfPackage = getListOfPackagesFromCollection(i);
		if ( !ListOfPackage.empty() )  {
			/*
			* It's a collection or a set
			*/
			for (auto i : ListOfPackage ) {
				if (checkPackageNameExist(i))
					continue;
				m_packageName = i;
				generateDependencies();
			}
		} else if (checkRegularFile(i)) {
			/*
			 * It's a regular archive file
			 */
			m_packageArchiveName = i;
			archive packageArchive(m_packageArchiveName.c_str());
			std::string name = packageArchive.name();
			if (checkPackageNameExist(name )) {
				m_upgrade=1;
			} else {
				m_upgrade=0;
			}
			name = "(" + packageArchive.collection()+") " + name;
			run();
			syslog(LOG_INFO,"%s",name.c_str());
		} else {
			/*
			 * It's a normal package
			 */
			m_packageName = i;
			generateDependencies();
		}
	}
	getLocalePackagesList();
	for ( auto i : getDependenciesList() ) {
		m_packageArchiveName = getPackageFileName(i.first);
		archive packageArchive(m_packageArchiveName.c_str());
		std::string name = packageArchive.name();
		if ( checkPackageNameExist(name )) {
			m_upgrade=1;
		} else {
			m_upgrade=0;
		}
		if (i.second > 0)
			setDependency();
		name = "(" + packageArchive.collection()+") " + name;
		run();
		if (!m_argParser.isSet(CardsArgumentParser::OPT_NOLOGENTRY))
			syslog(LOG_INFO,"%s",name.c_str());

	}
}
void install::parseArguments()
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);
	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
	if (getuid()) {
		m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	if (m_argParser.isSet(CardsArgumentParser::OPT_FORCE))
		m_force=true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_UPDATE))
		m_upgrade=true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_DISABLE))
		m_runPrePost = false;
}
void install::getLocalePackagesList()
{
	std::string packageFileName;
	cards::conf config(m_configFileName);

	if (config.groups().empty())
		return;
	std::set<std::string> tmpList;
	for (auto i : config.groups()) {
		for (auto j : getDependenciesList()) {
			std::string packageName  = j.first + "." + i;
#ifdef DEBUG
			std::cerr << packageName << std::endl;
#endif
			if (checkBinaryExist(packageName)) {
				packageFileName = getPackageFileName(packageName);
				if ( ! checkFileExist(packageFileName) )
					downloadPackageFileName(packageName);
				tmpList.insert(packageName);
			}
		}
	}
	if (tmpList.size() > 0 )
		for (auto i : tmpList) {
			std::pair<std::string,time_t> PackageTime;
			PackageTime.first=i;
			PackageTime.second=0;
			m_packageName = i;
			generateDependencies();
			addDependenciesList(PackageTime);
		}
#ifdef DEBUG
	for (auto i : m_dependenciesList )
		std::cerr << i.first << " " << i.second << std::endl;
#endif
}
}
