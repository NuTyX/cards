//  config_parser.cc
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

#include "config_parser.h"
#include "error_treat.h"

using namespace std;
ConfigParser::ConfigParser(const std::string& fileName)
	: m_configFileName(fileName)
{
}
void ConfigParser::parsePkgRepoCategoryDirectory()
{
	parseConfig(m_configFileName);
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		if ( i->Url.size() == 0 )
			continue;
		PortsDirectory portsDirectory;
		portsDirectory.Dir = i->Dir;
		portsDirectory.Url = i->Url;
		BasePackageInfo basePkgInfo;
		string categoryPkgRepoFile = i->Dir + "/.PKGREPO" ;
		if ( ! checkFileExist(categoryPkgRepoFile)) {
			FileDownload PkgRepoFile(i->Url + "/.PKGREPO",
				i->Dir,
				".PKGREPO", false);
			PkgRepoFile.downloadFile();
		}
		vector<string> PkgRepoFileContent;

		if ( parseFile(PkgRepoFileContent,categoryPkgRepoFile.c_str()) != 0) {
			cerr << "cannot read file PKGREPO" << endl;
			continue;
		}

		for ( vector<string>::iterator i = PkgRepoFileContent.begin();i!=PkgRepoFileContent.end();++i) {
			string input = *i;
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
					basePkgInfo.release = infos[4];
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
			cerr << i->Url << " "<< i->Dir << " " << j->basePackageName  << " " << j->version << " "  << j->md5SUM << endl;
		}
	}
#endif
}
void ConfigParser::parseCategoryDirectory()
{
	parseConfig(m_configFileName);
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		// We don't want to check the folders which can sync with any mirror
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
}
void ConfigParser::downloadPortsPkgRepo(const string& packageName)
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
set<string> ConfigParser::getListOfPackagesFromDirectory(const string& path)
{
	set<string> listOfPackages; 
	// For each category activate in cards.conf
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this category
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( path != i->Dir + "/" + j->basePackageName)
				continue;
			// If we are dealing with the correct path ...
			for (std::vector<PortFilesList>::iterator p = j->portFilesList.begin(); p != j ->portFilesList.end();++p) {
#ifndef NDEBUG
				cerr << p->md5SUM << "|" << p->name << "|" << p->arch   << endl;
#endif
				if ( ( p->arch == "any" ) || ( p->arch == m_config.arch) )
					listOfPackages.insert(p->name);
			}
		}
	}
	return listOfPackages;	
}
void ConfigParser::parseBasePackageList()
{
/*
 From here we can check whats is available in the port directory
 It can be the Pkgfile, the binary, the post-install, etc
 We have to parse the file 
 /var/lib/pkg/saravane/server/alsa-lib/.PKGREPO
 .PKGREPO file = /var/lib/pkg/saravane/server/alsa-lib/.PKGREPO
*/
	string PkgRepoFile = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName  + "/.PKGREPO";
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
			if ( infos.size() > 2 ) {
				if ( infos[2].size() > 0 ) {
					portFilesList.arch = infos[2];
				} else  {
				// If there are only 2 fields then it's not a binary go on with next one
					continue;
				}
			}
#ifndef NDEBUG
			cerr << m_BasePackageInfo_i->basePackageName << ": " << portFilesList.md5SUM << " " << portFilesList.name << " " << portFilesList.arch << endl;
#endif
		}

		/*
		 * Let's check if the first line is something like:
		 * 1401638336#.cards.tar.xz#4.14.1#..#..#..
		*/
		if (input[10] == '#' ) {	// We already have this info let's make shure it's still ok
			vector<string> infos;
			split( input, '#', infos, 0,true);
			if (m_BasePackageInfo_i->s_buildDate != infos[0] ) {
				cerr << m_BasePackageInfo_i->s_buildDate << " != " << infos[0] << endl;
			}
			if (m_BasePackageInfo_i->extention != infos[1]) {
				cerr << m_BasePackageInfo_i->extention << " != " << infos[1] << endl;
			}
			if (m_BasePackageInfo_i->version != infos[2] ) {
				cerr << m_BasePackageInfo_i->version << " != " << infos[2] << endl;
			}
#ifndef NDEBUG
			cerr << m_BasePackageInfo_i->basePackageName << ": " << m_BasePackageInfo_i->buildDate << " " << m_BasePackageInfo_i->extention << endl;
#endif
			continue;
		}
		m_portFilesList.push_back(portFilesList);
	}
}
void ConfigParser::parseBasePackageList(const std::string& packageName)
{
	bool found=false;
	vector<string> infos;
	// For each category activate in cards.conf
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
#ifndef NDEBUG
		cerr << m_PortsDirectory_i->Dir << endl;
#endif
		for ( m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			if ( m_BasePackageInfo_i->basePackageName == packageName ) {
				found=true;
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
		if (found)
			break;
	}
}
set<string> ConfigParser::getListOutOfDate()
{
	set<string> listOfPackages;
	// For each category activate in cards.conf
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this category
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->extention == "" ) {
				listOfPackages.insert(j->basePackageName);
			}
		}
	}
	return listOfPackages;
}
void ConfigParser::parsePackagePkgfileList()
{
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
			j->fileDate = modifyTimeFile(pkgFile);
			vector<string> pkgFileContent;
			if ( parseFile(pkgFileContent,pkgFile.c_str()) != 0) {
				cerr << "Cannot read the file: " << pkgFile << endl;
				cerr << " ... continue with next" << endl;
				continue;
			}
			for (vector<string>::const_iterator p = pkgFileContent.begin();p != pkgFileContent.end();++p) {
				string line = stripWhiteSpace(*p);
				if ( line.substr( 0, 8 ) == "version=" ){
					j->version = getValueBefore( getValue( line, '=' ), '#' );
					j->version = stripWhiteSpace( j->version );
				} else if ( line.substr( 0, 8 ) == "release=" ){
					j->release = getValueBefore( getValue( line, '=' ), '#' );
					j->release = stripWhiteSpace( j->release );
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
}
void ConfigParser::downloadPackageFileName(const std::string& packageName)
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
		parseBasePackageList();

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
unsigned int ConfigParser::getBinaryPackageList()
{
	unsigned int numberOfBinaries = 0;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this category
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
			numberOfBinaries++;
			cout << j->basePackageName << " " << j->version << endl;	
		}
	}
	return numberOfBinaries;
}
unsigned int ConfigParser::getPortsList()
{
	unsigned int numberOfPorts = 0;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this category
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
			cout << j->basePackageName << " " << j->version <<  endl;
			numberOfPorts++;
		}
	}
	return numberOfPorts;
}
bool ConfigParser::getPortInfo(const string& portName)
{
	bool found = false;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		if (i->Url.size() > 0 )
			continue;
		// For each directory found in this category
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
bool ConfigParser::getBinaryPackageInfo(const string& packageName)
{
	bool found = false;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		// For each directory found in this category
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == packageName ) {
				char * c_time_s = ctime(&j->buildDate);
				found = true;
				cout << "Name           : " << packageName << endl
					<< "Description    : " << j->description << endl
					<< "Version        : " << j->version << endl
					<< "Build date     : " << c_time_s
					<< "Url            : " << i->Url << endl
					<< "Local Directory: " << i->Dir << endl;
				break;
			}
		}
		if (found)
			break;
	}
	return found;
}
string ConfigParser::getPortDir (const std::string& portName)
{
	string portDir = "";
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
				portDir = i->Dir + "/" + j->basePackageName;
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	return portDir;
}
string ConfigParser::getBasePackageName(const string& packageName)
{
	string basePackageName = "";
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PortFilesList>::iterator k = j->portFilesList.begin(); k != j ->portFilesList.end();++k) {
				if ( k->name == packageName ) {
					basePackageName = j->basePackageName;
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
		if (found)
			break;
	}
	return basePackageName;
}

string ConfigParser::getPortVersion (const string& portName)
{
  string basePortName = portName;
  string::size_type pos = portName.find('.');
  if (pos != string::npos) {
    basePortName=portName.substr(0,pos);
  }

	string version = "";
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == basePortName ) {
				found = true;
				version = j->version;
				break;
			}
		}
		if (found)
			break;
	}
	return version;
}
bool ConfigParser::checkPortExist(const string& portName)
{
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	return found;
}
bool ConfigParser::checkBinaryExist(const string& packageName)
{

	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos) {
		basePackageName=packageName.substr(0,pos);
	}

	bool baseBinaryfound = false;
	bool Binaryfound = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			if ( m_BasePackageInfo_i->basePackageName == basePackageName ) {
				baseBinaryfound = true;
				break;
			}
		}
		if (baseBinaryfound)
					break;
	}
	if (baseBinaryfound) {
		if ( ! checkFileExist ( m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO"))
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		parseBasePackageList();
		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
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
string ConfigParser::getPackageFileName(const string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileName;
}
time_t ConfigParser::getBinaryBuildTime (const string& packageName)
{
	time_t buildTime = 0;
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PortFilesList>::iterator p = j->portFilesList.begin(); p != j ->portFilesList.end();++p) {
				if ( p->name == packageName ) {
					found = true;
					buildTime = j->buildDate;
					break;
				}
			}
			if (found)
				break;
		}
		if (found)
			break;
	}
	return buildTime;
}
bool ConfigParser::search(const string& s)
{
	bool found = false;
	if (s.size() < 2) {
		cout << "your string is too short, min 2 characters" << endl;	
		return false;
	}
	set<string> packageList;
	string packageToInsert;
	std::string::size_type pos;
	for (std::vector<PortsDirectory>::iterator i = m_portsDirectoryList.begin();i !=  m_portsDirectoryList.end();++i) {
		for (std::vector<BasePackageInfo>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( convertToLowerCase(s) == j->basePackageName ) {
				packageToInsert = j->basePackageName + " " + j->version + " " + j->description;
				packageList.insert(packageToInsert);
				found = true;
				break;
			}
			pos = j->basePackageName.find(convertToLowerCase(s));
			if (pos != std::string::npos) {
				packageToInsert = j->basePackageName + " " + j->version +" " + j->description;
				packageList.insert(packageToInsert);
				found = true;
				break;
			}
			pos = convertToLowerCase(j->description).find(convertToLowerCase(s));
			if (pos != std::string::npos) {
				packageToInsert = j->basePackageName + " " + j->version +" " + j->description;
				packageList.insert(packageToInsert);
				found = true;
				break;
			}
		}
	}
	for (set<string>::const_iterator i = packageList.begin(); i != packageList.end(); ++i) {
		cout << *i << endl;
	}
	return found;
}
int ConfigParser::parseConfig(const string& fileName)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if (!fp) {
		return -1;
	}

	char line[512];
	string s;
	while (fgets(line, 512, fp)) {
		if (line[strlen(line)-1] == '\n' ) {
			line[strlen(line)-1] = '\0';
		}
		s = line;

		// strip comments
		string::size_type pos = s.find('#');
		if (pos != string::npos) {
			s = s.substr(0,pos);
		}

		// whitespace separates
		pos = s.find(' ');
		if (pos == string::npos) {
			// try with a tab
			pos = s.find('\t');
		}
		if (pos != string::npos) {
			string key = s.substr(0, pos);
			string val = stripWhiteSpace(s.substr(pos));

			if (key == "dir") {
				string::size_type pos = val.find('|');
				DirUrl DU;
				if (pos != string::npos) {
					DU.Dir = stripWhiteSpace(val.substr(0,pos));
					DU.Url = stripWhiteSpace(val.substr(pos+1));
				} else{
					DU.Dir = stripWhiteSpace(val);
					DU.Url = "";
				}
				m_config.dirUrl.push_back(DU) ;
			}
			if (key == "arch") {
				if (val != "i686") {
					m_config.arch="x86_64";
				} else {
					m_config.arch="i686";
				}
			}
			if (key == "logdir") {
				m_config.logdir = val;
			}
			if (key == "locale") {
				m_config.locale.push_back(val);
			}
			if (key == "base") {
				m_config.baseDir.push_back(val);
			}
		}
	}
	fclose(fp);
	return 0;
}
int ConfigParser::parseConfig(const string& fileName, Config& config)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if (!fp) {
		return -1;
	}

	char line[512];
	string s;
	while (fgets(line, 512, fp)) {
		if (line[strlen(line)-1] == '\n' ) {
			line[strlen(line)-1] = '\0';
		}
		s = line;

		// strip comments
		string::size_type pos = s.find('#');
		if (pos != string::npos) {
			s = s.substr(0,pos);
		}

		// whitespace separates
		pos = s.find(' ');
		if (pos == string::npos) {
			// try with a tab
			pos = s.find('\t');
		}
		if (pos != string::npos) {
			string key = s.substr(0, pos);
			string val = stripWhiteSpace(s.substr(pos));

			if (key == "dir") {
					string::size_type pos = val.find('|');
					DirUrl DU;
					if (pos != string::npos) {
							DU.Dir = stripWhiteSpace(val.substr(0,pos));
							DU.Url = stripWhiteSpace(val.substr(pos+1));
					} else {
							DU.Dir = stripWhiteSpace(val);
							DU.Url = "";
					}					
					config.dirUrl.push_back(DU);
			}
			if (key == "logdir") {
					config.logdir = val;
			}
			if (key == "arch") {
				if (val != "i686") {
					config.arch="x86_64";
				} else {
					config.arch="i686";
				}
			}
			if (key == "locale") {
				config.locale.push_back(val);
			}
			if (key == "base") {
				config.baseDir.push_back(val);
			}
		}
	}
	fclose(fp);
	return 0;
}
// vim:set ts=2 :
