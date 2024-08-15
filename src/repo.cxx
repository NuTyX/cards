/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 - 2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 - .... by NuTyX team (https://nutyx.org)

#include "repo.h"

repo::repo(const std::string& fileName)
	: m_configFileName(fileName)
{
	if (!checkFileExist(m_configFileName)) {
		std::cerr << WHITE << "Warning: " << NORMAL << "No configuration file: " << YELLOW
			<<  m_configFileName  << NORMAL
			<< " found..." << std::endl
			<< WHITE << "You should create a new one based on the " << YELLOW
			<< m_configFileName << ".example" << NORMAL << " file" << std::endl;
	}

	m_parsePkgRepoCollectionFile = false;
}
void repo::throwError
	(const std::string& s) const
{
	switch ( m_ErrorCond )
	{
		case cards::ERROR_ENUM_CANNOT_FIND_DEPOT:
			throw RunTimeErrorWithErrno("could not find the Depot META " + s);
			break;
	}
}
void repo::parseCollectionPkgRepoFile()
{
	if (m_parsePkgRepoCollectionFile)
		return;
	cards::conf m_config(m_configFileName);
	for (auto i : m_config.dirUrl()) {
		PortsDirectory portsDirectory;
		portsDirectory.dir = i.dir;
		portsDirectory.url = i.url;
		std::string collectionPkgRepoFile = i.dir + PKG_REPO ;
		if ( ! checkFileExist(collectionPkgRepoFile)) {
			if ( i.url.size() > 0 ) {
				std::cout << "You should use " << YELLOW << "cards sync" << NORMAL << " for " << i.dir << std::endl;
			} else {
				m_ErrorCond = cards::ERROR_ENUM_CANNOT_FIND_DEPOT;
				throwError(collectionPkgRepoFile);
			}
			continue;
		}
		std::vector<std::string> PkgRepoFileContent;

		if ( parseFile(PkgRepoFileContent,collectionPkgRepoFile.c_str()) != 0) {
			m_ErrorCond = cards::ERROR_ENUM_CANNOT_FIND_DEPOT;
			throwError(collectionPkgRepoFile);
		}

		for ( auto input : PkgRepoFileContent) {
			BasePackageInfo basePkgInfo;
			basePkgInfo.release = 1;
			basePkgInfo.buildDate = 0;
			if ( input.size() <  34 ) {
				std::cerr << "missing info" << std::endl;
				continue;
			}
			if ( ( input[32] != '#' ) || ( input[43] != '#' ) ) {
				std::cerr << "wrong  info" << std::endl;
				continue;
			}
			std::vector<std::string> infos;
			split( input, '#', infos, 0,true);
			if ( infos.size() > 0 ) {
				if ( infos[0].size() > 0 ) {
					basePkgInfo.md5SUM = infos[0];
				}
			}
			if ( infos.size() > 1 ) {
				if ( infos[1].size() > 0 ) {
					basePkgInfo.s_buildDate = infos[1];
					basePkgInfo.buildDate = strtoul(infos[1].c_str(),NULL,0);
				}
			}
			if ( infos.size() > 2 ) {
				if ( infos[2].size() > 0 ) {
					basePkgInfo.basePackageName = infos[2];
				}
			}
			if ( infos.size() > 3 ) {
				if ( infos[3].size() > 0 ) {
					basePkgInfo.version = infos[3];
				}
			}
			if ( infos.size() > 4 ) {
				if ( infos[4].size() > 0 ) {
					basePkgInfo.release = atoi(infos[4].c_str());
				}
			}
			if ( infos.size() > 5 ) {
				if ( infos[5].size() > 0 ) {
					basePkgInfo.description = infos[5];
				}
			}
			if ( infos.size() > 6 ) {
				if ( infos[6].size() > 0 ) {
					basePkgInfo.url = infos[6];
				}
			}
			if ( infos.size() > 7 ) {
				if ( infos[7].size() > 0 ) {
					basePkgInfo.maintainer = infos[7];
				}
			}
			if ( infos.size() > 8 ) {
				if ( infos[8].size() > 0 ) {
					basePkgInfo.packager = infos[8];
				}
			}
			if ( infos.size() > 9 ) {
				if ( infos[9].size() > 0 ) {
					basePkgInfo.extention = infos[9];
				}
			}
			if ( infos.size() > 10 ) {
				if ( infos[10].size() > 0 ) {
					basePkgInfo.alias = infos[10];
				}
			}
			if ( infos.size() > 11 ) {
				if ( infos[11].size() > 0 ) {
					basePkgInfo.group = infos[11];
				}
			}
			if ( infos.size() > 12 ) {
				if ( infos[12].size() > 0 ) {
					basePkgInfo.contributors = infos[12];
				}
			}
			if ( infos.size() > 13 ) {
				if ( infos[13].size() > 0 ) {
					basePkgInfo.set = infos[13];
				}
			}
				if ( infos.size() > 14 ) {
				if ( infos[14].size() > 0 ) {
					basePkgInfo.categories = infos[14];
				}
			}
			portsDirectory.basePackageList.push_back(basePkgInfo);
		}
		m_portsDirectoryList.push_back(portsDirectory);
	}
#ifdef DEBUG
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			std::cerr << i.url << " "<< i.dir << " "
			<< j.basePackageName  << " "
			<< j.version << " "
			<< j.md5SUM <<  " "
			<< j.s_buildDate << std::endl;
		}
	}
#endif
	m_parsePkgRepoCollectionFile = true;
}
std::set<std::string> repo::getListOfPackagesFromCollection
	(const std::string& collectionName)
{
	parseCollectionPkgRepoFile();

	std::set<std::string> listOfPackages;

	for (auto i : m_portsDirectoryList) {
		std::string baseDir = basename(const_cast<char*>(i.dir.c_str()));
		if ( baseDir == collectionName) {
			for (auto j : i.basePackageList) {
				listOfPackages.insert(j.basePackageName);
			}
			return listOfPackages;
		}
	}
	return listOfPackages;
}
std::set<std::string> repo::getListOfPackagesFromSet
	(const std::string& setName)
{
	parseCollectionPkgRepoFile();

	std::set<std::string> listOfPackages;

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			std::set<std::string> set = parseDelimitedSetList(j.set, " ");
			for (auto k : set) {
				if (k == setName)
					listOfPackages.insert(j.basePackageName);
				}
		}
	}
	return listOfPackages;
}
std::set<cards::cache*>
repo::getBinaryPackageSet()
{
	parseCollectionPkgRepoFile();

	// std::set<cards::cache*> pkgList;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
#ifdef DEBUG
		std::cerr << i.dir << " " << i.url << std::endl;
#endif
		for (auto j : i.basePackageList) {
#ifdef DEBUG
			std::cerr << j.basePackageName << " "
				<< j.description << " "
				<< j.version << std::endl;
#endif
			cards::cache* Pkg = new cards::cache();
			std::set<std::string> sets;
			std::string baseDir = basename(const_cast<char*>(i.dir.c_str()));
			Pkg->name(j.basePackageName);
			Pkg->version(j.version);
			Pkg->description(j.description);
			if ( ( j.set=="none ") || (j.set.size()==0 ) ||
				 ( j.set=="none") )
				Pkg->collection(baseDir);
			else {
				sets.insert(j.set);
				Pkg->sets(sets);
			}
			Pkg->packager(j.packager);

			m_packagesList.insert(Pkg);
		}
	}
	return m_packagesList;
}
std::vector<RepoInfo> repo::getRepoInfo()
{
	std::string::size_type pos;
	std::vector<RepoInfo> List;
	parseCollectionPkgRepoFile();
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		RepoInfo ArchCollectionBasePackageList;
		pos = i.dir.find_last_of("/\\");
		if ( pos != std::string::npos ) {
			ArchCollectionBasePackageList.collection=i.dir.substr(pos+1);
			std::string s = i.dir.substr(0, pos );
			pos = s.rfind("/");
			if ( pos != std::string::npos ) {
				ArchCollectionBasePackageList.branch = s.substr(pos+1);
				s = i.dir.substr(0,pos);
				pos = s.rfind("/");
				if ( pos != std::string::npos )
					ArchCollectionBasePackageList.arch = s.substr(pos+1);
			}
		} else {
			ArchCollectionBasePackageList.collection="unknow";
			ArchCollectionBasePackageList.arch="unknow";
		}
		ArchCollectionBasePackageList.basePackageList=i.basePackageList;
		List.push_back(ArchCollectionBasePackageList);
	}
	return List;
}
bool repo::getBinaryPackageInfo(const std::string& packageName)
{
	parseCollectionPkgRepoFile();

	bool found = false;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == packageName ) {
				found = true;
				std::cout << _("Name           : ") << packageName << std::endl
					<< _("Alias          : ") << j.alias << std::endl
					<< _("Description    : ") << j.description << std::endl
					<< _("Categories     : ") << j.categories << std::endl
					<< _("Set            : ") << j.set << std::endl
					<< _("Groups         : ") << j.group << std::endl
					<< _("URL            : ") << j.url << std::endl
					<< _("Version        : ") << j.version << std::endl
					<< _("Release        : ") << j.release << std::endl
					<< _("Maintainer(s)  : ") << j.maintainer << std::endl
					<< _("Contributor(s) : ") << j.contributors << std::endl
					<< _("Packager       : ") << j.packager << std::endl
					<< _("Build date     : ") << getDateFromEpoch(j.buildDate) << std::endl
					<< _("Binary URL     : ") << i.url << std::endl
					<< _("Local folder   : ") << i.dir << std::endl;
				break;
			}
		}
		if (found)
			break;
	}
	return found;
}
std::string repo::getBasePackageName(const std::string& packageName)
{

	parseCollectionPkgRepoFile();

	std::string basePackageName = packageName;
	std::string::size_type pos = packageName.find('.');
	if (pos != std::string::npos) {
		basePackageName=packageName.substr(0,pos);
	}
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == basePackageName ) {
				return j.basePackageName;
			}
		}
	}
	return "";
}
std::string repo::getBasePackageVersion(const std::string& packageName)
{
	parseCollectionPkgRepoFile();

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == packageName ) {
					return j.version;
			}
		}
	}
	return "";
}
int repo::getBasePackageRelease (const std::string& packageName)
{
	 parseCollectionPkgRepoFile();

	int release = 1;
	for (auto i : m_portsDirectoryList) {
		for (auto  j : i.basePackageList) {
			if ( j.basePackageName == packageName ) {
				return j.release;
			}
		}
	}
	return release;
}
time_t repo::getBinaryBuildTime (const std::string& packageName)
{
	parseCollectionPkgRepoFile();

	std::string basePackageName = packageName;
	std::string::size_type pos = packageName.find('.');
	if (pos != std::string::npos)
		basePackageName=packageName.substr(0,pos);
	time_t buildTime = 0;
	bool found = false;
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == basePackageName ) {
				found = true;
				buildTime = j.buildDate;
				break;
			}
			if (found)
				break;
		}
		if (found)
			break;
	}
	return buildTime;
}
void repo::clearPackagesList()
{
	for (auto i : m_packagesList )
	{
		if (i !=nullptr)
			delete i;
	}
	m_packagesList.clear();
}
