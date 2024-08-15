/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 - 2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 - .... by NuTyX team (https://nutyx.org)
#pragma once

#include "file_utils.h"
#include "cache.h"
#include "conf.h"

/**
 * representation of the .REPO file which belong
 * to the collection directory
 * define in the configuration file cards.conf.
 *
 */
struct PortsDirectory {
	std::string dir;
	std::string url;
	std::vector<cards::cache> packagesList;
};
/**
 * representation of the Repo mainly for the website
**/
struct RepoInfo {
	std::string branch;
	std::string arch;
	std::string collection;
	std::vector<cards::cache> packagesList;
};

class repo
{
	bool m_parsePkgRepoCollectionFile;

	cards::ErrorEnum m_ErrorCond;
	void clearPackagesList();
	std::set<cards::cache*> m_packagesList;

public:

	repo(const std::string& fileName);

	virtual ~repo() { clearPackagesList(); }

	void throwError(const std::string& s) const;

/**
 * return the basename of the packagename
 *
 */
	std::string getBasePackageName(const std::string& packageName);

/**
 * return the version of the packagename
 *
 */
	std::string getBasePackageVersion(const std::string& packageName);

/**
 * return the release of the packagename
 *
 */
	int getBasePackageRelease (const std::string& packageName);

/**
 * retun list of packages of the collection
 *
 */
	std::set<std::string> getListOfPackagesFromCollection(const std::string& collectionName);

/**
 * retun list of packages of a set
 *
 */
	std::set<std::string> getListOfPackagesFromSet(const std::string& collectionName);

/**
 * return the build time of the binary
 *
 */
	time_t getBinaryBuildTime (const std::string& portName);

/**
 * return a Pkg set List of available binaries packages
 *
 */
	std::set<cards::cache*> getBinaryPackageSet();

/**
 * populate RepoInfo List
 *
 */
	
	std::vector<RepoInfo> getRepoInfo();

	bool getBinaryPackageInfo(const std::string& packageName);

protected:

/**
 * parse the .PKGREPO file which belong to the collection found
 * in the configuration file cards.conf. It populate the
 * the m_packageList.basePackageName part by looking
 * the downloaded .PKGREPO file of each activate collection
 *
 * Depends on: m_config.dirUrl
 *
 * populate: m_portsDirectoryList (Dir, Url, BasePackageInfo for each possible port found in Dir)
 * with the contents of the collection .PKGREPO file
 *
 **/
	void parseCollectionPkgRepoFile();

	std::string m_configFileName;
	cards::conf m_config;

	std::vector<PortsDirectory> m_portsDirectoryList;

};
