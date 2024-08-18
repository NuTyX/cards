/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "conf.h"
#include "file_utils.h"
#include "cache.h"

namespace cards {

typedef std::map<std::string, cards::cache> repo_t;

class pkgrepo {

	std::string m_configFileName;
	cards::conf m_config;
	repo_t m_listOfPackages;
	std::set<std::string> m_binaryPackageList;
	std::string m_binaryPackageInfo;

	void parseRepofile();

public:
	pkgrepo(const std::string& fileName);
	virtual ~pkgrepo() { }

	std::string&           getBinaryPackageInfo(const std::string& name);
	std::set<std::string>& getBinaryPackageList();
};

} // end of 'cards' namespace
