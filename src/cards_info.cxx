/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "cards_info.h"

cards_info::cards_info(const CardsArgumentParser& argParser, const std::string& configFileName)
	: pkginfo("cards info"),cards::pkgrepo(configFileName), m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	if ((m_argParser.getCmdValue() == ArgParser::CMD_INFO)) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			getBinaryPackageInfo(m_argParser.otherArguments()[0]);
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_SETS)) {
			std::set<std::string> sortedPackagesList = getListOfPackagesFromSet(m_argParser.otherArguments()[0]);;
			if (sortedPackagesList.size() == 0)
				sortedPackagesList = getListOfPackagesFromCollection(m_argParser.otherArguments()[0]);;

			for (auto i : sortedPackagesList)
				std::cout << "("
					<< m_argParser.otherArguments()[0]
					<< ") "
					<< i
					<< std::endl;
		} else {
			m_details_mode=1;
			m_arg=m_argParser.otherArguments()[0];
			run();
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_LIST) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			std::set<std::string> sortedPackagesList;
			for (auto i : getListOfPackages()) {
				std::string s, _s;
				if ( i.second.sets().size()  > 0 ) {
					for (auto s : i.second.sets())
							_s += s + " ";
					s =  "(" + _s + ") ";
				} else {
					s = "(" + i.second.collection() + ") ";
				}
				s += i.first + " ";
				s += i.second.version() + " ";
				s += i.second.description();
				sortedPackagesList.insert(s);
			}
			for (auto i : sortedPackagesList)
				std::cout << i << std::endl;

		} else if (m_argParser.isSet(CardsArgumentParser::OPT_SETS)) {
			std::set<std::string> sortedSetList;
			for (auto i : getListOfPackages())
				if ( i.second.sets().size() > 0 ) {
					for (auto s : i.second.sets())
						sortedSetList.insert(s);
				}
			for (auto i : sortedSetList)
				std::cout << i << std::endl;
		} else {
			m_installed_mode=1;
			if (m_argParser.isSet(CardsArgumentParser::OPT_FULL))
				m_fulllist_mode=true;
			run();
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_QUERY)) {
		m_owner_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_FILES)) {
		m_list_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();		
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_SEARCH) ) {
		buildSimpleDatabase();
		for (auto j : getListOfPackages()) {
			std::string::size_type pos;
			pos = j.second.collection().find(convertToLowerCase(m_argParser.otherArguments()[0]));
			if  (pos == std::string::npos)
				pos = j.first.find(convertToLowerCase(m_argParser.otherArguments()[0]));
			if  (pos == std::string::npos)
				pos = convertToLowerCase(j.second.description()).find(convertToLowerCase(m_argParser.otherArguments()[0]));
			if  (pos == std::string::npos)
				pos = convertToLowerCase(j.second.url()).find(convertToLowerCase(m_argParser.otherArguments()[0]));
			if  (pos == std::string::npos)
				pos = convertToLowerCase(j.second.packager()).find(convertToLowerCase(m_argParser.otherArguments()[0]));
			if  (pos == std::string::npos)
				pos = convertToLowerCase(j.second.version()).find(convertToLowerCase(m_argParser.otherArguments()[0]));
			if (pos != std::string::npos) {
				std::cout << "(" << j.second.collection() << ") ";
			if (checkPackageNameExist(j.first)) {
				std::cout << GREEN;
			}
			std::cout << j.first
				<< NORMAL
				<< " " << j.second.version()
				<< " " << j.second.description()
				<< std::endl;
			std::set<std::string> groupList;
			groupList = parseDelimitedSetList(j.second.group()," ");
			for (auto k : groupList) {
				if (k == j.first)
					continue;
				std::string name = j.first
				+ "."
				+ k;
				std::cout << "(" << j.second.collection() << ") ";
				if (checkPackageNameExist(name))
					std::cout << GREEN;
					std::cout << name
						<< NORMAL
						<< " " << j.second.version()
						<< " " << j.second.description()
						<< std::endl;
					name="";
				}
				continue;
			}
			bool found = false;
			std::set<std::string> groupList;
			groupList = parseDelimitedSetList(j.second.group()," ");
			for (auto k : groupList) {
				if (k == j.first)
					continue;
				if (convertToLowerCase(m_argParser.otherArguments()[0]) == k) {
					std::string name = j.first
						+ "."
						+ k;
					std::cout << "(" << j.second.collection() << ") ";
					if (checkPackageNameExist(name))
						std::cout << GREEN;

					std::cout << name
						<< NORMAL
						<< " " << j.second.version()
						<< " " << j.second.description()
						<< std::endl;
					name="";
					found = true;
					continue;
				}
			}
			if (found)
				continue;
			std::set<std::string> aliasList;
			aliasList = j.second.alias();
			for (auto k : aliasList) {
				if (k == j.first)
					continue;
				if (convertToLowerCase(m_argParser.otherArguments()[0]) == k) {
					std::string name = j.first;
					std::cout << "(" << j.second.collection() << ") ";
					if ( checkPackageNameExist(name )) {
						std::cout << GREEN;
					}
					std::cout << name
						<< NORMAL
						<< " " << j.second.version()
						<< " " << j.second.description()
						<< std::endl;
					name="";
					found = true;
					continue;
				}
			}
		}
	}
}
