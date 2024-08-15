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
			cards::cache PkgInfo;
			PkgInfo.release(1);
			PkgInfo.build(0);
			portsDirectory.packagesList.push_back(PkgInfo);
		}
		m_portsDirectoryList.push_back(portsDirectory);
	}
	m_parsePkgRepoCollectionFile = true;
}
std::set<std::string> repo::getListOfPackagesFromCollection
	(const std::string& collectionName)
{
	parseCollectionPkgRepoFile();

	std::set<std::string> listOfPackages;

	return listOfPackages;
}
std::set<std::string> repo::getListOfPackagesFromSet
	(const std::string& setName)
{
	parseCollectionPkgRepoFile();

	std::set<std::string> listOfPackages;

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.packagesList) {
			std::set<std::string> set = j.sets();
			for (auto k : set) {
				if (k == setName)
					listOfPackages.insert(j.baseName());
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
		for (auto j : i.packagesList) {
#ifdef DEBUG
			std::cerr << j.basePackageName << " "
				<< j.description << " "
				<< j.version << std::endl;
#endif
			cards::cache* Pkg = new cards::cache();
			std::set<std::string> sets;
			std::string baseDir = basename(const_cast<char*>(i.dir.c_str()));
			Pkg->name(j.baseName());
			Pkg->version(j.version());
			Pkg->description(j.description());
			if (j.sets().size()==0 )
				Pkg->collection(baseDir);
			else {
				Pkg->sets(j.sets());
			}
			Pkg->packager(j.packager());

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
		ArchCollectionBasePackageList.packagesList=i.packagesList;
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
		for (auto j : i.packagesList) {
			if ( j.baseName() == packageName ) {
				found = true;
				std::cout << _("Name           : ") << packageName << std::endl
					<< _("Description    : ") << j.description() << std::endl
					<< _("Groups         : ") << j.group() << std::endl
					<< _("URL            : ") << j.url() << std::endl
					<< _("Version        : ") << j.version() << std::endl
					<< _("Release        : ") << j.release() << std::endl
					<< _("Maintainer(s)  : ") << j.maintainer() << std::endl
					<< _("Contributor(s) : ") << j.contributors() << std::endl
					<< _("Packager       : ") << j.packager() << std::endl
					<< _("Build date     : ") << getDateFromEpoch(j.build()) << std::endl
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
		for (auto j : i.packagesList) {
			if ( j.baseName() == basePackageName ) {
				return j.baseName();
			}
		}
	}
	return "";
}
std::string repo::getBasePackageVersion(const std::string& packageName)
{
	parseCollectionPkgRepoFile();

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.packagesList) {
			if ( j.baseName() == packageName ) {
					return j.version();
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
		for (auto  j : i.packagesList) {
			if ( j.baseName() == packageName ) {
				return j.release();
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
		for (auto j : i.packagesList) {
			if ( j.baseName() == basePackageName ) {
				found = true;
				buildTime = j.build();
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
