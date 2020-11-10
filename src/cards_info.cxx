/*
 * cards_info.cxx
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

#include "cards_info.h"

using namespace std;

Cards_info::Cards_info(const CardsArgumentParser& argParser, const std::string& configFileName)
	: Pkginfo("cards info"),Pkgrepo(configFileName), m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
	if ((m_argParser.command() == CardsArgumentParser::CMD_INFO) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			getBinaryPackageInfo(m_argParser.otherArguments()[0]);
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_PORTS)) {
			getPortInfo(m_argParser.otherArguments()[0]);
		} else {
			m_details_mode=1;
			m_arg=m_argParser.otherArguments()[0];
			run();
		}
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_LIST) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			set<string> sortedPackagesList;
			set<Pkg*> binaryList = getBinaryPackageSet();
			for ( auto i : binaryList) {
				string s;
				s =  "(" + i->getCollection() + ") ";
				s += i->getName() + " ";
				s +=  i->getVersion() + " ";
				s +=  i->getDescription();
				sortedPackagesList.insert(s);
			}
			for ( auto i : sortedPackagesList) cout << i << endl;

		} else if (m_argParser.isSet(CardsArgumentParser::OPT_PORTS)) {
			getPortsList();
		} else {
			m_installed_mode=1;
			if (m_argParser.isSet(CardsArgumentParser::OPT_FULL))
				m_fulllist_mode=true;
			run();
		}
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_QUERY) ) {
		m_owner_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_FILES) ) {
		m_list_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();		
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_SEARCH) ) {
		std::vector<RepoInfo> List;
		List = getRepoInfo();
		buildSimpleDatabase();
		for (auto i : List) {
			for (auto j : i.basePackageList) {
				string::size_type pos;
				pos = convertToLowerCase(j.categories).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = i.collection.find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = j.set.find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = j.basePackageName.find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = convertToLowerCase(j.description).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = convertToLowerCase(j.URL).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = convertToLowerCase(j.packager).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = convertToLowerCase(j.version).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if ( pos != std::string::npos ) {
					cout << "(" << i.collection << ") ";
					if ( checkPackageNameExist(j.basePackageName) ) {
						cout << GREEN;
					}
					cout << j.basePackageName
						<< NORMAL
						<< " " << j.version
						<< " " << j.description
						<< endl;
					continue;
				}
				bool found = false;
				set<string> groupList;
				groupList = parseDelimitedSetList(j.group,' ');
				for ( auto k : groupList ) {
					if ( k == j.basePackageName)
						continue;
					if ( convertToLowerCase(m_argParser.otherArguments()[0]) == k ) {
					string name = j.basePackageName
						+ "."
						+ k;
					cout << "(" << i.collection << ") ";
					if ( checkPackageNameExist(name )) {
						cout << GREEN;
					}
					cout << name
						<< NORMAL
						<< " " << j.version
						<< " " << j.description
						<< endl;
					name="";
					found = true;
					continue;
					}
				}
				if (found)
					continue;
				set<string> aliasList;
				aliasList = parseDelimitedSetList(j.alias,' ');
				for ( auto k : aliasList ) {
					if ( k == j.basePackageName)
						continue;
					if ( convertToLowerCase(m_argParser.otherArguments()[0]) == k ) {
						string name = j.basePackageName;
						cout << "(" << i.collection << ") ";
						if ( checkPackageNameExist(name )) {
							cout << GREEN;
						}
					cout << name
						<< NORMAL
						<< " " << j.version
						<< " " << j.description
						<< endl;
					name="";
					found = true;
					continue;
					}
				}
			}
		}

	}
}
// vim:set ts=2 :
