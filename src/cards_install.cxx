/*
 * cards_install.cxx
 *
 * Copyright 2015 - 2020 Thierry Nuttens <tnut@nutyx.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "cards_install.h"


Cards_install::Cards_install(const CardsArgumentParser& argParser,
		const char *configFileName)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
	parseArguments();
	for( auto i : m_argParser.otherArguments() ) {
		if (checkFileExist(i)) {
			continue;
		}
		if ( getListOfPackagesFromCollection(i).empty() &&
			(! checkBinaryExist(i) )  && 
			getListOfPackagesFromSet(i).empty() ){
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(i);
		}
	}
	Pkgrepo::parseConfig(configFileName, m_config);
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
			ArchiveUtils packageArchive(m_packageArchiveName.c_str());
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
	for ( auto i : m_dependenciesList ) {
		m_packageArchiveName = getPackageFileName(i.first);
		ArchiveUtils packageArchive(m_packageArchiveName.c_str());
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
Cards_install::Cards_install(const CardsArgumentParser& argParser,
	const char *configFileName,
	const std::vector<std::string>& listOfPackages)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
	parseArguments();
	buildCompleteDatabase(false);
	for (auto i : listOfPackages) {
		std::string packageName = basename(const_cast<char*>(i.c_str()));
		if ( packageName == m_argParser.otherArguments()[0])
			break;
		std::set<std::string> listofBinaries;
		if (findFile(listofBinaries, i) != 0) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(i);
		}
		for (auto j : listofBinaries ) {
			if (j.find("cards.tar")== std::string::npos )
				continue;
			m_packageArchiveName = i + "/" + j;
			ArchiveUtils packageArchive(m_packageArchiveName.c_str());
			std::string name = packageArchive.name();
			if ( ! checkPackageNameExist(name )) {
				m_upgrade=0;
				m_force=0;
				run();
			}
		}
	}
}
void Cards_install::parseArguments()
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);
	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	if (m_argParser.isSet(CardsArgumentParser::OPT_FORCE))
		m_force=true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_UPDATE))
		m_upgrade=true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_DISABLE))
		m_runPrePost = false;
}
void Cards_install::getLocalePackagesList()
{
	std::string packageFileName;
	if (m_config.group.empty())
		return;
	std::set<std::string> tmpList;
	for ( auto i :  m_config.group ) {
		for ( auto j :m_dependenciesList ) {
			std::string packageName  = j.first + "." + i;
#ifndef NDEBUG
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

			m_dependenciesList.push_back(PackageTime);
		}
#ifndef NDEBUG
	for (auto i : m_dependenciesList ) std::cerr << i.first << " " << i.second << std::endl;
#endif
}
