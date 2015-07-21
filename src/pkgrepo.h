//  pkgrepo.h
//
//  Copyright (c) 2002-2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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


#include <string>
#include <vector>
#include <list>
#include <map>
#include "file_download.h"
#include "file_utils.h"

struct PortFilesList {
	std::string md5SUM;
	std::string name;
	std::string arch;
};

/**
 * \representation of the .PKGREPO file which belong to the collection directory
 * define in the configuration file cards.conf.
 *************************************************
 * 73193bfc1cb30fe02a880ed088ed7590#1414192958#aalib#1.4rc5##n.a#n.a#n.a#n.a#.cards.tar.xz
 * 650ed499ce78791d45b91aaf7f91b445#1428615787#firefox#37.0.1#1#Standalone web browser from mozilla.org#http://www.mozilla.com/firefox/#n.a#pierre at nutyx dot org,tnut at nutyx dot org#.cards.tar.xz
 *************************************************
 **/
struct BasePackageInfo {
	std::string md5SUM;
	std::string s_buildDate;
	std::string basePackageName;
	std::string extention;
	std::string version;
	int release;
	std::string description;
	std::string URL;
	std::string maintainer;
	std::string packager;
	std::string fileDate;
	time_t buildDate;
	std::vector<PortFilesList> portFilesList;
};

/**
 * \representation of the .PKGREPO file locate in each port directory.
 * The difference with the collection .PKGREPO, it contains the list of
 * possible files (binaries, Pkgfile, README etc)
 * Exemple of firefox/.PKGREPO contents:
 ******************************************************
 * 1428615787#.cards.tar.xz#37.0.1#1#Standalone web browser from mozilla.org#http://www.mozilla.com/firefox/#n.a#pierre at nutyx dot org,tnut at nutyx dot org
 * ada2187e655daaec9195802b955cce4b#firefox#i686
 * 30413e9eac84e9a727c9f89fda341fb0#firefox.devel#any
 * 23e6f253809cdac206693c84a56184be#firefox.post-install
 * d47bfe8887dbbad7effc320ddf116d2e#Pkgfile
 *******************************************************
 **/
struct FileList {
	std::string basePackageName;
	std::string description;
	std::string URL;
	std::string md5SUM;
	std::string version;
	int release;
	std::string packager;
	std::string maintainer;
	std::string fileDate;
	std::string s_buildDate;
	time_t buildDate;
	std::string extention;
	std::vector<PortFilesList> portFilesList;
};

struct PortsDirectory {
	std::string Dir;
	std::string Url;
	std::vector<BasePackageInfo> basePackageList;
};

class Pkgrepo
{
	private:
/**
 *
 * \parse the config file
 **/
		int parseConfig(const std::string& fileName);

/**
 * \parse the .PKGREPO file which belong to the collection found
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
 * \parse the directory directly based on what we have locally.
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
 * \parse the ".PKGREPO" file of a port directory
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
 * \parse the "Pkgfile" file for each basePackage
 * add the version of the port found in the Pkgfile
 *
 */
		void parsePackagePkgfileFile();

/**
 *
 * \parse the .PKGREPO of all the packageName directory
 * 
 *
 */
		void parsePackagesPkgRepoFile();

	public:

		Pkgrepo(const std::string& fileName);
		static int parseConfig(const std::string& fileName,
			Config& config);
/**
 * \download the .PKGREPO of packageName
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * populate: nothing
 *
 * add: the .PKGREPO of the packageName port from the mirror
 *
 */
		void downloadPortsPkgRepo(const std::string& packageName);

/**
 * \download the packagefileName
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * populater: nothing
 *
 * add: The packageFileName from the mirror
 *
 */
		void downloadPackageFileName(const std::string& packageFileName);


/**
 *
 * \parse the basePackage Directory for binary package
 *  return a list of packages found 
 * 
 */
	set<string> getListOfPackagesFromDirectory(const std::string& path);


/**
 *
 * \return a list of ports which has to be compiled OR
 *  has to be updated
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * Populate: nothing
 *
 */
		set<string> getListOutOfDate();

/*
 *	\return the folder of the port name
 *
 */
		string getPortDir (const std::string& portName);

/*
 *      \return the basename of the packagename
 *
 */
		string getBasePackageName(const string& packageName);

/*
 *  \return the version of the port name
 */
		string getPortVersion (const std::string& portName);

/*
 *  \return the release of the port name
 */
		int getPortRelease(const std::string& portName);
/*
 *  \return true if port name exist
 */ 
		bool checkPortExist(const std::string& portName);

/*
 *	\return true if binarie name exist
 */
		bool checkBinaryExist(const std::string& packageName);

/*
 *  \return the location and the filename of the package if exist else
 *	return the packageName
 *
 *  Depends on: parsePkgRepoCollectionFile
 *
 *  Populate: nothing
 */
		std::string getPackageFileName(const std::string& packageName);
/*
 *	\return the build time of the binary 
 */
		time_t getBinaryBuildTime (const std::string& portName);

/*
 * \return true if one or more occurencies found
 */
		bool search(const std::string& s);

/*
 * \printout the list of available binaries packages
 *
 */
		unsigned int getBinaryPackageList();

/*
 * \printout the list of available ports which are compiled
 *  return a list of name version
 *
 */
    unsigned int getPortsList();

		bool getBinaryPackageInfo(const std::string& packageName);
		bool getPortInfo(const std::string& portName);

	private:
		bool m_parsePkgRepoCollectionFile;
		bool m_parseCollectionDirectory;
		bool m_parsePackagesPkgRepoFile;
		bool m_parsePackagePkgfileFile;

		std::vector<PortsDirectory>::iterator m_PortsDirectory_i;
		std::vector<BasePackageInfo>::iterator m_BasePackageInfo_i;
		std::vector<PortFilesList>::iterator m_PortFilesList_i;

		std::vector<PortFilesList> m_portFilesList;

		std::string m_packageFileName;
		std::string m_configFileName;
		Config m_config;

		std::vector<PortsDirectory> m_portsDirectoryList;
};
#endif /* PKGREPO_H */
// vim:set ts=2 :
