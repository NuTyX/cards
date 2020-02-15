//
//  pkgrepo.h
//
//  Copyright (c) 2002 - 2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 - 2020 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

#ifndef PKGREPO_H
#define PKGREPO_H

#include "file_utils.h"
#include "pkg.h"

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
	std::string URL;
	std::string contributors;
	std::string maintainer;
	std::string packager;
	std::string fileDate;
	std::string alias;
	std::string set;
	std::string group;
	time_t buildDate;
	std::vector<PortFilesList> portFilesList;
};

struct PortsDirectory {
	std::string Dir;
	std::string Url;
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

class Pkgrepo
{
public:

		Pkgrepo(const std::string& fileName);
		virtual ~Pkgrepo() { clearPackagesList(); }

		static int parseConfig(const char *fileName,
			Config& config);

		void throwError(const std::string& s) const;

/**
 *
 *  return a list of ports which has to be compiled OR
 *  has to be updated
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * Populate: nothing
 *
 */
		std::set<std::string> getListOutOfDate();

/**
 *	return the folder of the port name
 *
 */
		std::string getPortDir (const std::string& portName);


/**
 *
 * 	return the basename of the portname
 *
 */
		std::string getBasePortName (const std::string& portName);
/**
 *      return the basename of the packagename
 *
 */
		std::string getBasePackageName(const std::string& packageName);
/**
 *
 *	return the version of the packagename
 */

		std::string getBasePackageVersion(const std::string& packageName);
/**
 *  return the version of the port name
 */
		std::string getPortVersion (const std::string& portName);

/**
 *  return the release of the packagename
 *
 */
		int getBasePackageRelease (const std::string& packageName);

/**
 *  return the release of the port name
 */
		int getPortRelease(const std::string& portName);
/**
 *  return true if port name exist
 */ 
		bool checkPortExist(const std::string& portName);

/**
 *  retun list of packages of the collection
 */
		std::set<std::string> getListOfPackagesFromCollection(const std::string& collectionName);

/**
 *  retun list of packages of a set
 */
		std::set<std::string> getListOfPackagesFromSet(const std::string& collectionName);

/**
 *	return the build time of the binary 
 */
		time_t getBinaryBuildTime (const std::string& portName);

/**
 *  return the list of available binaries packages
 *
 */
		std::set<std::string> getBinaryPackageList();

/**
 *  return a Pkg set List of available binaries packages
 *
 */
		std::set<Pkg*> getBinaryPackageSet();
/**
 * populate RepoInfo List
 *
 */
		std::vector<RepoInfo> getRepoInfo();
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
 *
 * parse the config file
 **/
    int parseConfig(const char *fileName);

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
    void parsePkgRepoCollectionFile();

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
    void parseCurrentPackagePkgRepoFile();

/**
 * parse the "Pkgfile" file for each basePackage
 * add the version of the port found in the Pkgfile
 *
 */
    void parsePackagePkgfileFile();


		bool m_parsePkgRepoCollectionFile;
		bool m_parseCollectionDirectory;
		bool m_parsePackagePkgfileFile;

		std::vector<PortsDirectory>::iterator m_PortsDirectory_i;
		std::vector<BasePackageInfo>::iterator m_BasePackageInfo_i;
		std::vector<PortFilesList>::iterator m_PortFilesList_i;

		std::vector<PortFilesList> m_portFilesList;

		std::string m_configFileName;
		Config m_config;

		std::vector<PortsDirectory> m_portsDirectoryList;
private:
		error m_ErrorCond;
		void clearPackagesList();
		std::set<Pkg*> m_packagesList;
};
#endif /* PKGREPO_H */
// vim:set ts=2 :
