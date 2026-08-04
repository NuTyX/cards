/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "search.h"

namespace cards {

search::search(const CardsArgumentParser& argParser,
            const std::string& configFileName)
        : m_argParser(argParser)
        , m_configFileName(configFileName)
{
	parse();
	list();
}
void search::parse(){
    int fd = 0;

    cards::conf config(m_configFileName);
    config.size();


    for (auto i : config.dirUrl()) {
        std::string s = i.depot + "/" + i.collection + PKG_REPO_META;

        fd = open(s.c_str(),O_RDONLY);

		char *data = static_cast<char*>
			(mmap(nullptr, i.sRepo, PROT_READ, MAP_PRIVATE, fd, 0));

        if (data == MAP_FAILED) {
			perror("mmap");
            std::cerr << "Cannot read "
                << s
                << "\n Quit now !!!"
                << std::endl;
			close(fd);
			return;
		}

		const char *p = data;
		const char *end = data + i.sRepo;

        package pkg;
        pkg.collection=" ";

        while (p < end) {
			const char *nl = (const char*)memchr(p, '\n', i.sRepo);
    		if (!nl)
				nl = end;

			if (p[0] == '\n') {
				m_listOfPackages.push_back(pkg);
				pkg.group="";
				pkg.description="";
				pkg.nls="";
			} else {
				size_t len = nl - p;
				std::string found(p + 1, len - 1);
				switch (p[0]) {
					case NAME:
						pkg.name = found;
						break;
					case VERSION:
						pkg.version = found;
						break;
					case RELEASE:
						pkg.release = found;
						break;
					case COLLECTION:
						pkg.collection = found;
						break;
					case DESCRIPTION:
						pkg.description = found;
						break;
					case URL:
						pkg.url = found;
						break;
					case PACKAGER:
						pkg.packager = found;
						break;
					case GROUP:
						pkg.group = found;
						break;
					case NLS:
						pkg.nls = found;
						break;
				}
			}
			p = nl + 1;
		}
        munmap(data,i.sRepo);
		close(fd);
    }
}
void search::list() {

	bool   found = false;
	pkgdbh dbh;

	dbh.buildDatabase(false);
	for (auto j : m_listOfPackages) {
		found = false;
		std::string::size_type pos;

		if (m_argParser.isSet(CardsArgumentParser::OPT_NAMES)) {
			if ( j.name.find(convertToLowerCase(m_argParser.otherArguments()[0])) == std::string::npos)
					continue;

			std::cout << "(" << j.collection << ") ";
			if (dbh.checkPackageNameExist(j.name)) {
				std::cout << GREEN;
			}
			std::cout << j.name
					<< NORMAL
					<< " " << j.version
					<< " " << j.description
					<< std::endl;
			continue;
		}
		pos = j.collection.find(convertToLowerCase(m_argParser.otherArguments()[0]));
		if  (pos == std::string::npos)
			pos = j.name.find(convertToLowerCase(m_argParser.otherArguments()[0]));
		if  (pos == std::string::npos)
			pos = convertToLowerCase(j.description).find(convertToLowerCase(m_argParser.otherArguments()[0]));
		if  (pos == std::string::npos)
			pos = convertToLowerCase(j.url).find(convertToLowerCase(m_argParser.otherArguments()[0]));
		if  (pos == std::string::npos)
			pos = convertToLowerCase(j.packager).find(convertToLowerCase(m_argParser.otherArguments()[0]));
		if  (pos == std::string::npos)
			pos = convertToLowerCase(j.version).find(convertToLowerCase(m_argParser.otherArguments()[0]));
		if (pos != std::string::npos) {
			if (j.nls.size() > 0) {
				found = false;
			} else {
				found = true;
			}
			if (!found)
				continue;

			std::cout << "(" << j.collection << ") ";
			if (dbh.checkPackageNameExist(j.name)) {
				std::cout << GREEN;
			}

			std::cout << j.name
				<< NORMAL
				<< " " << j.version
				<< " " << j.description
				<< std::endl;
		}
	}
}
}
