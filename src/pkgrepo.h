/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 - 2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 - .... by NuTyX team (https://nutyx.org)
#pragma once

#include "file_utils.h"
#include "cache.h"
#include "conf.h"

struct PortFilesList {
	std::string md5SUM;
	std::string name;
	std::string arch;
};

/**
 * representation of the .PKGREPO file which belong to the collection directory
 * define in the configuration file cards.conf.
 *
 */

/*
 **************************************************
 * 73193bfc1cb30fe02a880ed088ed7590#1414192958#aalib#1.4rc5##n.a#n.a#n.a#n.a#.cards.tar.xz
 * 650ed499ce78791d45b91aaf7f91b445#1428615787#firefox#37.0.1#1#Standalone web browser from mozilla.org#http://www.mozilla.com/firefox/#n.a#pierre at nutyx dot org,tnut at nutyx dot org#.cards.tar.xz
 *************************************************
 */
struct BasePackageInfo {
	std::string md5SUM;
	std::string s_buildDate;
	std::string basePackageName;
	std::string extention;
	std::string version;
	int release;
	std::string description;
	std::string categories;
	std::string url;
	std::string contributors;
	std::string maintainer;
	std::string packager;
	std::string fileDate;
	std::string alias;
	std::string set;
	std::string group;
	time_t buildDate;
	alias_t aliasList;
	std::vector<PortFilesList> portFilesList;
};

struct PortsDirectory {
	std::string dir;
	std::string url;
	std::vector<BasePackageInfo> basePackageList;
};
/**
 * representation of the Repo mainly for the website
**/
struct RepoInfo {
	std::string branch;
	std::string arch;
	std::string collection;
	std::vector<BasePackageInfo> basePackageList;
};

class pkgrepo
{
public:

	pkgrepo(const std::string& fileName);

	virtual ~pkgrepo() { clearPackagesList(); }

	void throwError(const std::string& s) const;

/**
 * return a list of ports which has to be compiled OR
 * has to be updated
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * Populate: nothing
 *
 */
	std::set<std::string> getListOutOfDate();

/**
 * return the folder of the port name
 *
 */
	std::string getPortDir (const std::string& portName);

/**
 * return the basename of the portname
 *
 */
	std::string getBasePortName (const std::string& portName);

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
 * return the version of the port name
 *
 */
	std::string getPortVersion (const std::string& portName);

/**
 * return the release of the packagename
 *
 */
	int getBasePackageRelease (const std::string& packageName);

/**
 * return the release of the port name
 *
 */
	int getPortRelease(const std::string& portName);

/**
 * return true if port name exist
 *
 */ 
	bool checkPortExist(const std::string& portName);

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
 * return the list of available binaries packages
 *
 */
	std::set<std::string> getBinaryPackageList();

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

/**
 * return the list of Ports Directory datas
 *
 */
std::vector<PortsDirectory> getListOfPortsDirectory();

/**
 * return the list of Collection Directory datas
 *
 */
std::vector<PortsDirectory> getListOfCollectionDirectory();

/**
 * return the list of files found in the founded PKGREPO file
 *
 */
std::vector<PortFilesList> getCurrentPackagePkgRepoFile(const std::string& portName);

/**
 * printout the list of available ports which are compiled
 *  return a list of name version
 *
 */
	unsigned int getPortsList();

	bool getBinaryPackageInfo(const std::string& packageName);
	bool getPortInfo(const std::string& portName);

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

/**
 * parse the directory directly based on what we have locally.
 * This method is used in the case of synchronisation with the mirror
 * is NOT possible. If they are no directories, they will be nothing add
 *
 * Depends on: m_config.dirUrl
 *
 * populate:  m_portsDirectoryList  ( Dir, BasePackageInfo->basePackageName only )
 *
 **/
	void parseCollectionDirectory();

/**
 * parse the ".PKGREPO" file of a port directory
 * if it found a first line with the date of construction
 * and the extension of the archive then it populate
 * the list of packages
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * Populate: m_portFilesList (md5SUM.name,arch)
 *
 */
	void parsePackagePkgRepoFile(const std::string& pkgRepo);

/**
 * parse the "Pkgfile" file for each basePackage
 * add the version of the port found in the Pkgfile
 *
 */
	void parsePackagePkgfileFile();



	std::vector<PortFilesList> m_portFilesList;

	std::string m_configFileName;
	cards::conf m_config;

	std::vector<PortsDirectory> m_portsDirectoryList;

private:

	bool m_parsePkgRepoCollectionFile;
	bool m_parseCollectionDirectory;
	bool m_parsePackagePkgfileFile;

	cards::ErrorEnum m_ErrorCond;
	void clearPackagesList();
	std::set<cards::cache*> m_packagesList;
};
