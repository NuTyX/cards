/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "conf.h"
namespace cards {

conf::conf()
{
	m_filename = NUTYX_VERSION_FILE;
	parseConfig();
	consolidate(true);
	m_filename = CARDS_CONF_FILE;
	parseConfig();
	consolidate(false);
}
conf::conf(const std::string& filename)
{
	m_filename = NUTYX_VERSION_FILE;
	parseConfig();
	consolidate(true);
	m_filename = filename;
	parseConfig();
	consolidate(false);
}
void conf::consolidate(bool sys)
{
	for (auto i : m_dirUrl) {
		cards::DirUrl DU;
		DU.collection = i.collection;
		if (i.depot.size() == 0) {
			if (depot().size() > 0)
				DU.depot = depot();
			else
				DU.depot = ".";
		} else
			DU.depot = i.depot;

		if (i.url.size() == 0) {
			if (url().size() > 0)
				DU.url = url();
		} else
			DU.url = i.url;

		if (i.dir.size() == 0) {
			DU.dir = DU.depot
				+ "/"
				+ DU.collection;
		} else
			DU.dir = i.dir;

		if (sys)
			m_sysconf.push_back(DU);
		else
			m_userconf.push_back(DU);
	}
	if (!sys) {
		if (m_url.size() > 0) {
			for (auto& i : m_sysconf)
				i.url = m_url;
		}
	}
	m_dirUrl.clear();
}
void conf::parseConfig()
{
	FILE* fp = fopen(m_filename.c_str(), "r");
	if (!fp)
		return;
	char line[BUFSIZ];
	std::string s;
	while (fgets(line, BUFSIZ, fp)) {
		if (line[strlen(line)-1] == '\n' ) {
			line[strlen(line)-1] = '\0';
		}
		s = line;

		// strip comments
		std::string::size_type pos = s.find('#');
		if (pos != std::string::npos) {
			s = s.substr(0,pos);
		}

		// whitespace separates
		pos = s.find(' ');
		if (pos == std::string::npos) {
			// try with a tab
			pos = s.find('\t');
		}
		if (pos != std::string::npos) {
			std::string key = s.substr(0, pos);
			std::string val = stripWhiteSpace(s.substr(pos));

			if (key == "dir") {
				std::string::size_type pos = val.find('|');
				cards::DirUrl DU;
				if (pos != std::string::npos) {
					DU.dir = stripWhiteSpace(val.substr(0,pos));
					DU.url = stripWhiteSpace(val.substr(pos+1));
				} else {
					DU.dir = stripWhiteSpace(val);
					DU.url = "";
					pos = DU.dir.rfind('/');
					if (pos != std::string::npos) {
						DU.collection = stripWhiteSpace(DU.dir.substr(pos+1));
						DU.depot = stripWhiteSpace(DU.dir.substr(0,pos));
					}
				}
				m_dirUrl.push_back(DU);
			}
			if (key == "collection") {
				cards::DirUrl DU;
				DU.collection = val;
				m_dirUrl.push_back(DU);
			}
			if (key == "depot") {
				m_depot = val;
			}
			if (key == "url") {
				m_urls.push_back(val);
			}
			if (key == "logdir") {
				m_logdir = val;
			}
			m_arch = getMachineType();
			if ( m_arch== "" ) {
				return;
			}
			if ( (key == "locale") || (key == "group") ) {
				m_groups.push_back(val);
			}
			if (key == "base") {
				m_baseDir.push_back(val);
			}
			if (key == "name") {
				m_name = val;
			}
			if (key == "version") {
				m_version = val;
			}
			if (key == "url")
				m_url = val;
			if (key == "hostname")
				m_hostname = val;
			if (key == "username")
				m_username = val;
			if (key == "database")
				m_database = val;
			if (key == "password")
				m_password = val;
			if (key == "arch") {
				m_archs.push_back(val);
			}
		}
	}
	fclose(fp);
}

conf::~conf()
{
	//nothing todo ATM
}
std::vector<DirUrl> conf::dirUrl()
{
	if (m_userconf.size() > 0)
		return m_userconf;
	else
		return m_sysconf;
}
std::string conf::url()
{
	return m_url;
}
std::string conf::hostname()
{
	return m_hostname;
}
std::string conf::username()
{
	return m_username;
}
std::string conf::password()
{
	return m_password;
}
std::string conf::database()
{
	return m_database;
}
std::string conf::version()
{
	return m_version;
}
std::string conf::arch()
{
	return m_arch;
}
std::string conf::depot()
{
	return m_depot;
}
std::string conf::logdir()
{
	return m_logdir;
}
std::vector<std::string> conf::groups()
{
	return m_groups;
}
std::vector<std::string> conf::collections()
{
	return m_collections;
}
std::vector<std::string> conf::depots()
{
	return m_depots;
}
std::vector<std::string> conf::baseDir()
{
	return m_baseDir;
}
} // end of cards namespace

