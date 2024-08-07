/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "string_utils.h"

namespace cards {

struct DirUrl {
	std::string dir;
	std::string url;
	std::string collection;
};

class conf
{
public:
	conf();
	conf(const std::string& filename);
	~conf();

	std::string version();
	std::string url();
	std::string hostname();
	std::string username();
	std::string password();
	std::string database();

	std::string arch();
	std::string logdir();
	std::vector<std::string> groups();
	std::vector<DirUrl> dirUrl();
	std::vector<std::string> baseDir();

private:
	std::string m_filename;
	void parseSystemConf();
	void parseCardsconf();
	std::string m_name;
	std::string m_version;
	std::string m_arch;
	std::string m_url;
	std::string m_dir;
	std::string m_base;
	std::string m_logdir;
	std::string m_username;
	std::string m_hostname;
	std::string m_database;
	std::string m_password;
	std::vector<std::string> m_groups;
	std::vector<DirUrl> m_dirUrl;
	std::vector<std::string> m_baseDir;
	std::vector<std::string> m_archs;
};

} // endof cards namespace
