/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "info.h"

namespace cards {

info::info(const CardsArgumentParser& argParser,
		   const std::string& configFileName)
		: m_argParser(argParser)
		, m_pkgrepo(configFileName)
{
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
}

}
