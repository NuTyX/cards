//  config_parser.h
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

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include "file_download.h"


struct PackageFilesList {
	std::string md5SUM;
	std::string name;
	std::string arch;
};

struct FileList {
	std::string basePackageName;
	std::string description;
	std::string md5SUM;
	std::string version;
	time_t buildDate;
	std::string extention;
	std::vector<PackageFilesList> packageFilesList;
};

struct DirUrl {
	std::string Dir;
	std::string Url;
};

struct PortsDirectory {
	std::string Dir;
	std::string Url;
	std::vector<FileList> basePackageList;
}; 
struct Config {
	Config() {}
	std::string arch;
	std::vector<std::string> locale;
	std::vector<DirUrl> dirUrl;
	std::vector<std::string> baseDir;
};

class ConfigParser
{
	public:
		ConfigParser(const std::string& fileName);
		int parseConfig(const std::string& fileName);
		static int parseConfig(const std::string& fileName,
			Config& config);
/**
 * \parse the MD5SUM file which belong to the category found
 * in the configuration file cards.conf. It populate the 
 * the m_packageList.basePackageName part by looking
 * the download MD5SUM file of each activate category
 **/
		int parseMD5sumCategoryDirectory();

/**
 * \parse the directory directly based on what we have locally.
 * This method is used in the case of synchronisation with the mirror
 * is NOT possible. 
 **/
		int parseCategoryDirectory();

/**
 * \download the MD5SUM of each port
 */
		int parsePortsList();

/**
 * \download the packagefileName
 */
		void downloadPackageFileName(const std::string& packageFileName);

/**
 *
 * \parse the basePackage Directory for binary package
 *  return a list of packages found 
 * 
 */
	set<string> getListOfPackagesFromDirectory(const string path);

/**
 * \parse the MD5SUM of a port directory
 * if it found a first line with the date of construction
 * and the extension of the archive then it populate
 * the list of packages
 */
		int parseBasePackageList();

/*
 *  \parse the info file for each basePackage
 *	add the description of the port found in the first line of the info file
 */
		int parsePackageInfoList();

/*
 *  \return the version of the port name
 */
		string getPortVersion (const std::string& portName);

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
		bool getBinaryPackageInfo(const std::string& packageName);
		bool getPortInfo(const std::string& portName);

	private:
		std::vector<PortsDirectory>::iterator m_i;
		std::vector<FileList>::iterator m_j;
		std::vector<PackageFilesList>::iterator m_p;	
		std::string m_packageFileName;
		std::string m_configFileName;
		Config m_config;
		std::vector<PortsDirectory> m_packageList;
};
#endif /* CONFIGPARSER_H */
// vim:set ts=2 :
