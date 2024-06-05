/*
 * cards_info.cxx
 * 
 * Copyright 2015 - 2023 Thierry Nuttens <tnut@nutyx.org>
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

Cards_info::Cards_info(const CardsArgumentParser& argParser, const std::string& configFileName)
	: pkginfo("cards info"),pkgrepo(configFileName), m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
	if ((m_argParser.getCmdValue() == ArgParser::CMD_INFO) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			getBinaryPackageInfo(m_argParser.otherArguments()[0]);
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_PORTS)) {
			getPortInfo(m_argParser.otherArguments()[0]);
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_SETS)) {
			std::set<std::string> sortedPackagesList = getListOfPackagesFromSet(m_argParser.otherArguments()[0]);;
			if (sortedPackagesList.size() == 0)
				sortedPackagesList = getListOfPackagesFromCollection(m_argParser.otherArguments()[0]);;
			for ( auto i : sortedPackagesList )
				std::cout << "("
				<< m_argParser.otherArguments()[0]
				<< ") "
				<< i << std::endl;
		} else {
			m_details_mode=1;
			m_arg=m_argParser.otherArguments()[0];
			run();
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_LIST) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			std::set<std::string> sortedPackagesList;
			std::set<cards::cache*> binaryList = getBinaryPackageSet();
			for ( auto i : binaryList) {
				std::string s, _s;
				if ( i->sets().size()  > 0 ) {
					for (auto s: i->sets())
							_s += s + " ";
					s =  "(" + _s + ") ";
				} else {
					s = "(" + i->collection() + ") ";
				}
				s += i->name() + " ";
				s +=  i->version() + " ";
				s +=  i->description();
				sortedPackagesList.insert(s);
			}
			for ( auto i : sortedPackagesList) std::cout << i << std::endl;

		} else if (m_argParser.isSet(CardsArgumentParser::OPT_PORTS)) {
			getPortsList();
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_SETS)) {
			std::set<std::string> sortedSetList;
			std::set<cards::cache*> binaryList = getBinaryPackageSet();
			for ( auto i : binaryList )
				if ( i->sets().size() > 0 ) {
					for ( auto s:i->sets() )
						sortedSetList.insert(s);
				}
			for ( auto i : sortedSetList )
				std::cout << i << std::endl;
		} else {
			m_installed_mode=1;
			if (m_argParser.isSet(CardsArgumentParser::OPT_FULL))
				m_fulllist_mode=true;
			run();
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_QUERY) ) {
		m_owner_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_FILES) ) {
		m_list_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();		
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_SEARCH) ) {
		std::vector<RepoInfo> List;
		List = getRepoInfo();
		buildSimpleDatabase();
		for (auto i : List) {
			for (auto j : i.basePackageList) {
				std::string::size_type pos;
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
					pos = convertToLowerCase(j.url).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = convertToLowerCase(j.packager).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if  ( pos == std::string::npos )
					pos = convertToLowerCase(j.version).find(convertToLowerCase(m_argParser.otherArguments()[0]));
				if ( pos != std::string::npos ) {
					std::cout << "(" << i.collection << ") ";
					if ( checkPackageNameExist(j.basePackageName) ) {
						std::cout << GREEN;
					}
					std::cout << j.basePackageName
						<< NORMAL
						<< " " << j.version
						<< " " << j.description
						<< std::endl;
					std::set<std::string> groupList;
					groupList = parseDelimitedSetList(j.group," ");
					for ( auto k : groupList ) {
						if ( k == j.basePackageName)
							continue;
						std::string name = j.basePackageName
						+ "."
						+ k;
						std::cout << "(" << i.collection << ") ";
						if ( checkPackageNameExist(name ))
							std::cout << GREEN;
						std::cout << name
							<< NORMAL
							<< " " << j.version
							<< " " << j.description
							<< std::endl;
						name="";
					}
					continue;
				}
				bool found = false;
				std::set<std::string> groupList;
				groupList = parseDelimitedSetList(j.group," ");
				for ( auto k : groupList ) {
					if ( k == j.basePackageName)
						continue;
					if ( convertToLowerCase(m_argParser.otherArguments()[0]) == k ) {
					std::string name = j.basePackageName
						+ "."
						+ k;
					std::cout << "(" << i.collection << ") ";
					if ( checkPackageNameExist(name )) {
						std::cout << GREEN;
					}
					std::cout << name
						<< NORMAL
						<< " " << j.version
						<< " " << j.description
						<< std::endl;
					name="";
					found = true;
					continue;
					}
				}
				if (found)
					continue;
				std::set<std::string> aliasList;
				aliasList = parseDelimitedSetList(j.alias," ");
				for ( auto k : aliasList ) {
					if ( k == j.basePackageName)
						continue;
					if ( convertToLowerCase(m_argParser.otherArguments()[0]) == k ) {
						std::string name = j.basePackageName;
						std::cout << "(" << i.collection << ") ";
						if ( checkPackageNameExist(name )) {
							std::cout << GREEN;
						}
					std::cout << name
						<< NORMAL
						<< " " << j.version
						<< " " << j.description
						<< std::endl;
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
