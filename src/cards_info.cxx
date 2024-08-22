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
		bool found = false;
		buildSimpleDatabase();
		for (auto j : getListOfPackages()) {
			found = false;
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
				if (j.second.group().size() > 0) {
					/*
					 * TODO find a better algo
					 */
					if (j.second.group() == "devel")
						found = true;
					if (j.second.group() == "man")
						found = true;
					if (j.second.group() == "service")
						found = true;
					if (j.second.group() == "lib")
						found = true;
					if (j.second.group() == "doc")
						found = true;
				} else
					found = true;

				if (!found)
					continue;

				std::cout << "(" << j.second.collection() << ") ";
			if (checkPackageNameExist(j.first)) {
				std::cout << GREEN;
			}

			std::cout << j.first
				<< NORMAL
				<< " " << j.second.version()
				<< " " << j.second.description()
				<< std::endl;
			}
		}
	}
}
