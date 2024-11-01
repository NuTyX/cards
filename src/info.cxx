/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "info.h"

namespace cards {

info::info(const CardsArgumentParser& argParser,
		   const std::string& configFileName)
		: m_argParser(argParser)
		, m_pkgrepo(configFileName)
{
	if ((m_argParser.getCmdValue() == ArgParser::CMD_INFO)) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_SETS)) {
			std::set<std::string> sortedPackagesList = m_pkgrepo.getListOfPackagesFromSet(m_argParser.otherArguments()[0]);;
			if (sortedPackagesList.size() == 0)
				sortedPackagesList = m_pkgrepo.getListOfPackagesFromCollection(m_argParser.otherArguments()[0]);;

			for (auto i : sortedPackagesList)
				std::cout << "("
					<< m_argParser.otherArguments()[0]
					<< ") "
					<< i
					<< std::endl;
		} else if(m_argParser.isSet(CardsArgumentParser::OPT_VERSION)) {
			std::cout << m_pkgrepo.version(m_argParser.otherArguments()[0])
				<< "-"
				<< itos(m_pkgrepo.release(m_argParser.otherArguments()[0]))
				<< std::endl;
		} else {
			std::cout << m_pkgrepo.getBinaryPackageInfo(m_argParser.otherArguments()[0]);
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_LIST) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_SETS)) {
			std::set<std::string> sortedSetList;
			for (auto i : m_pkgrepo.getListOfPackages())
				if ( i.second.sets().size() > 0 ) {
					for (auto s : i.second.sets())
						sortedSetList.insert(s);
				}
			for (auto i : sortedSetList)
				std::cout << i << std::endl;
		} else {
			std::set<std::string> sortedPackagesList;
			for (auto i : m_pkgrepo.getListOfPackages()) {
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
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_QUERY)) {
		/*
		* FIXME
		*/
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_FILES)) {
		/*
		* FIXME
		*/
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_SEARCH) ) {

		bool   found = false;
		pkgdbh dbh;

		dbh.buildDatabase(false, false);
		for (auto j : m_pkgrepo.getListOfPackages()) {
			found = false;
			std::string::size_type pos;

			if (m_argParser.isSet(CardsArgumentParser::OPT_NAMES)) {
				if ( j.first.find(convertToLowerCase(m_argParser.otherArguments()[0])) == std::string::npos)
					continue;

				std::cout << "(" << j.second.collection() << ") ";
				if (dbh.checkPackageNameExist(j.first)) {
					std::cout << GREEN;
				}

				std::cout << j.first
					<< NORMAL
					<< " " << j.second.version()
					<< " " << j.second.description()
					<< std::endl;
				continue;
			}

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
				} else {
					found = true;
				}
				if (!found)
					continue;

				std::cout << "(" << j.second.collection() << ") ";
				if (dbh.checkPackageNameExist(j.first)) {
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

}
