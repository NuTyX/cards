//
//  pkgrepo.cxx
//
//  Copyright (c) 2002 - 2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 - 2023 by NuTyX team (http://nutyx.org)
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


#include "pkgrepo.h"

Pkgrepo::Pkgrepo(const std::string& fileName)
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
	m_parseCollectionDirectory = false;
	m_parsePackagePkgfileFile = false;

}
void Pkgrepo::throwError
	(const std::string& s) const
{
	switch ( m_ErrorCond )
	{
		case cards::ERROR_ENUM_CANNOT_FIND_DEPOT:
			throw RunTimeErrorWithErrno("could not find the Depot META " + s);
			break;
	}
}
void Pkgrepo::parseCollectionPkgRepoFile()
{
	if (m_parsePkgRepoCollectionFile)
		return;
	cards::Conf m_config(m_configFileName);
	for (auto i : m_config.dirUrl()) {
		PortsDirectory portsDirectory;
		portsDirectory.dir = i.dir;
		portsDirectory.url = i.url;
		std::string collectionPkgRepoFile = i.dir + "/.PKGREPO" ;
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
#ifndef NDEBUG
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			std::cerr << i.url << " "<< i.Dir << " "
			<< j.basePackageName  << " "
			<< j.version << " "
			<< j.md5SUM <<  " "
			<< j.s_buildDate << std::endl;
		}
	}
#endif
	m_parsePkgRepoCollectionFile = true;
}
std::vector<PortsDirectory> Pkgrepo::getListOfPortsDirectory()
{
	parseCollectionPkgRepoFile();
	return m_portsDirectoryList;

}
void Pkgrepo::parseCollectionDirectory()
{
	if (m_parseCollectionDirectory)
		return;
	cards::Conf m_config(m_configFileName);
	for (auto i : m_config.dirUrl()) {
		// We don't want to check the folders which can sync with a mirror
		if ( i.url.size() > 0 )
			continue;
		PortsDirectory portsDirectory;
		BasePackageInfo basePkgInfo;
		portsDirectory.dir = i.dir;
		std::set<std::string> localPackagesList;
		std::string::size_type pos;
		if ( findDir( localPackagesList, portsDirectory.dir) != 0 ) {
			std::cerr << YELLOW << "continue with the next entry" << NORMAL << std::endl;
			continue;
		}

		for (auto li : localPackagesList) {
			basePkgInfo.basePackageName = li;
			portsDirectory.basePackageList.push_back(basePkgInfo);
		}
		m_portsDirectoryList.push_back(portsDirectory);
		localPackagesList.clear();
	}
	if (m_portsDirectoryList.size() > 0)
		m_parseCollectionDirectory = true;
}
std::vector<PortsDirectory> Pkgrepo::getListOfCollectionDirectory()
{
	parseCollectionDirectory();
	return m_portsDirectoryList;

}

void Pkgrepo::parsePackagePkgRepoFile(const std::string& pkgRepoFile)
{
/*
 *
 * From here we can check whats is available in the port directory,
 * It means from here the concerned VALID .PKGREPO file MUST be available.
 * It can be the Pkgfile, the binary, the post-install, etc
 * We have to parse the file /var/lib/pkg/depot/cli/alsa-lib/.PKGREPO
 *
*/
#ifndef NDEBUG
	std::cerr << pkgRepoFile << std::endl;
#endif
	std::vector<std::string> PkgRepoFileContent;
	parseFile(PkgRepoFileContent,pkgRepoFile.c_str());
	PortFilesList portFilesList;
	for (auto input : PkgRepoFileContent) {
		if (input.size() < 11) {
			std::cerr << "[" << input << "]: Wrong format field to small" << std::endl;
			continue;
		}
		if ( input[10] != '#' ) {	// It's not the first line, then it can be one of our files
			std::vector<std::string> infos;
			split( input, '#', infos, 0,true);
			portFilesList.md5SUM = infos[0];
			portFilesList.name = infos[1];
			portFilesList.arch = "";
			if ( infos.size() > 2 ) {
				if ( infos[2].size() > 0 ) {
					portFilesList.arch = infos[2];
				} else  {
				// If there are only 2 fields then it's not a binary go on with next one
					continue;
				}
			} else {
				// If there are only 2 fields then it's not a binary go on with next one
				continue;
			}
#ifndef NDEBUG
			std::cerr << m_BasePackageInfo_i->basePackageName << ": " << portFilesList.md5SUM
			<< " " << portFilesList.name << " " << portFilesList.arch << std::endl;
#endif
		}
		m_portFilesList.push_back(portFilesList);
	}
}
std::vector<PortFilesList> Pkgrepo::getCurrentPackagePkgRepoFile(const std::string& portName)
{
	if (checkPortExist(portName))
		parsePackagePkgRepoFile(portName + "/.PKGREPO");

	return m_portFilesList;
}
std::set<std::string> Pkgrepo::getListOutOfDate()
{
	parseCollectionDirectory();

	std::set<std::string> listOfPackages;
	// For each collection activate in cards.conf
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
		for (auto j : i.basePackageList) {
			if ( j.extention == "" ) {
				listOfPackages.insert(j.basePackageName);
			}
		}
	}
	return listOfPackages;
}
void Pkgrepo::parsePackagePkgfileFile()
{
	if (m_parsePackagePkgfileFile)
		return;
	parseCollectionDirectory();

	std::string::size_type pos;
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			cards::Cache* pkg = new cards::Cache();
			std::string pkgFile = i.dir + "/" + j.basePackageName  + "/Pkgfile";
			if ( ! checkFileExist (pkgFile) ) {
				pkg->name("");
				continue;
			}
			pkg->fileDate(getModifyTimeFile(pkgFile));
			std::vector<std::string> pkgFileContent;
			if ( parseFile(pkgFileContent,pkgFile.c_str()) != 0) {
				std::cout << "You should use " << YELLOW << "ports -u" << NORMAL << " for " << i.dir << std::endl;
				std::cerr << "Cannot read the file: " << pkgFile << std::endl
					<< " ... continue with next" << std::endl;
				pkg->name("");
				continue;
			}
			pkg->dirName(i.dir);
			std::string baseDir = basename(const_cast<char*>(i.dir.c_str()));
			pkg->collection(baseDir);
			pkg->name(j.basePackageName);
			pkg->release(1);
			for (auto p : pkgFileContent) {
				std::string line = stripWhiteSpace(p);
				if ( line.substr( 0, 12 ) == "description=" ){
					j.description = getValueBefore( getValue( line, '=' ), '#' );
					pkg->description(stripWhiteSpace( j.description ));
				} else if ( line.substr( 0, 4 ) == "url=" ){
					j.url = getValueBefore( getValue( line, '=' ), '#' );
					pkg->url(stripWhiteSpace( j.url ));
				} else if ( line.substr( 0, 9 ) == "packager=" ){
					j.packager = getValueBefore( getValue( line, '=' ), '#' );
					pkg->packager(stripWhiteSpace( j.packager ));
				} else if ( line.substr( 0, 13 ) == "contributors=" ){
					j.contributors = getValueBefore( getValue( line, '=' ), '#' );
					pkg->contributors(stripWhiteSpace( j.contributors ));
				} else if ( line.substr( 0, 11 ) == "maintainer=" ){
					j.maintainer = getValueBefore( getValue( line, '=' ), '#' );
					pkg->maintainer(stripWhiteSpace( j.maintainer ));
				} else if ( line.substr( 0, 8 ) == "version=" ){
					j.version = getValueBefore( getValue( line, '=' ), '#' );
					pkg->version(stripWhiteSpace( j.version ));
				} else if ( line.substr( 0, 8 ) == "release=" ){
					std::string release = getValueBefore( getValue( line, '=' ), '#' );
					release = stripWhiteSpace(release );
					pkg->release(atoi(release.c_str()));
				} else if ( line[0] == '#' ) {
					while ( !line.empty() &&
							( line[0] == '#' || line[0] == ' ' || line[0] == '\t' ) ) {
						line = line.substr( 1 );
					}
					pos = line.find(':');
					if ( pos != std::string::npos ) {
						if ( startsWithNoCase( line, "desc" ) ) {
							pkg->description(stripWhiteSpace( getValue( line, ':' ) ) );
						} else if ( startsWithNoCase( line, "url" ) ) {
							pkg->url(stripWhiteSpace( getValue( line, ':' ) ) );
						} else if ( startsWithNoCase( line, "pac" ) ) {
							pkg->packager(stripWhiteSpace( getValue( line, ':' ) ) );
						} else if ( startsWithNoCase( line, "mai" ) ) {
							pkg->maintainer(stripWhiteSpace( getValue( line, ':' ) ) );
						}
					}
				}
			}
			m_packagesList.insert(pkg);
		}
	}
	m_parsePackagePkgfileFile = true;
}
std::set<std::string> Pkgrepo::getListOfPackagesFromCollection
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
std::set<std::string> Pkgrepo::getListOfPackagesFromSet
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
std::set<std::string> Pkgrepo::getBinaryPackageList()
{
	parseCollectionPkgRepoFile();

	std::string packageNameVersion;
	std::string baseDir;
	std::set<std::string>  binaryList;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
#ifndef NDEBUG
		std::cerr << i.dir << " " << i.url << std::endl;
#endif
		for (auto j : i.basePackageList) {
#ifndef NDEBUG
			std::cerr << j.basePackageName << " "
					<< j.description << " "
					<< j.md5SUM << " "
					<< j.version << " "
					<< j.s_buildDate << " "
					<< j.extention << std::endl ;
#endif
			if ( ( j.set=="none ") || (j.set.size()==0 ) ||
				 ( j.set=="none") )
				baseDir = basename(const_cast<char*>(i.dir.c_str()));
			else
				baseDir = j.set;
			packageNameVersion= "(" + baseDir + ")\t"
				+ j.basePackageName
				+ "\t"
				+ j.version
				+ "\t"
				+ j.packager
				+ "\t"
				+ j.description;
			binaryList.insert(packageNameVersion);
		}
	}
	return binaryList;
}

std::set<cards::Cache*>
Pkgrepo::getBinaryPackageSet()
{
	parseCollectionPkgRepoFile();

	// std::set<cards::Cache*> pkgList;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
#ifndef NDEBUG
		std::cerr << i.dir << " " << i.url << std::endl;
#endif
		for (auto j : i.basePackageList) {
#ifndef NDEBUG
			std::cerr << j.basePackageName << " "
				<< j.description << " "
				<< j.version << std::endl;
#endif
			cards::Cache* pkg = new cards::Cache();
			std::string baseDir = basename(const_cast<char*>(i.dir.c_str()));
			pkg->name(j.basePackageName);
			pkg->version(j.version);
			pkg->description(j.description);
			if ( ( j.set=="none ") || (j.set.size()==0 ) ||
				 ( j.set=="none") )
				pkg->collection(baseDir);
			else
				pkg->sets(j.set);

			pkg->packager(j.packager);

			m_packagesList.insert(pkg);
		}
	}
	return m_packagesList;
}
std::vector<RepoInfo> Pkgrepo::getRepoInfo()
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
unsigned int Pkgrepo::getPortsList()
{
	parseCollectionDirectory();
	parsePackagePkgfileFile();


	unsigned int numberOfPorts = 0;
	for (auto j : m_packagesList) {
		// For each directory found in this collection
		std::cout << "("
			<< j->collection()
			<< ") "
			<< j->name()
			<< " "
			<< j->version()
			<< "-"
			<< j->release()
			<<  std::endl;
		numberOfPorts++;
	}
	return numberOfPorts;
}
bool Pkgrepo::getPortInfo(const std::string& portName)
{

	parseCollectionDirectory();
	parsePackagePkgfileFile();

	bool found = false;
	for (auto j : m_packagesList) {
		if ( j->name() == portName ) {
			found = true;
			std::cout << _("Name           : ") << portName << std::endl
				<< _("Description    : ") << j->description() << std::endl
				<< _("URL            : ") << j->url() << std::endl
				<< _("Version        : ") << j->version() << std::endl
				<< _("Release        : ") << j->release() << std::endl
				<< _("Maintainer(s)  : ") << j->maintainer() << std::endl
				<< _("Contributor(s) : ") << j->contributors() << std::endl
				<< _("Packager       : ") << j->packager() << std::endl
				<< _("Date of Pkgfile: ") << j->fileDate() << std::endl
				<< _("Local folder   : ") << j->dirName() << std::endl;
		}
	}
	return found;
}
bool Pkgrepo::getBinaryPackageInfo(const std::string& packageName)
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
std::string Pkgrepo::getPortDir (const std::string& portName)
{
	parseCollectionDirectory();

	std::string portDir = "";
	bool found = false;
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == portName ) {
				portDir = i.dir + "/" + j.basePackageName;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	return portDir;
}
std::string Pkgrepo::getBasePackageName(const std::string& packageName)
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
std::string Pkgrepo::getBasePackageVersion(const std::string& packageName)
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
std::string Pkgrepo::getBasePortName (const std::string& portName)
{
	parseCollectionDirectory();
	parsePackagePkgfileFile();

	std::string basePortName = portName;
	std::string::size_type pos = portName.find('.');
	if (pos != std::string::npos) {
		basePortName = portName.substr(0,pos);
	}
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == basePortName ) {
				return j.basePackageName;
			}
		}
	}
	return "";
}
std::string Pkgrepo::getPortVersion (const std::string& portName)
{
	parseCollectionDirectory();
	parsePackagePkgfileFile();

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == portName ) {
				return j.version;
			}
		}
	}
	return "";
}
int Pkgrepo::getBasePackageRelease (const std::string& packageName)
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
int Pkgrepo::getPortRelease (const std::string& portName)
{

	parseCollectionDirectory();
	parsePackagePkgfileFile();

	int release = 1;
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == portName ) {
				return j.release;
			}
		}
	}
	return release;
}
bool Pkgrepo::checkPortExist(const std::string& portName)
{
	parseCollectionDirectory();
	parsePackagePkgfileFile();

	std::string basePortName = portName;
	std::string::size_type pos = portName.find('.');
	if (pos != std::string::npos) {
		basePortName=portName.substr(0,pos);
	}

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == basePortName ) {
				return true;
			}
		}
	}
	return false;
}
time_t Pkgrepo::getBinaryBuildTime (const std::string& packageName)
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
void Pkgrepo::clearPackagesList()
{
	for (auto i : m_packagesList )
	{
		if (i !=nullptr)
			delete i;
	}
	m_packagesList.clear();
}
// vim:set ts=2 :
