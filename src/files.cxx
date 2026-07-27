/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "files.h"

namespace cards {

files::files(const CardsArgumentParser& argParser,
            const std::string& configFileName)
        : m_argParser(argParser)
        , m_configFileName(configFileName)
{
     if ((m_argParser.isSet(CardsArgumentParser::OPT_BINARIES))) {
        parse();
        return;
    }
    pkginfo pkginfo("cards files");
    pkginfo.list(m_argParser.otherArguments()[0]);
    pkginfo.run();
}
void files::parse() {
    std::string pkgName = "@" + m_argParser.otherArguments()[0] + ".cards-";
    std::string::size_type pos;
    bool pkgFound = false;

    cards::conf config(m_configFileName);

    for (auto i : config.dirUrl()) {
        std::string s = i.depot + "/" + i.collection + PKG_REPO_FILES;
        pkgFound = false;

        // Checking file's access already done
        FILE* fp = fopen (s.c_str(),"r");

        std::string line(BUFSIZ,'\0');
        char input[BUFSIZ];
        while (fgets(input,BUFSIZ,fp)) {
            input[strlen(input)-1] = '\0';
            line = input;
            if (line[0] == '@')  {
                pos = line.find(pkgName);
				if (pos != std::string::npos) {
					pkgFound = true;
					continue;
				}
			}
			if (pkgFound) {
				if (line.size() > 0) {
					std::cout << line << std::endl;
				} else {
					fclose(fp);
					return;
				}
			}
        }
        fclose(fp);
    }
}

}