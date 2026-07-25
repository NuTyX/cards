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
	std::cout << "List of installed Packages: "
		<< std::endl
		<< std::endl;
    pkginfo pkginfo("cards list");
    pkginfo.installed();
    pkginfo.run();
}
void list::parse() {
	struct package {
		std::string name;
		std::string ver;
		std::string rel;
		std::string col;
	};

    cards::conf config(m_configFileName);

    struct stat st;
    size_t size = 0;      // Size of the file we parse

    std::vector<package> listOfPackages;

    for (auto i : config.dirUrl()) {
        std::string s = i.depot + "/" + i.collection + "/.REPO";
    
        int fd = open(s.c_str(),O_RDONLY);

        if (fstat(fd, &st) < 0) {
			perror("fstat");
			close(fd);
            std::cerr << "Cannot open "
                << s
                << "\n Quit now !!!"
                << std::endl;
			return;
		}
        size = st.st_size;

		char *data = static_cast<char*>
			(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
        
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
		const char *end = data + size;
        
        package pkg;
        pkg.col=" ";

        while (p < end) {
			const char *nl = (const char*)memchr(p, '\n', size);
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
        munmap(data,size);
		close(fd);
    }
	std::cout << "\n\nList of available binaries Packages: "
		<< std::endl
		<< std::endl;
    for (auto p : listOfPackages)
        printf("(%s) %s %s-%s\n"
            , p.col.c_str()
            , p.name.c_str()
            , p.ver.c_str()
            , p.rel.c_str());
}

}