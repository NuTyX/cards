//
//  pkgrepo.cxx
//
//  Copyright (c) 2002-2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014-2019 by NuTyX team (http://nutyx.org)
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

using namespace std;

Pkgrepo::Pkgrepo(const std::string& fileName)
	: m_configFileName(fileName)
{
	if (!checkFileExist(m_configFileName)) {
		cerr << WHITE << "Warning: " << NORMAL << "No configuration file: " << YELLOW
			<<  m_configFileName  << NORMAL
			<< " found..." << endl
			<< WHITE << "You should create a new one based on the " << YELLOW
			<< m_configFileName << ".example" << NORMAL << " file" << endl;
	}

	m_parsePkgRepoCollectionFile = false;
	m_parseCollectionDirectory = false;
	m_parsePackagePkgfileFile = false;

}
void Pkgrepo::throwError(const string& s) const{
	switch ( m_ErrorCond )
	{
		case CANNOT_FIND_DEPOT:
			throw RunTimeErrorWithErrno("could not find the Depot META " + s);
			break;
	}
}
void Pkgrepo::parsePkgRepoCollectionFile()
{
if (m_parsePkgRepoCollectionFile == false) {
	parseConfig(m_configFileName.c_str());
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		PortsDirectory portsDirectory;
		portsDirectory.Dir = i->Dir;
		portsDirectory.Url = i->Url;
		string collectionPkgRepoFile = i->Dir + "/.PKGREPO" ;
		if ( ! checkFileExist(collectionPkgRepoFile)) {
			if ( i->Url.size() > 0 ) {
				cout << "You should use " << YELLOW << "cards sync" << NORMAL << " for " << i->Dir << endl;
			} else {
				m_ErrorCond = CANNOT_FIND_DEPOT;
				throwError(collectionPkgRepoFile);
			}
			continue;
		}
		vector<string> PkgRepoFileContent;

		if ( parseFile(PkgRepoFileContent,collectionPkgRepoFile.c_str()) != 0) {
			m_ErrorCond = CANNOT_FIND_DEPOT;
			throwError(collectionPkgRepoFile);
		}

		for ( auto input : PkgRepoFileContent) {
			BasePackageInfo basePkgInfo;
			basePkgInfo.release = 1;
			basePkgInfo.buildDate = 0;
			if ( input.size() <  34 ) {
				cerr << "missing info" << endl;
				continue;
			}
			if ( ( input[32] != '#' ) || ( input[43] != '#' ) ) {
				cerr << "wrong  info" << endl;
				continue;
			}
			vector<string> infos;
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
					basePkgInfo.URL = infos[6];
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
			cerr << i.Url << " "<< i.Dir << " "
			<< j.basePackageName  << " "
			<< j.version << " "
			<< j.md5SUM <<  " "
			<< j.s_buildDate << endl;
		}
	}
#endif
	m_parsePkgRepoCollectionFile = true;
	}
}
void Pkgrepo::parseCollectionDirectory()
{
	parseConfig(m_configFileName.c_str());
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		// We don't want to check the folders which cannot sync with any mirror
		if ( i->Url.size() > 0 )
			continue;
		PortsDirectory portsDirectory;
		BasePackageInfo basePkgInfo;
		portsDirectory.Dir = i->Dir;
		set<string> localPackagesList;
		string::size_type pos;
		if ( findFile( localPackagesList, portsDirectory.Dir) != 0 ) {
			cerr << YELLOW << "continue with the next entry" << NORMAL << endl;
			continue;
		}

		for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); ++li) {
			basePkgInfo.basePackageName = *li;
			portsDirectory.basePackageList.push_back(basePkgInfo);
		}
		m_portsDirectoryList.push_back(portsDirectory);
		localPackagesList.clear();
	}
	if (m_portsDirectoryList.size() > 0)
		m_parseCollectionDirectory = true;
}

void Pkgrepo::parseCurrentPackagePkgRepoFile()
{
/*
 From here we can check whats is available in the port directory,
 It means from here the concerned VALID .PKGREPO file MUST be available.
 It can be the Pkgfile, the binary, the post-install, etc
 We have to parse the file
 /var/lib/pkg/saravane/server/alsa-lib/.PKGREPO
 .PKGREPO file = /var/lib/pkg/saravane/server/alsa-lib/.PKGREPO
*/

	string PkgRepoFile = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName  + "/.PKGREPO";
#ifndef NDEBUG
	cerr << PkgRepoFile << endl;
#endif
	vector<string> PkgRepoFileContent;
	parseFile(PkgRepoFileContent,PkgRepoFile.c_str());
	PortFilesList portFilesList;
	for (auto input : PkgRepoFileContent) {
		if (input.size() < 11) {
			cerr << "[" << input << "]: Wrong format field to small" << endl;
			continue;
		}
		if ( input[10] != '#' ) {	// It's not the first line, then it can be one of our files
			vector<string> infos;
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
			cerr << m_BasePackageInfo_i->basePackageName << ": " << portFilesList.md5SUM << " " << portFilesList.name << " " << portFilesList.arch << endl;
#endif
		} else {
		/*
		 * Let's check if the first line is something like:
		 * 1401638336#.cards.tar.xz#4.14.1#..#..#..
		 * We already have this info let's make sure it's still ok
		*/
			vector<string> infos;
			split( input, '#', infos, 0,true);
			if ( (m_BasePackageInfo_i->s_buildDate != infos[0] ) && ( m_BasePackageInfo_i->s_buildDate.size() > 0 ) ) {
				cerr << m_BasePackageInfo_i->basePackageName << ": " << m_BasePackageInfo_i->s_buildDate << " != " << infos[0] << endl;
			}
			if ( (m_BasePackageInfo_i->extention != infos[1]) && ( m_BasePackageInfo_i->extention.size() > 0 ) ) {
				cerr << m_BasePackageInfo_i->basePackageName << ": " << m_BasePackageInfo_i->extention << " != " << infos[1] << endl;
			}
			if ( (m_BasePackageInfo_i->version != infos[2] ) && ( m_BasePackageInfo_i->version.size() > 0 )) {
				cerr << m_BasePackageInfo_i->basePackageName << ": " << m_BasePackageInfo_i->version << " != " << infos[2] << endl;
			}
#ifndef NDEBUG
			cerr << m_BasePackageInfo_i->basePackageName << ": " << m_BasePackageInfo_i->buildDate << " " << m_BasePackageInfo_i->extention << endl;
#endif
			continue;
		}
		m_portFilesList.push_back(portFilesList);
	}
}
set<string> Pkgrepo::getListOutOfDate()
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	set<string> listOfPackages;
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
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	std::string::size_type pos;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			string pkgFile = i->Dir + "/" + j->basePackageName  + "/Pkgfile";
			if ( ! checkFileExist (pkgFile) ) {
				j->basePackageName = "";
				continue;
			}
			j->fileDate = getModifyTimeFile(pkgFile);
			vector<string> pkgFileContent;
			if ( parseFile(pkgFileContent,pkgFile.c_str()) != 0) {
				cout << "You should use " << YELLOW << "ports -u" << NORMAL << " for " << i->Dir << endl;
				cerr << "Cannot read the file: " << pkgFile << endl
					<< " ... continue with next" << endl;
				j->basePackageName = "";
				continue;
			}
			j->release = 1;
			for (vector<string>::const_iterator p = pkgFileContent.begin();p != pkgFileContent.end();++p) {
				string line = stripWhiteSpace(*p);
				if ( line.substr( 0, 12 ) == "description=" ){
					j->description = getValueBefore( getValue( line, '=' ), '#' );
					j->description = stripWhiteSpace( j->description );
				} else if ( line.substr( 0, 4 ) == "url=" ){
					j->URL = getValueBefore( getValue( line, '=' ), '#' );
					j->URL = stripWhiteSpace( j->URL );
				} else if ( line.substr( 0, 9 ) == "packager=" ){
					j->packager = getValueBefore( getValue( line, '=' ), '#' );
					j->packager = stripWhiteSpace( j->packager );
				} else if ( line.substr( 0, 13 ) == "contributors=" ){
					j->contributors = getValueBefore( getValue( line, '=' ), '#' );
					j->contributors = stripWhiteSpace( j->contributors );
				} else if ( line.substr( 0, 11 ) == "maintainer=" ){
					j->maintainer = getValueBefore( getValue( line, '=' ), '#' );
					j->maintainer = stripWhiteSpace( j->maintainer );
				} else if ( line.substr( 0, 8 ) == "version=" ){
					j->version = getValueBefore( getValue( line, '=' ), '#' );
					j->version = stripWhiteSpace( j->version );
				} else if ( line.substr( 0, 8 ) == "release=" ){
					string release = getValueBefore( getValue( line, '=' ), '#' );
					release = stripWhiteSpace(release );
					j->release = atoi(release.c_str());
				} else if ( line[0] == '#' ) {
					while ( !line.empty() &&
							( line[0] == '#' || line[0] == ' ' || line[0] == '\t' ) ) {
						line = line.substr( 1 );
					}
					pos = line.find(':');
					if ( pos != string::npos ) {
						if ( startsWithNoCase( line, "desc" ) ) {
							j->description = stripWhiteSpace( getValue( line, ':' ) );
						} else if ( startsWithNoCase( line, "url" ) ) {
							j->URL = stripWhiteSpace( getValue( line, ':' ) );
						} else if ( startsWithNoCase( line, "pac" ) ) {
							j->packager = stripWhiteSpace( getValue( line, ':' ) );
						} else if ( startsWithNoCase( line, "mai" ) ) {
							j->maintainer = stripWhiteSpace( getValue( line, ':' ) );
						}
					}
				}
			}
		}
	}
	m_parsePackagePkgfileFile = true;
}
set<string> Pkgrepo::getListOfPackagesFromCollection(const string& collectionName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	std::set<string> listOfPackages;

	for (auto i : m_portsDirectoryList) {
		string baseDir = basename(const_cast<char*>(i.Dir.c_str()));
		if ( baseDir == collectionName) {
			for (auto j : i.basePackageList) {
				listOfPackages.insert(j.basePackageName);
			}
			return listOfPackages;
		}
	}
	return listOfPackages;
}
set<string> Pkgrepo::getListOfPackagesFromSet(const string& setName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	std::set<string> listOfPackages;

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			std::set<string> set = parseDelimitedSetList(j.set, ' ');
			for (auto k : set) {
				if (k == setName)
					listOfPackages.insert(j.basePackageName);
				}
		}
	}
	return listOfPackages;
}
set<string> Pkgrepo::getBinaryPackageList()
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	string packageNameVersion;
	string baseDir;
	set<string>  binaryList;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
#ifndef NDEBUG
		cerr << i.Dir << " " << i.Url << endl;
#endif
		for (auto j : i.basePackageList) {
#ifndef NDEBUG
			cerr << j.basePackageName << " "
					<< j.description << " "
					<< j.md5SUM << " "
					<< j.version << " "
					<< j.s_buildDate << " "
					<< j.extention << endl ;
#endif
			if ( ( j.set=="none ") || (j.set.size()==0 ) || 
				 ( j.set=="none") )
				baseDir = basename(const_cast<char*>(i.Dir.c_str()));
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
set<Pkg*> Pkgrepo::getBinaryPackageSet()
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	// set<Pkg*> pkgList;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
#ifndef NDEBUG
		cerr << i.Dir << " " << i.Url << endl;
#endif
		for (auto j : i.basePackageList) {
#ifndef NDEBUG
			cerr << j.basePackageName << " "
				<< j.description << " "
				<< j.version << endl;
#endif
			Pkg* pkg = new Pkg();
			string baseDir = basename(const_cast<char*>(i.Dir.c_str()));
			pkg->setName(j.basePackageName);
			pkg->setVersion(j.version);
			pkg->setDescription(j.description);
			if ( ( j.set=="none ") || (j.set.size()==0 ) || 
				 ( j.set=="none") )
				pkg->setCollection(baseDir);
			else
				pkg->setCollection(j.set);
			pkg->setPackager(j.packager);

			m_packagesList.insert(pkg);
		}
	}
	return m_packagesList;
}
vector<RepoInfo> Pkgrepo::getRepoInfo()
{
	string::size_type pos;
	std::vector<RepoInfo> List;
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		RepoInfo ArchCollectionBasePackageList;
		pos = i.Dir.find_last_of("/\\");
		if ( pos != string::npos ) {
			ArchCollectionBasePackageList.collection=i.Dir.substr(pos+1);
			string s = i.Dir.substr(0, pos );
			pos = s.rfind("/");
			if ( pos != string::npos ) {
				ArchCollectionBasePackageList.branch = s.substr(pos+1);
				s = i.Dir.substr(0,pos);
				pos = s.rfind("/");
				if ( pos != string::npos )
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
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePackagePkgfileFile)
		parsePackagePkgfileFile();


	unsigned int numberOfPorts = 0;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
#ifndef NDEBUG
		cerr << i.Dir << " " << i.Url << endl;
#endif
		for (auto j : i.basePackageList) {
#ifndef NDEBUG
			cerr << j.basePackageName << " "
				<< j.description << " "
				<< j.md5SUM << " "
				<< j.version << " "
				<< j.release << " "
				<< j.s_buildDate << " "
				<< j.extention << endl ;
#endif
			if (j.basePackageName.size() > 0) {
				string baseDir = basename(const_cast<char*>(i.Dir.c_str()));
				cout <<  i.Dir + "/"
					+ j.basePackageName
					<< " "
					<< j.version
					<< "-"
					<< j.release
					<< " "
					<< j.packager << endl;
				numberOfPorts++;
			}
		}
	}
	return numberOfPorts;
}
bool Pkgrepo::getPortInfo(const string& portName)
{

	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	if (!m_parsePackagePkgfileFile)
		parsePackagePkgfileFile();

	bool found = false;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		if (i.Url.size() > 0 )
			continue;
		// For each directory found in this collection
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == portName ) {
				found = true;
				cout << _("Name           : ") << portName << endl
					<< _("Description    : ") << j.description << endl
					<< _("URL            : ") << j.URL << endl
					<< _("Version        : ") << j.version << endl
					<< _("Release        : ") << j.release << endl
					<< _("Maintainer(s)  : ") << j.maintainer << endl
					<< _("Contributor(s) : ") << j.contributors << endl
					<< _("Packager       : ") << j.packager << endl
					<< _("Date of Pkgfile: ") << j.fileDate << endl
					<< _("Local folder   : ") << i.Dir << endl;
			}
		}
	}
	return found;
}
bool Pkgrepo::getBinaryPackageInfo(const string& packageName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	bool found = false;
	// For each defined collection
	for (auto i : m_portsDirectoryList) {
		// For each directory found in this collection
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == packageName ) {
				found = true;
				cout << _("Name           : ") << packageName << endl
					<< _("Alias          : ") << j.alias << endl
					<< _("Description    : ") << j.description << endl
					<< _("Groups         : ") << j.group << endl
					<< _("URL            : ") << j.URL << endl
					<< _("Version        : ") << j.version << endl
					<< _("Release        : ") << j.release << endl
					<< _("Maintainer(s)  : ") << j.maintainer << endl
					<< _("Contributor(s) : ") << j.contributors << endl
					<< _("Packager       : ") << j.packager << endl
					<< _("Build date     : ") << getDateFromEpoch(j.buildDate) << endl
					<< _("Binary URL     : ") << i.Url << endl
					<< _("Local folder   : ") << i.Dir << endl;
				break;
			}
		}
		if (found)
			break;
	}
	return found;
}
string Pkgrepo::getPortDir (const std::string& portName)
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	string portDir = "";
	bool found = false;
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == portName ) {
				portDir = i.Dir + "/" + j.basePackageName;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	return portDir;
}
string Pkgrepo::getBasePackageName(const string& packageName)
{

	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos) {
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
string Pkgrepo::getBasePackageVersion(const string& packageName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			if ( j.basePackageName == packageName ) {
					return j.version;
			}
		}
	}
	return "";
}
string Pkgrepo::getBasePortName (const string& portName)
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePackagePkgfileFile)
		parsePackagePkgfileFile();
	string basePortName = portName;
	string::size_type pos = portName.find('.');
	if (pos != string::npos) {
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
string Pkgrepo::getPortVersion (const string& portName)
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePackagePkgfileFile)
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
int Pkgrepo::getBasePackageRelease (const string& packageName)
{
	if (!m_parsePkgRepoCollectionFile)
		 parsePkgRepoCollectionFile();

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
int Pkgrepo::getPortRelease (const string& portName)
{

	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePackagePkgfileFile)
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
bool Pkgrepo::checkPortExist(const string& portName)
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePackagePkgfileFile)
		parsePackagePkgfileFile();

	string basePortName = portName;
	string::size_type pos = portName.find('.');
	if (pos != string::npos) {
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
time_t Pkgrepo::getBinaryBuildTime (const string& packageName)
{
	if (! m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();
	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos)
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
int Pkgrepo::parseConfig(const char *fileName)
{
	return parseConfig(fileName,m_config);
}
int Pkgrepo::parseConfig(const char *fileName, Config& config)
{
	int result = 0;
	if (checkFileExist("/var/lib/pkg/nutyx-version") ) {
		result = getConfig( "/var/lib/pkg/nutyx-version" , config);
		if ( result != 0 )
			return result;
	}
#ifndef NDEBUG
	cerr << "name:" << config.name << endl
		<< "version:" << config.version << endl;
#endif
	if ( config.name.size() == 0 ) {
		config.name="current";
	}
	if ( config.version.size() == 0 ) {
		config.version="current";
	}

	std::string::size_type pos;
	result = getConfig(fileName,config);
	if ( result != 0 )
		return result;

	for (vector<DirUrl>::iterator i = config.dirUrl.begin();
	i != config.dirUrl.end();
	++i) {
		DirUrl DU = *i ;
		if (DU.Url.size() == 0 )
			continue;

		string categoryDir = DU.Dir;
		string category = basename(const_cast<char*>(categoryDir.c_str()));
		string url = DU.Url
			+ "/"
			+ getMachineType()
			+ "/"
			+ config.version
			+ "/"
			+ category;

#ifndef NDEBUG
		cerr << url << endl;
#endif
		i->Url = url;
	}
	return result;
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
