/* SPDX-License-Identifier: LGPF-2.1-or-later */
#include "sync.h"

namespace cards {

const std::string sync::DEFAULT_REPOFILE = ".REPO";

sync::sync()
	:m_config("/etc/cards.conf")
{
    m_repoFile = DEFAULT_REPOFILE;
    m_root = "/";
    m_configFile = "/etc/cards.conf";
}
sync::sync(const std::string configFile)
	: m_root("/")
	, m_configFile(configFile)
	, m_config(m_root + m_configFile)
{
    m_repoFile = DEFAULT_REPOFILE;
}
void sync::purge()
{
	std::set<std::string> listOfFiles;
	for (auto collection : m_config.dirUrl()) {
		std::string path = collection.depot
			+ "/"
			+ collection.collection;

		if (collection.url.size() == 0 )
			continue;

		findRecursiveFile(listOfFiles, path.c_str(),WS_DEFAULT);
		for (auto file : listOfFiles)
			remove(file.c_str());
	}
}
void sync::run() 
{
	for (auto collection : m_config.dirUrl()) {
		if (collection.url.size() == 0 ) {
			continue;
		}
		cards::dwl repo(collection.url
			+ "/"
			+ m_config.arch()
			+ "/"
			+ m_config.version()
			+ "/"
			+ collection.collection
			+ "/"
			+ m_repoFile,
			collection.depot + "/" + collection.collection,
			m_repoFile, true);
	}
}
}
