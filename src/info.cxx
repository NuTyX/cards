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

			return;
		}
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
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

			return;
		}
		std::cout << "List of installed Packages: "
			<< std::endl
			<< std::endl;
		pkginfo pkginfo("cards info");
		pkginfo.installed();
		pkginfo.run();
		return;
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_QUERY)) {
		regex_t preg;
        if (regcomp(&preg, m_argParser.otherArguments()[0].c_str(), REG_EXTENDED | REG_NOSUB)) {
			throw std::runtime_error(_("error compiling regular expression '")
				+ m_argParser.otherArguments()[0]
				+ _("', aborting"));
		}
        struct info
        {
            std::string collection;
            std::string file;
        };
        info Info;
        std::vector<std::pair<std::string, info>> result;
        Info.collection=_("Collection");
        Info.file=_("File");
        result.push_back(std::pair<std::string, info>(_("Package"),Info));
        unsigned int width1 = result.begin()->second.collection.length(); // Width of "Collection"
        unsigned int width2 = result.begin()->first.length(); // Width of "Package"

		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			for (auto i : m_pkgrepo.getListOfPackages()) {
				for (auto j : i.second.files) {
					const std::string file('/' + j);
					if (!regexec(&preg, file.c_str(), 0, 0, 0)) {
						info Info;
						Info.collection = i.second.collection();
						Info.file = j;
						result.push_back(std::pair<std::string, info>(i.first, Info));
						if (i.second.collection().length() > width1) {
							width1 = i.second.collection().length();
						}
						if (i.first.length() > width2) {
							width2 = i.first.length();
						}
					}
				}
			}
			regfree(&preg);
        	if (result.size() > 1) {
            	for (auto i : result) {
                	std::cout << std::left
                		<< std::setw(width1 + 2)
               	    	<< i.second.collection
               	    	<< std::setw(width2 + 2)
            	        << i.first
            	        << i.second.file
        	            << std::endl;
    	            }
	        } else {
                std::cout << _(": no owner(s) found") << std::endl;
        	}
		} else {
			pkginfo pkginfo("cards query");
			pkginfo.query(m_argParser.otherArguments()[0]);
			pkginfo.run();
			return;
		}
	}
	if ((m_argParser.getCmdValue() == ArgParser::CMD_FILES)) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			for (auto i : m_pkgrepo.getListOfPackages()) {
				if (m_argParser.otherArguments()[0] == i.first) {
						for (auto f : i.second.files)
							std::cout << f << std::endl;
				}
			}
		} else {
			pkginfo pkginfo("cards files");
			pkginfo.list(m_argParser.otherArguments()[0]);
			pkginfo.run();
			return;
		}

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
