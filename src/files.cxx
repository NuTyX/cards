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
    std::vector<std::string> repoFiles;


    cards::conf config(m_configFileName);


    for (auto i : config.dirUrl()) {
        std::string s = i.depot + "/" + i.collection + PKG_REPO_FILES;
        repoFiles.clear();
        pkgFound = false;
        if (parseFile(repoFiles,s.c_str()) !=0) {
           std::cerr << "Cannot read the file: "
                      << s
                      << std::endl
                      << "... continue with next"
                      << std::endl;
            continue;
		}
		for ( auto p : repoFiles) {
			if (p[0] == '@') {
                pos = p.find(pkgName);
				if (pos != std::string::npos) {
					pkgFound = true;
					continue;
				}
			}
			if (pkgFound) {
				if (p.size() > 0) {
					std::cout << p << std::endl;
				} else {
					pkgFound = false;
				}
			}
		}
    }
}

}