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
	std::set<std::string> m_binarySetList;
	std::set<std::string> m_binaryCollectionList;
	std::set<std::string> m_binaryPackageList;
	std::string m_binaryPackageInfo;

	void parse();

public:
	pkgrepo(const std::string& fileName);
	virtual ~pkgrepo() { }

	std::string&           getBinaryPackageInfo(const std::string& name);
	std::set<std::string>& getBinaryPackageList();
	std::set<std::string>& getListOfPackagesFromSet(const std::string& name);
	std::set<std::string>& getListOfPackagesFromCollection(const std::string& name);
	repo_t&                getListOfPackages();
	bool                   checkPackageNameExist(const std::string& name);
};

} // end of 'cards' namespace
