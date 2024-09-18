/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "string_utils.h"

namespace cards {

struct DirUrl {
	std::string collection;
	std::string depot;
	std::string dir;
	std::string url;
};

class conf
{
public:
	conf();
	conf(const std::string& filename);
	~conf();

	std::string version();
	std::string url();
	std::string keypath();
	std::string hostname();
	std::string username();
	std::string password();
	std::string database();

	std::string arch();
	std::string depot();
	std::string logdir();
	std::vector<std::string> groups();
	std::vector<std::string> urls();
	std::vector<std::string> depots();
	std::vector<std::string> collections();
	std::vector<DirUrl> dirUrl();
	std::vector<std::string> baseDir();

private:
	std::string m_filename;
	void consolidate(bool sys);
	void parseConfig();
	std::string m_name;
	std::string m_version;
	std::string m_arch;
	std::string m_depot;
	std::string m_url;
	std::string m_keypath;

	std::string m_dir;
	std::string m_base;
	std::string m_logdir;
	std::string m_username;
	std::string m_hostname;
	std::string m_database;
	std::string m_password;
	std::vector<std::string> m_groups;
	std::vector<std::string> m_urls;
	std::vector<std::string> m_collections;
	std::vector<std::string> m_depots;
	std::vector<std::string> m_baseDir;
	std::vector<std::string> m_archs;

	std::vector<DirUrl> m_dirUrl;

	std::vector<DirUrl> m_sysconf;
	std::vector<DirUrl> m_userconf;
};

} // endof cards namespace
