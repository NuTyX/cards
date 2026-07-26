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
			return;
		}
		if(m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			std::cout << m_pkgrepo.getBinaryPackageInfo(m_argParser.otherArguments()[0]);
			return;
		}
		pkginfo pkginfo("cards info");
		pkginfo.details(m_argParser.otherArguments()[0]);
		pkginfo.run();
		return;
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_SEARCH) ) {

		bool   found = false;
		pkgdbh dbh;

		dbh.buildDatabase(false);
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
