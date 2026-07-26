/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "query.h"

namespace cards {

query::query(const CardsArgumentParser& argParser,
            const std::string& configFileName)
        : m_argParser(argParser)
        , m_configFileName(configFileName)
{
     if ((m_argParser.isSet(CardsArgumentParser::OPT_BINARIES))) {
        parse();
        return;
    }
	pkginfo pkginfo("cards query");
	pkginfo.query(m_argParser.otherArguments()[0]);
	pkginfo.run();
}
void query::parse() {
    regex_t preg;
    // m_argParser.otherArguments()[0] is the file we are searching
    if (regcomp(&preg, m_argParser.otherArguments()[0].c_str(), REG_EXTENDED | REG_NOSUB)) {
		throw std::runtime_error(_("error compiling regular expression '")
			+ m_argParser.otherArguments()[0]
			+ _("', aborting"));
	}
    struct package {
        std::string collection;
        std::string name;
        std::string file;
    };

    package Pkg;
    std::vector<package> listOfFiles;
    Pkg.collection = _("Collection");
    Pkg.name = _("Package");
    Pkg.file = _("File");
    listOfFiles.push_back(Pkg);
    unsigned int width1 = listOfFiles.begin()->collection.length(); // Width of "Collection"
    unsigned int width2 = listOfFiles.begin()->name.length();       // Width of "Package"

    cards::conf config(m_configFileName);

    for (auto i : config.dirUrl()) {
        std::string s = i.depot + "/" + i.collection + PKG_REPO_FILES;

        // Checking file's access already done
        FILE* fp = fopen (s.c_str(),"r");

        std::string line(BUFSIZ,'\0');
        char input[BUFSIZ];
        while (fgets(input,BUFSIZ,fp)) {
            input[strlen(input)-1] = '\0';
            line = input;
            if (line[0] == '@')  {
                Pkg.name = line.substr(1);
                continue;
            } else {
                line = "/" + line;
                if (!regexec(&preg, line.c_str(), 0, 0, 0)) {
                    Pkg.collection = i.collection;
                    Pkg.file = line;
                    listOfFiles.push_back(Pkg);
                    if (Pkg.collection.length() > width1)
                        width1 = Pkg.collection.length();
                    if (Pkg.name.length() > width2)
                        width2 = Pkg.name.length();

                }
            }
        }
    }
    regfree(&preg);
    if (listOfFiles.size() > 1) {
        for (auto i : listOfFiles) {
            std::cout << std::left
                << std::setw(width1 + 2)
               	<< i.collection
               	<< std::setw(width2 + 2)
            	<< i.name
                << i.file
        	     << std::endl;
    	    }
	} else {
        std::cout << _(": no owner(s) found") << std::endl;
	}
}

}