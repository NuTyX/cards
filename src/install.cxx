/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "install.h"

namespace cards {

install::install(const CardsArgumentParser& argParser,
		const char *configFileName)
	: m_pkgrepo(configFileName)
	, m_argParser(argParser)
	, m_configFileName(configFileName)
{
	parseArguments();
	for( auto i : m_argParser.otherArguments() ) {
		if (checkFileExist(i)) {
			continue;
		}
		if ( m_pkgrepo.getListOfPackagesFromCollection(i).empty() &&
			(! m_pkgrepo.checkBinaryExist(i) )  &&
			m_pkgrepo.getListOfPackagesFromSet(i).empty() ){
				m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
				treatErrors(i);
		}
	}

	buildSimpleDatabase();
	for( auto i : m_argParser.otherArguments() ) {
		std::set<std::string> ListOfPackage = m_pkgrepo.getListOfPackagesFromSet(i);
		if (ListOfPackage.empty() )
			ListOfPackage = m_pkgrepo.getListOfPackagesFromCollection(i);
		if ( !ListOfPackage.empty() )  {
			/*
			* It's a collection or a set
			*/
			for (auto i : ListOfPackage ) {
				if (checkPackageNameExist(i))
					continue;

				m_pkgrepo.generateDependencies(i);
			}
		} else if (checkRegularFile(i)) {
			/*
			 * It's a regular archive file
			 */

			archive packageArchive(i);
			if (checkPackageNameExist(packageArchive.name())) {
				m_upgrade=1;
			} else {
				m_upgrade=0;
			}

			m_packageArchiveName = i;
			run();

			std::string p = "(" + packageArchive.collection()+") " + packageArchive.name();
			syslog(LOG_INFO,"%s",p.c_str());
		} else {
			/*
			 * It's a normal package
			 */
			m_pkgrepo.generateDependencies(i);
		}
	}

	getLocalePackagesList();

	for ( auto i : m_pkgrepo.getDependenciesList() ) {
		m_packageArchiveName = m_pkgrepo.dirName(i.first) + "/" + m_pkgrepo.fileName(i.first);
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
		for (auto j : m_pkgrepo.getDependenciesList()) {
			std::string name = j.first + "." + i;
			if (m_pkgrepo.checkBinaryExist(name)) {
				std::string packageName  = m_pkgrepo.dirName(name) + "/" + m_pkgrepo.fileName(name);
				packageFileName = m_pkgrepo.fileName(name);
				if ( ! checkFileExist(packageName) )
					m_pkgrepo.downloadPackageFileName(name);
				tmpList.insert(name);
			}
		}
	}
	if (tmpList.size() > 0 ) {
		for (auto i : tmpList) {
			std::pair<std::string,time_t> PackageTime;
			PackageTime.first=i;
			PackageTime.second=0;

			m_pkgrepo.generateDependencies(i);
			m_pkgrepo.addDependenciesList(PackageTime);
		}
	}
}
}
