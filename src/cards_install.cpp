/*
 * cards_install.cpp
 * 
 * Copyright 2015 - 2016 Thierry Nuttens <tnut@nutyx.org>
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
		const std::string& configFileName)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
	parseArguments();
	for( auto i : m_argParser.otherArguments() ) {
		if ( getListOfPackagesFromCollection(i).empty() &&
			(! checkBinaryExist(i) ) ) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(i);
		}
	}
	Pkgrepo::parseConfig(configFileName, m_config);
	buildDatabaseWithNameVersion();

	for( auto i : m_argParser.otherArguments() ) {
		set<string> ListOfPackage = getListOfPackagesFromCollection(i);
		if ( (!ListOfPackage.empty()) && (!checkBinaryExist(i)) ) {
			for (auto i : ListOfPackage ) {
				if (checkPackageNameExist(i))
					continue;
				m_packageName = i;
				generateDependencies();
			}
		} else {
			m_packageName = i;
			generateDependencies();
		}
	}
	getLocalePackagesList();
	for ( auto i : m_dependenciesList ) {
		m_packageArchiveName = getPackageFileName(i);
		ArchiveUtils packageArchive(m_packageArchiveName.c_str());
		string name = packageArchive.name();
		if ( checkPackageNameExist(name )) {
			m_upgrade=1;
		} else {
			m_upgrade=0;
		}
		run();
	}	
}
Cards_install::Cards_install(const CardsArgumentParser& argParser,
	const std::string& configFileName,
	const std::vector<string>& listOfPackages)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
	parseArguments();
	buildDatabaseWithDetailsInfos(false);
	for (auto i : listOfPackages) {
		string packageName = basename(const_cast<char*>(i.c_str()));
		if ( packageName == m_argParser.otherArguments()[0])
			break;
		std::set<string> listofBinaries;
		if (findFile(listofBinaries, i) != 0) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(i);
		}
		for (auto j : listofBinaries ) {
			if (j.find("cards.tar")== string::npos )
				continue;
			m_packageArchiveName = i + "/" + j;
			ArchiveUtils packageArchive(m_packageArchiveName.c_str());
			string name = packageArchive.name();
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
}
void Cards_install::getLocalePackagesList()
{
	if (m_config.locale.empty())
		return;
	vector<string> tmpList;
	for ( auto i :  m_config.locale ) {
		for ( auto j :m_dependenciesList ) {
			string packageName  = j + "." + i;
#ifndef NDEBUG
			cerr << packageName << endl;
#endif
			if (checkBinaryExist(packageName)) {
				m_packageFileName = getPackageFileName(packageName);
				if ( ! checkFileExist(m_packageFileName) )
					downloadPackageFileName(packageName);
				tmpList.push_back(packageName);
			}
		}
		if (tmpList.size() > 0 )
			for (auto i : tmpList) m_dependenciesList.push_back(i);
	}
#ifndef NDEBUG
	for (auto i : m_dependenciesList ) cerr << i << endl;
#endif
}
