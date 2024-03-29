#include "conf.h"
namespace cards {

Conf::Conf()
{
	parseSystemConf();
	m_filename = CARDS_CONF_FILE;
	parseCardsconf();
}
Conf::Conf(const std::string& filename)
{
	parseSystemConf();
	m_filename = filename;
	parseCardsconf();
}
void Conf::parseCardsconf()
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
			if ( i.url.size() == 0 ) {
				DU.url=m_url;
			} else {
				DU.url=i.url;
			}
			list.push_back(DU);
		}
		m_dirUrl.swap(list);
	}
#ifndef NDEBUG
	std::cerr << "name: " << m_name << std::endl
		<< "version: " << m_version << std::endl;
	for ( auto i : m_dirUrl)
		std::cerr << "Dir: "
			<< i.dir
			<< std::endl
			<< "Url: "
			<< i.url
			<< std::endl;
#endif
}
Conf::~Conf()
{
	//nothing todo ATM
}
std::vector<DirUrl> Conf::dirUrl()
{
	std::vector<DirUrl> ret;
	for (auto i : m_dirUrl) {
		DirUrl du;
		du.dir = i.dir;
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
			+ basename(const_cast<char*>(i.dir.c_str()));
		ret.push_back(du);
	}
	return ret;
}
void Conf::parseSystemConf()
{
	FILE* fp = fopen(NUTYX_VERSION_FILE, "r");
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
			if (key == "name") {
				m_name = val;
			}
			if (key == "version") {
				m_version = val;
			}
		}
	}
	fclose(fp);
#ifndef NDEBUG
		std::cerr << "name: " << m_name << std::endl
			<< "version: " << m_version << std::endl;
#endif
}
std::string Conf::url()
{
	return m_url;
}
std::string Conf::hostname()
{
	return m_hostname;
}
std::string Conf::username()
{
	return m_username;
}
std::string Conf::password()
{
	return m_password;
}
std::string Conf::database()
{
	return m_database;
}
std::string Conf::version()
{
	return m_version;
}
std::string Conf::arch()
{
	return m_arch;
}
std::string Conf::logdir()
{
	return m_logdir;
}
std::vector<std::string> Conf::groups()
{
	return m_groups;
}
std::vector<std::string> Conf::baseDir()
{
	return m_baseDir;
}
} // endof cards namespace

// vim:set ts=2 :

