/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "webrepo.h"

namespace cards {

webrepo::webrepo(const std::string& configFileName)
            : m_configFileName(configFileName)
            , m_config(configFileName)
{
    parse();
}
void webrepo::parse(){
    if (m_listOfPackages.size() > 0)
        return;

    pkg info;
    std::string::size_type pos;
    std::string version;
    for (auto i : m_config.dirUrl()){
        std::string repoFile = i.depot
            + "/"
            + i.collection
            + PKG_REPO_META;
        pos = i.depot.rfind('/');
        if (pos != std::string::npos) 
			version = stripWhiteSpace(i.depot.substr(pos+1));

        std::vector<std::string> repoFileContent;
        if (parseFile(repoFileContent,repoFile.c_str()) != 0){
            std::cerr << "CANNOT READ "
                << repoFile
                << std::endl;
            continue;
        }

        if (version == "systemd") {
            info.system("systemd");
            info.branch("rolling");
        }
        if (version == "rolling") {
            info.system("sys-v");
            info.branch("rolling");
        }
        if (version == "testingd"){
            info.system("systemd");
            info.branch("testing");
        }
        if (version == "testing") {
            info.system("sys-v");
            info.branch("testing");
        }
        info.collection(i.collection);

        bool pkgFound = false;
        std::string pkgName;
        for (auto p : repoFileContent) {
            if(p[0]== '@') {
                pos = p.find(".cards-");
                if (pos != std::string::npos) {
                    info.group("");
                    info.baseName("");
                    pkgFound = true;
                }
                continue;
            }
            if (pkgFound) {
                switch(p[0]) {
                case NAME:
                    pkgName = info.system()
                        + " "
                        + info.branch()
                        + " "
                        + p.substr(1);
                    info.baseName(p.substr(1));
                    continue;
                case VERSION:
                    info.version(p.substr(1));
                    continue;
                case RELEASE:
                    info.release(stoi(p.substr(1)));
                    continue;
                case DESCRIPTION:
                    info.description(p.substr(1));
                    continue;
                case BUILD:
                    info.build(strtoul(p.substr(1).c_str(), nullptr, 0));
                    continue;
                }
            }
            if (p.size() == 0) {
                pkgFound = false;
                m_listOfPackages[pkgName] = info;
            }
        }

    }
}
const webrepo_t& webrepo::getListOfPackages() {
    parse();
    return m_listOfPackages;
}

}
