/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "webrepo.h"

namespace cards {

webrepo::webrepo(const std::string& configFileName)
            : m_configFileName(configFileName)
            , m_config(configFileName)
{
    parse();
}
void webrepo::parse() {
    if (m_listOfPackages.size() > 0)
        return;

    pkg info;
    std::string::size_type pos;
    std::string version;

    struct stat st;
    size_t size = 0;


    for (auto i : m_config.dirUrl()) {
        std::string repoFile = i.depot
            + "/"
            + i.collection
            + PKG_REPO_META;
        pos = i.depot.rfind('/');

        if (pos != std::string::npos) 
		version = stripWhiteSpace(i.depot.substr(pos+1));

        if (version == "systemd") {
            info.system("systemd");
            info.branch("systemd");
        }
        if (version == "rolling") {
            info.system("sys-v");
            info.branch("rolling");
        }
        if (version == "testingd"){
            info.system("systemd");
            info.branch("testingd");
        }
        if (version == "testing") {
            info.system("sys-v");
            info.branch("testing");
        }
        info.collection(i.collection);

        std::string pkgName;

	int fd = open(repoFile.c_str(), O_RDONLY);
	if (fstat(fd, &st) < 0 ) {
		close(fd);
		return;
	}

	size = st.st_size;

	char *data = static_cast<char*>
		(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
	if (data == MAP_FAILED) {
		close(fd);
		return;
	}

	const char *p = data;
	const char *end = data + size;
	while (p < end) {
	    const char *nl = (const char*)memchr(p, '\n', size);
	    if (!nl)
		    nl = end;

            if (p[0]== '\n') {
		    m_listOfPackages[pkgName] = info;
	    } else {
		    size_t len = nl - p;
		    std::string found(p + 1, len - 1);
		    switch (p[0]) {
			    case NAME:
				    pkgName = info.system()
					    + " "
					    + info.branch()
					    + " "
					    + found;
				    info.baseName(found);
				    break;
			    case VERSION:
				    info.version(found);
				    break;
			    case RELEASE:
				    info.release(stoi(found));
				    break;
			    case DESCRIPTION:
				    info.description(found);
				    break;
			    case BUILD:
				    info.build(strtoul(found.c_str(), nullptr, 0));
				    break;
		    }
            }
	    p = nl + 1;
	}
	munmap(data,size);
	close(fd);
    }
}
const webrepo_t& webrepo::getListOfPackages() {
    parse();
    return m_listOfPackages;
}

} // cards namespace
