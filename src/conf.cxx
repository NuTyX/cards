/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "conf.h"
namespace cards {

conf::conf()
{
	m_filename = NUTYX_VERSION_FILE;
	parseCardsconf();
	m_filename = CARDS_CONF_FILE;
	parseCardsconf();
}
conf::conf(const std::string& filename)
{
	m_filename = NUTYX_VERSION_FILE;
	parseCardsconf();
	m_filename = filename;
	parseCardsconf();
}
void conf::parseCardsconf()
{
	FILE* fp = fopen(m_filename.c_str(), "r");
	if (!fp)
		return;
	const int length = BUFSIZ;
	char line[length];
	std::string s;
	while (fgets(line, length, fp)) {
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
				}
				pos = DU.dir.rfind('/');
				if (pos != std::string::npos) {
					DU.depot = stripWhiteSpace(DU.dir.substr(pos+1));
				} else {
					DU.depot = "";
				}
				m_dirUrl.push_back(DU);
			}
			if (key == "path") {
				std::string::size_type pos = val.rfind('/');
				cards::DirUrl DU;
				if (pos != std::string::npos) {
					DU.depot = stripWhiteSpace(DU.dir.substr(pos+1));
					DU.dir = DU.dir = stripWhiteSpace(val.substr(0,pos));
				}
				m_dirUrl.push_back(DU);
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
	if (m_url.size() > 0) {
		std::vector<DirUrl> list;
		DirUrl DU;
		for ( auto i : m_dirUrl) {
			DU.dir=i.dir;
			DU.depot=i.depot;
			if ( i.url.size() == 0 ) {
				DU.url=m_url;
			} else {
				DU.url=i.url;
			}
			list.push_back(DU);
		}
		m_dirUrl.swap(list);
	}
#ifdef DEBUG
	std::cerr << "name: " << m_name << std::endl
		<< "version: " << m_version << std::endl;
	for ( auto i : m_dirUrl)
		std::cerr << "Dir: "
			<< i.dir
			<< std::endl
			<< "Url: "
			<< i.url
			<< std::endl
			<< "Depot: "
			<< i.depot
			<< std::endl;
#endif
}

conf::~conf()
{
	//nothing todo ATM
}
std::vector<DirUrl> conf::dirUrl()
{
	std::vector<DirUrl> ret;
	for (auto i : m_dirUrl) {
		DirUrl du;
		du.dir = i.dir;
		du.depot = i.depot;
		if (i.url.size()==0) {
			ret.push_back(du);
			continue;
		}
		du.url = i.url
			+ "/"
			+ arch()
			+ "/"
			+ version()
			+ "/"
			+ i.depot;
		ret.push_back(du);
	}
	return ret;
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
std::string conf::logdir()
{
	return m_logdir;
}
std::vector<std::string> conf::groups()
{
	return m_groups;
}
std::vector<std::string> conf::baseDir()
{
	return m_baseDir;
}
} // end of cards namespace

