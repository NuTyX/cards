/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "list.h"

namespace cards {

list::list(const CardsArgumentParser& argParser,
            const std::string& configFileName)
        : m_argParser(argParser)
        , m_configFileName(configFileName)
{
    if ((m_argParser.isSet(CardsArgumentParser::OPT_BINARIES))) {
        parse();
        return;
    }
    if ((m_argParser.isSet(CardsArgumentParser::OPT_SETS))) {
        parseSets();
        return;
    }
    pkginfo pkginfo("cards list");
    pkginfo.installed();
    pkginfo.run();
}
void list::parseSets() {
    int fd = 0;
    cards::conf config(m_configFileName);
    config.size();

    std::set<std::string> sortedListOfSets;

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

        while (p < end) {
            const char *nl = (const char*)memchr(p, '\n', i.sRepo);
            if (!nl)
                nl = end;

			if (p[0] != '\n') {
				size_t len = nl - p;
				std::string found(p + 1, len - 1);
				switch (p[0]) {
					case SETS:
                        sortedListOfSets.insert(found);
						break;
				}
			}
			p = nl + 1;
		}
        munmap(data,i.sRepo);
		close(fd);
    }
    for (auto s : sortedListOfSets)
        printf("%s\n", s.c_str());
}
void list::parse() {
    int fd = 0;
	struct package {
		std::string name;
		std::string ver;
		std::string rel;
		std::string col;
	};

    cards::conf config(m_configFileName);
    config.size();

    std::vector<package> listOfPackages;

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
        pkg.col=" ";

        while (p < end) {
			const char *nl = (const char*)memchr(p, '\n', i.sRepo);
    		if (!nl)
				nl = end;

			if (p[0] == '\n') {
				listOfPackages.push_back(pkg);
			} else {
				size_t len = nl - p;
				std::string found(p + 1, len - 1);
				switch (p[0]) {
					case NAME:
						pkg.name = found;
						break;
					case VERSION:
						pkg.ver = found;
						break;
					case RELEASE:
						pkg.rel = found;
						break;
					case COLLECTION:
						pkg.col = found;
						break;
				}
			}
			p = nl + 1;
		}
        munmap(data,i.sRepo);
		close(fd);
    }
    for (auto p : listOfPackages)
        printf("(%s) %s %s-%s\n"
            , p.col.c_str()
            , p.name.c_str()
            , p.ver.c_str()
            , p.rel.c_str());
}

}