/* SPDX-License-Identifier: LGPF-2.1-or-later */
#include "sync.h"

namespace cards {

const std::string sync::DEFAULT_REPOFILE = ".REPO";

sync::sync() {
    m_repoFile = DEFAULT_REPOFILE;
    m_root = "/";
    m_configFile = "/etc/cards.conf";
}
sync::sync(const std::string configFile)
	: m_configFile(configFile) 
	{
    m_repoFile = DEFAULT_REPOFILE;
    m_root = "/";
}
void sync::run() 
{
	std::string configFile = m_root + m_configFile;
	cards::conf config(configFile);
	for (auto collection : config.dirUrl()) {
		std::cout << collection.url
			+ "/"
			+ m_repoFile
			+ collection.dir
			<< std::endl;
		if (collection.url.size() == 0 ) {
			continue;
		}
		FileDownload repo(collection.url
			+ "/"
			+ m_repoFile,
			collection.dir,
			m_repoFile, true);
	}
}
}
