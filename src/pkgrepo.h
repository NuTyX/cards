/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "cache.h"
#include "conf.h"
#include "file_utils.h"
#include "pkgadd.h"

namespace cards {

typedef std::map<std::string, cards::cache> repo_t;

class pkgrepo : public pkgadd {

    std::string           m_configFileName;
    cards::conf           m_config;
    repo_t                m_listOfPackages;

    std::set<std::string> m_binarySetList;
    std::set<std::string> m_binaryCollectionList;
    std::set<std::string> m_binaryPackageList;

    std::string           m_binaryPackageInfo;
    std::string           m_packageFileName;
    std::string           m_packageFileNameSignature;
    std::string           m_packageVersion;

    std::vector<std::pair<std::string,time_t>>
                          m_dependenciesList;
    void parse();

public:
    pkgrepo(const std::string& fileName);
    virtual ~pkgrepo() { }

	void generateDependencies(const std::pair<std::string,time_t>& packageName);
	void generateDependencies();
	void downloadPackageFileName(const std::string& packageName);

    bool                     checkBinaryExist(const std::string& name);

    time_t                   getBinaryBuildTime (const std::string& name);

    std::string&             getPackageFileName(const std::string& name);
    std::string&             getPackageFileNameSignature(const std::string& name);
    std::string&             getBinaryPackageInfo(const std::string& name);
    std::string&             getPackageVersion(const std::string& name);
    unsigned short int       getPackageRelease(const std::string& name);

    std::vector<std::string> getListofGroups();
    std::set<std::string>&   getBinaryPackageList();
    std::set<std::string>&   getListOfPackagesFromSet(const std::string& name);
    std::set<std::string>&   getListOfPackagesFromCollection(const std::string& name);

    std::vector<std::pair<std::string,time_t>>&
                             getDependenciesList();
    void                     addDependenciesList(std::pair<std::string,time_t>& name);

    repo_t&                  getListOfPackages();
};

} // end of 'cards' namespace
