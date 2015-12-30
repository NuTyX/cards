//  pkgrepo.cc
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

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>

#include "pkgrepo.h"
#include "error_treat.h"

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
	m_parsePackagesPkgRepoFile = false;
	m_parsePackagePkgfileFile = false;

}
void Pkgrepo::parsePkgRepoCollectionFile()
{
if (m_parsePkgRepoCollectionFile == false) {
	parseConfig(m_configFileName);
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		PortsDirectory portsDirectory;
		portsDirectory.Dir = i->Dir;
		portsDirectory.Url = i->Url;
		BasePackageInfo basePkgInfo;
		string collectionPkgRepoFile = i->Dir + "/.PKGREPO" ;
		basePkgInfo.release = 1;
		basePkgInfo.buildDate = 0;
		if ( ! checkFileExist(collectionPkgRepoFile)) {
			if ( i->Url.size() > 0 ) {
				cout << "You should used " << YELLOW << "cards sync" << NORMAL << " for " << i->Dir << endl;
			}
			continue;
		}
		vector<string> PkgRepoFileContent;

		if ( parseFile(PkgRepoFileContent,collectionPkgRepoFile.c_str()) != 0) {
			cerr << "cannot read file .PKGREPO" << endl;
			continue;
		}

		for ( vector<string>::iterator i = PkgRepoFileContent.begin();i!=PkgRepoFileContent.end();++i) {
			string input = *i;
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
			portsDirectory.basePackageList.push_back(basePkgInfo);
		}
		m_portsDirectoryList.push_back(portsDirectory);
	}
#ifndef NDEBUG
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		PortsDirectory portsDirectory = *i;
		for (std::vector<BasePackageInfo>::iterator j = portsDirectory.basePackageList.begin(); j != portsDirectory.basePackageList.end();++j) {
			cerr << i->Url << " "<< i->Dir << " "
			<< j->basePackageName  << " "
			<< j->version << " "
			<< j->md5SUM <<  " "
			<< j->s_buildDate << endl;
		}
	}
#endif
	m_parsePkgRepoCollectionFile = true;
	}
}
void Pkgrepo::parseCollectionDirectory()
{
	parseConfig(m_configFileName);
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
void Pkgrepo::downloadPortsPkgRepo(const string& packageName)
{
	InfoFile downloadFile;
	vector<InfoFile> downloadFilesList;
	bool found = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			if ( m_BasePackageInfo_i->basePackageName == packageName ) {
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
/*
	We should check if the PKGREPO of the port is available
	.PKGREPO file is /var/lib/pkg/saravane/server/alsa-lib/.PKGREPO */

	if ( m_PortsDirectory_i->Url.size() > 0 ) {
		downloadFile.url = m_PortsDirectory_i->Url + "/" + m_BasePackageInfo_i->basePackageName  + "/.PKGREPO";
		downloadFile.dirname = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName;
		downloadFile.filename = "/.PKGREPO";
		downloadFile.md5sum = m_BasePackageInfo_i-> md5SUM;
			downloadFilesList.push_back(downloadFile);
	}
#ifndef NDEBUG
	cerr << m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName  << endl;
#endif
	if ( downloadFilesList.size() > 0 ) {
		FileDownload FD(downloadFilesList,false);
	}
}
set<string> Pkgrepo::getListOfPackagesFromDirectory(const string& path)
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	set<string> listOfPackages; 
	bool found = false;

	// For each collection activate in cards.conf
	for (m_PortsDirectory_i = m_portsDirectoryList.begin(); m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
#ifndef NDEBUG
		cerr << m_PortsDirectory_i->Dir<< endl;
#endif
		// For each directory found in this collection
		for ( m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
#ifndef NDEBUG
			cerr << m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName << endl;
#endif
			if ( path == m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName) {
				found=true;
				parseCurrentPackagePkgRepoFile();
				break;
			}
		}
		if (found)
			break;
	}
	if (found) {
		// If we are dealing with the correct path ...
		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
#ifndef NDEBUG
			cerr << m_PortFilesList_i->md5SUM << "|" << m_PortFilesList_i->name << "|" << m_PortFilesList_i->arch   << endl;
#endif
			if ( ( m_PortFilesList_i->arch == "any" ) || ( m_PortFilesList_i->arch == m_config.arch) )
#ifndef NDEBUG
				cerr << m_PortFilesList_i->name << endl;
#endif
				listOfPackages.insert(m_PortFilesList_i->name);
		}
	}

	return listOfPackages;	
}
void Pkgrepo::parseCurrentPackagePkgRepoFile()
{
/*
 From here we can check whats is available in the port directory
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
	for (vector<string>::const_iterator i = PkgRepoFileContent.begin();i != PkgRepoFileContent.end(); ++i) {
		string input = *i;
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
		 * We already have this info let's make shure it's still ok
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
void Pkgrepo::parsePackagesPkgRepoFile()
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	vector<string> infos;
	// For each collection activate in cards.conf
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
#ifndef NDEBUG
		cerr << "m_PortsDirectory_i->Dir: " << m_PortsDirectory_i->Dir << endl;
#endif
		// For each port found in the current collection
		for ( m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			vector<string> PkgRepoFileContent;
			string PkgRepoFile = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName  + "/.PKGREPO";
			parseFile(PkgRepoFileContent,PkgRepoFile.c_str());
			PortFilesList portFilesList;
			for (vector<string>::const_iterator i = PkgRepoFileContent.begin();i != PkgRepoFileContent.end(); ++i) {
				string input = *i;
				if (input.size() < 11) {
					cerr << "[" << input << "]: Wrong format field to small" << endl;
					continue;
				}
				if ( input[10] != '#' ) { // It's not the first line, then it can be one of our files
#ifndef NDEBUG
					cerr << "input: " << input << endl;
#endif
					infos.clear();
					split( input, '#', infos, 0, false);
#ifndef NDEBUG
					cerr << "Number of fields: " << infos.size()<< endl;
#endif
					portFilesList.md5SUM = infos[0];
					portFilesList.name = infos[1];
					if ( infos.size() > 2 ) {
						if ( infos[2].size() > 0 ) {
							portFilesList.arch = infos[2];
						}
					} else {
							portFilesList.arch = "";
					}
#ifndef NDEBUG
					cerr << m_BasePackageInfo_i->basePackageName << ": "<< portFilesList.md5SUM << " " << portFilesList.name << " " << portFilesList.arch << endl;
#endif
				}
				if (input[10] == '#' ) {
					vector<string> infos;
					split( input, '#', infos, 0,true);
					m_BasePackageInfo_i->s_buildDate = infos[0];
					m_BasePackageInfo_i->extention = infos[1];
					m_BasePackageInfo_i->version = infos[2];
				}
				m_BasePackageInfo_i->portFilesList.push_back(portFilesList);
			}
		}
	}
}
set<string> Pkgrepo::getListOutOfDate()
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	set<string> listOfPackages;
	// For each collection activate in cards.conf
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this collection
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->extention == "" ) {
				listOfPackages.insert(j->basePackageName);
			}
		}
	}
	return listOfPackages;
}
void Pkgrepo::parsePackagePkgfileFile()
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();

	InfoFile downloadFile;
	vector<InfoFile> downloadFilesList;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PortFilesList>::iterator p = j->portFilesList.begin(); p != j ->portFilesList.end();++p) {
#ifndef NDEBUG
				cerr << p->md5SUM << " " << p->name << endl << endl;
#endif
				if ( p->name == "Pkgfile" ){
					if ( ! checkFileExist(i->Dir + "/" + j->basePackageName  + "/" + p->name) ) {
						downloadFile.url = i->Url + "/" + j->basePackageName  + "/" + p->name;
						downloadFile.dirname = i->Dir + "/" + j->basePackageName;
						downloadFile.filename = "/"+p->name;
						downloadFile.md5sum = p-> md5SUM;
						downloadFilesList.push_back(downloadFile);
#ifndef NDEBUG
						cerr << i->Dir + "/" + j->basePackageName  + "/" + p->name << endl;
#endif
					}
				}
			}
		}
	}
	if ( downloadFilesList.size() > 0 ) {
		FileDownload FD(downloadFilesList,false);
	}
/*
 * Time to add the various informations
 */
	std::string::size_type pos;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			string pkgFile = i->Dir + "/" + j->basePackageName  + "/Pkgfile";
			if ( ! checkFileExist (pkgFile) ) {
				j->basePackageName = "";
				continue;
			}
			j->fileDate = modifyTimeFile(pkgFile);
			vector<string> pkgFileContent;
			if ( parseFile(pkgFileContent,pkgFile.c_str()) != 0) {
				cerr << "Cannot read the file: " << pkgFile << endl;
				cerr << " ... continue with next" << endl;
				j->basePackageName = "";
				continue;
			}
			j->release = 1;
			for (vector<string>::const_iterator p = pkgFileContent.begin();p != pkgFileContent.end();++p) {
				string line = stripWhiteSpace(*p);
				if ( line.substr( 0, 8 ) == "version=" ){
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
void Pkgrepo::downloadPackageFileName(const std::string& packageName)
{
	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos) {
		basePackageName=packageName.substr(0,pos);
	}

	bool found = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i != m_portsDirectoryList.end();++m_PortsDirectory_i) {
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin();m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			if ( m_BasePackageInfo_i->basePackageName == basePackageName) {
				found=true;
				break;
			}
		}
		if (found)
			break;
	}
	if (found) {
#ifndef NDEBUG
		cerr << packageName << endl;
#endif
		if ( ! checkFileExist (m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO"))
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		parseCurrentPackagePkgRepoFile();

		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
			if (m_PortFilesList_i->name == packageName)
				break;
		}

		string url = m_PortsDirectory_i->Url + "/" + m_BasePackageInfo_i->basePackageName + "/" + m_PortFilesList_i->name + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i->arch + m_BasePackageInfo_i->extention;
		string dir = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName;
		string fileName = m_PortFilesList_i->name + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i-> arch + m_BasePackageInfo_i->extention;
#ifndef NDEBUG
		cerr << url << " "
		<< dir << " "
		<< fileName << " "
		<< m_PortFilesList_i->md5SUM << " "
		<< endl;
#endif
		FileDownload FD(url,dir,fileName,true);
		FD.downloadFile();
	}
}
set<string> Pkgrepo::getListOfPackagesFromCollection(const string& collectionName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	std::set<string> listOfPackages;

	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		string baseDir = basename(const_cast<char*>(i->Dir.c_str()));
		if ( baseDir == collectionName) {
			for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
				listOfPackages.insert(j->basePackageName);
			}
			return listOfPackages;
		}
	}
	return listOfPackages;
}
unsigned int Pkgrepo::getBinaryPackageList()
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	std::string packageNameVersion;
	std::set<string>  binaryList;
	// For each defined collection
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this collection
#ifndef NDEBUG
		cerr << i->Dir << " " << i->Url << endl;
#endif
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
#ifndef NDEBUG
			cerr << j->basePackageName << " " 
					<< j->description << " " 
					<< j->md5SUM << " " 
					<< j->version << " " 
					<< j->s_buildDate << " "
					<< j->extention << endl ;
#endif
			string baseDir = basename(const_cast<char*>(i->Dir.c_str()));
			packageNameVersion= "(" + baseDir + ") " +j->basePackageName + " " + j->version;
			binaryList.insert(packageNameVersion);
		}
	}
	for ( auto i : binaryList) cout << i << endl;
	return binaryList.size();
}
unsigned int Pkgrepo::getPortsList()
{
	if (!m_parseCollectionDirectory)
		parseCollectionDirectory();
	if (!m_parsePackagePkgfileFile)
		parsePackagePkgfileFile();


	unsigned int numberOfPorts = 0;
	// For each defined collection
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this collection
#ifndef NDEBUG
		cerr << i->Dir << " " << i->Url << endl;
#endif
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
#ifndef NDEBUG
			cerr << j->basePackageName << " "
				<< j->description << " "
				<< j->md5SUM << " "
				<< j->version << " "
				<< j->release << " "
				<< j->s_buildDate << " "
				<< j->extention << endl ;
#endif
			if (j->basePackageName.size() > 0) {
				string baseDir = basename(const_cast<char*>(i->Dir.c_str()));
				cout <<  i->Dir + "/" + j->basePackageName << " " << j->version << "-" << j->release << endl;
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
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		if (i->Url.size() > 0 )
			continue;
		// For each directory found in this collection
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
				found = true;
				cout << "Name           : " << portName << endl
					<< "Description    : " << j->description << endl
					<< "URL            : " << j->URL << endl
					<< "Version        : " << j->version << endl
					<< "Release        : " << j->release << endl
					<< "Maintainer     : " << j->maintainer << endl
					<< "Packager       : " << j->packager << endl
					<< "Date of Pkgfile: " << j->fileDate
					<< "Local Directory: " << i->Dir << endl;
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
				char * c_time_s = ctime(&j.buildDate);
				found = true;
				cout << "Name           : " << packageName << endl
					<< "Description    : " << j.description << endl
					<< "URL            : " << j.URL << endl
					<< "Version        : " << j.version << endl
					<< "Release        : " << j.release << endl
					<< "Maintainer     : " << j.maintainer << endl
					<< "Packager       : " << j.packager << endl
					<< "Build date     : " << c_time_s
					<< "Binary Url     : " << i.Url << endl
					<< "Local Directory: " << i.Dir << endl;
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

	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == packageName ) {
					return j->version;
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
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == basePortName ) {
				return j->basePackageName;
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

	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
				return j->version;
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
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == packageName ) {
				return j->release;
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
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
				return j->release;
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

	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == basePortName ) {
				return true;
			}
		}
	}
	return false;
}
bool Pkgrepo::checkBinaryExist(const string& packageName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos)
		basePackageName=packageName.substr(0,pos);
#ifndef NDEBUG
	cerr << "basePackageName: " << basePackageName << endl;
#endif
	bool baseBinaryfound = false;
	bool Binaryfound = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
#ifndef NDEBUG
		 cerr << m_PortsDirectory_i->Url << " " <<  m_PortsDirectory_i->Dir << endl;
#endif
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
#ifndef NDEBUG
			cerr << "m_BasePackageInfo_i->basePackageName: " << m_BasePackageInfo_i->basePackageName << ":" << m_BasePackageInfo_i->s_buildDate <<"." <<  endl;
#endif
			if ( m_BasePackageInfo_i->basePackageName == basePackageName ) {
				baseBinaryfound = true;
				break;
			}
		}
		if (baseBinaryfound)
					break;
	}
	if (baseBinaryfound) {
		if ( ! checkFileExist ( m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO")) {
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		}
		string pkgRepoFile= m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO";
		string pkgReporMD5sum = m_BasePackageInfo_i->md5SUM;
		if ( ! checkMD5sum( pkgRepoFile.c_str(), pkgReporMD5sum.c_str())) {
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		}
		parseCurrentPackagePkgRepoFile();
		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
#ifndef NDEBUG
			cerr << m_PortFilesList_i->name << endl;
#endif
			if (m_PortFilesList_i->name == packageName) {
				Binaryfound = true;
				break;
			}
		}
		if (Binaryfound)
			m_packageFileName = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/" + packageName + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i-> arch + m_BasePackageInfo_i->extention;
#ifndef NDEBUG
		cerr << packageName << " is " << m_packageFileName << endl;
#endif
	}
	return Binaryfound;
}
string Pkgrepo::getPackageFileName(const string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileName;
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
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == basePackageName ) {
				found = true;
				buildTime = j->buildDate;
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
bool Pkgrepo::search(const string& s)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	if (s.size() < 2) {
		cout << "your string is too short, min 2 characters" << endl;	
		return false;
	}
	bool found = false;
	set<string> packageList;
	string packageToInsert;
	std::string::size_type pos;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			string baseDir = basename(const_cast<char*>(i->Dir.c_str()));
			if ( convertToLowerCase(s) == j->basePackageName ) {
				packageToInsert = "(" + baseDir + ") " + j->basePackageName + " " + j->version + " " + j->description;
				packageList.insert(packageToInsert);
				found = true;
			}
			pos = j->basePackageName.find(convertToLowerCase(s));
			if (pos != std::string::npos) {
				packageToInsert = "(" + baseDir + ") " + j->basePackageName + " " + j->version + " " + j->description;
				packageList.insert(packageToInsert);
				found = true;
			}
			pos = convertToLowerCase(j->description).find(convertToLowerCase(s));
			if (pos != std::string::npos) {
				packageToInsert = "(" + baseDir + ") " + j->basePackageName + " " + j->version + " " + j->description;
				packageList.insert(packageToInsert);
				found = true;
			}
			pos = convertToLowerCase(j->version).find(convertToLowerCase(s));
			if (pos != std::string::npos) {
				packageToInsert = "(" + baseDir + ") " + j->basePackageName + " " + j->version + " " + j->description;
				packageList.insert(packageToInsert);
				found = true;
			}
		}
	}
	for (set<string>::const_iterator i = packageList.begin(); i != packageList.end(); ++i) {
		cout << *i << endl;
	}
	return found;
}
int Pkgrepo::parseConfig(const string& fileName)
{
	/*
		TODO This will need to be beter done
		I guess ones houaphan version  of NuTyX is release we can simply that part
	*/
	int result = 0;
	if (checkFileExist("/var/lib/pkg/nutyx-version") ) {
		result = getConfig("/var/lib/pkg/nutyx-version", m_config);
		if ( result != 0 )
			return result;
	}
	if ( m_config.name.size() == 0 ) {
		m_config.name="current";
	}
	if ( m_config.version.size() == 0 ) {
		m_config.version="current";
	}

	result = getConfig(fileName,m_config);
	if ( result != 0 )
		return result;

	std::string::size_type pos;

	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		DirUrl DU = *i ;
		if (DU.Url.size() == 0 )
			continue;

		string categoryDir, url;
		categoryDir = DU.Dir;

		string category = basename(const_cast<char*>(categoryDir.c_str()));

		pos = DU.Url.find (m_config.version);

		if (pos != std::string::npos) {
			url = DU.Url.substr(0,pos-1) + "/" + m_config.version + "/" + getMachineType()+ "/" + category;
#ifndef NDEBUG
			cerr << pos << endl;
#endif
		} else {
			 url = DU.Url + "/" + m_config.version + "/" + getMachineType()+ "/" + category;
		}
#ifndef NDEBUG
		cerr << url << endl;
#endif
		i->Url = url;
	}
	return result;
}
int Pkgrepo::parseConfig(const string& fileName, Config& config)
{
	/*
		TODO This will need to be beter done
		I guess ones houaphan version  of NuTyX is release we can simply that part
	*/
	int result = 0;
	if (checkFileExist("/var/lib/pkg/nutyx-version") ) {
		result = getConfig( "/var/lib/pkg/nutyx-version" , config);
		if ( result != 0 )
			return result;
	}
#ifndef NDEBUG
	cerr << config.name << endl;
	cerr << config.version << endl;
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

	for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end(); ++i) {
		DirUrl DU = *i ;
		if (DU.Url.size() == 0 )
			continue;

		string categoryDir, url;
		categoryDir = DU.Dir;

		string category = basename(const_cast<char*>(categoryDir.c_str()));

		pos = DU.Url.find (config.version);
		if (pos != std::string::npos) {
			url = DU.Url.substr(0,pos-1) + "/" + config.version + "/" + getMachineType()+ "/" + category;
#ifndef NDEBUG
			cerr << pos << endl;
#endif
		} else {
			url = DU.Url + "/" + config.version + "/" + getMachineType()+ "/" + category;
		}
#ifndef NDEBUG
		cerr << url << endl;
#endif
		i->Url = url;

	}
	return result;
}
// vim:set ts=2 :
