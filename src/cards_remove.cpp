/*
 * cards_remove.cpp
 * 
 * Copyright 2013-2016 Thierry Nuttens <tnut@nutyx.org>
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

using namespace std;

Cards_remove::Cards_remove(const string& commandName,
	const CardsArgumentParser& argParser,
	const string& configFileName)
	: Pkgrm(commandName),m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	if (!m_argParser.isSet(CardsArgumentParser::OPT_ALL)){
		set<string> basePackagesList;
		Config config;
		Pkgrepo::parseConfig(configFileName, config);
		for (auto i : config.baseDir) {
			if ( findFile(basePackagesList, i) != 0 ) {
				m_actualError = CANNOT_READ_DIRECTORY;
				treatErrors(i);
			}
		}	
		if (basePackagesList.empty())
			throw runtime_error("No package found for the base System" );

		// Retrieve info about all the packages
		buildDatabaseWithNameVersion();

		set<string> listOfPackagesToRemove;

		for ( auto i : m_argParser.otherArguments() ) {
			bool found = false;
			for (auto j : m_listOfInstPackages) {
				if  (( j.second.collection == i) ||
				( j.second.group == i) ) {
				listOfPackagesToRemove.insert(j.first);
				}
			}
			if ( listOfPackagesToRemove.empty()) {
				for (auto j : m_listOfInstPackages) {
					if  (( j.second.base == i) ) {
						listOfPackagesToRemove.insert(j.first);
					}
				}
			}

			if ( listOfPackagesToRemove.empty()) {
				listOfPackagesToRemove.insert(i);
			}
		}
		for ( auto i : listOfPackagesToRemove ) {
			bool found = false;
			for (auto j : basePackagesList) {
				if ( i == j) {
					found = true;
					break;
				}
			}
			if (found){	
				cout << "The package '" << i 
					<< "' is in the base list" << endl;
				cout << "   specify -a to remove it anyway" << endl;  
				continue;
			}
			
			m_packageName = i;
			run();
		}
	} else {
		for ( auto i : m_argParser.otherArguments() ) {
			m_packageName = i;
			run();
		}
	}
	finish();
}
// vim:set ts=2 :
