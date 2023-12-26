/*
 * cards_remove.cxx
 *
 * Copyright 2013 - 2023 Thierry Nuttens <tnut@nutyx.org>
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


#include "cards_remove.h"
#include "pkgrepo.h"

Cards_remove::Cards_remove(const std::string& commandName,
	const CardsArgumentParser& argParser,
	const char *configFileName)
	: Pkgrm(commandName),m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	cards::Conf config(configFileName);

	std::set<std::string> basePackagesList;
	for (auto i : config.baseDir()) {
		if ( findDir(basePackagesList, i) != 0 ) {
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_DIRECTORY;
			treatErrors(i);
		}
	}
	if (basePackagesList.empty())
		throw std::runtime_error(_("No package found for the base System") );

	// Retrieve info about all the packages
	buildCompleteDatabase(false);

	std::set< std::pair<std::string,std::string> > listOfPackagesToRemove;
	std::pair<std::string,std::string> PackageToRemove;

	for ( auto i : m_argParser.otherArguments() ) {
		for (auto j : m_listOfPackages) {
			for (auto k : j.second.dependencies() ){
				if ( i == k.first) {
					m_actualError = cards::ERROR_ENUM_PACKAGE_IN_USE;
					std::cout << std::endl << i << _(" is a runtime dependency for ") << j.first << " !!!" << std::endl << std::endl;
					treatErrors(i);
				}
			}
			if  ((j.second.collection() == i) ||
				(j.second.group() == i)) {
				PackageToRemove.first=j.first;
				PackageToRemove.second=j.second.collection();
				listOfPackagesToRemove.insert(PackageToRemove);
			}
		}
		if ( listOfPackagesToRemove.empty()) {
			for (auto j : m_listOfPackages) {
				for (auto k : parseDelimitedSetList(j.second.sets()," ")) {
					if ( i == k ) {
						PackageToRemove.first = j.first;
						PackageToRemove.second=j.second.collection();
						listOfPackagesToRemove.insert(PackageToRemove);
					}
				}
			}
		}
		{
			// if it's an alias get the real name
			std::string a = m_listOfAlias [i];
			PackageToRemove.first = a ;
			listOfPackagesToRemove.insert(PackageToRemove);
		}
	}
	std::set< std::pair<std::string,std::string> > groupSetPackagesToRemove;
	for ( auto i :  config.groups() ) {
		if ( i == "lib" )
			continue;
		for ( auto j : listOfPackagesToRemove ) {
			PackageToRemove.first = j.first  + "." + i;
			PackageToRemove.second = j.second;
			groupSetPackagesToRemove.insert(PackageToRemove);
		}
	}
	for ( auto i : groupSetPackagesToRemove ) {
		if (checkPackageNameExist(i.first))
			listOfPackagesToRemove.insert(i);
	}
	for ( auto i : listOfPackagesToRemove ) {
		bool found = false;
		for (auto j : basePackagesList) {
			if ( i.first == j) {
				found = true;
				break;
			}
		}
		if ( found && !m_argParser.isSet(CardsArgumentParser::OPT_ALL)){
				std::cout << "The package '" << i.first
					<< "' is in the base list" << std::endl;
				std::cout << "   specify -a to remove it anyway" << std::endl;
				continue;
		}
		m_packageName = i.first;
		if (m_packageName.size() == 0 )
			continue;
		run();
		std::string name = "(" +  m_packageCollection + ") ";
		name += i.first;
		syslog(LOG_INFO,"%s",name.c_str());
	}

	// Lets get read of obsolets dependencies
	getListOfManInstalledPackages ();
	bool found;
	std::set<std::string> obsoletsDeps;
	std::set<std::string> obsoletsGroups;
	for ( auto i : m_listOfPackages  ) {
		found = false;
		for (  auto j : m_listofDependencies ) {
			if ( i.first == j ) {
				found = true;
				break;
			}
		}
		if (!found)
			obsoletsDeps.insert(i.first);
	}
	for ( auto i :  config.groups() ) {
		if ( i == "lib" )
			continue;
		for ( auto j : obsoletsDeps ) {
			obsoletsGroups.insert(j+"."+i);
		}
	}
	for ( auto i : obsoletsGroups ) {
		if (checkPackageNameExist(i))
			obsoletsDeps.insert(i);
	}
	for ( auto i : obsoletsDeps ) {
		m_packageName = i;
		run();
		std::string name = "(" +  m_packageCollection + ") ";
		name += i;
		syslog(LOG_INFO,"%s",name.c_str());
	}
}
// vim:set ts=2 :
