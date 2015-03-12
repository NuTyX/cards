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
int ConfigParser::parseMD5sumCategoryDirectory()
{
	parseConfig(m_configFileName);
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		if ( i->Url.size() == 0 )
			continue;
		PortsDirectory pD;
		FileList fL;
		pD.Dir = i->Dir;
		pD.Url = i->Url;
		
		string categoryMD5sumFile = i->Dir + "/MD5SUM" ;
		FileDownload MD5Sum(i->Url + "/MD5SUM",
			i->Dir,
			"MD5SUM", false);
		MD5Sum.downloadFile();
		
		vector<string> MD5SUMFileContent;

		if ( parseFile(MD5SUMFileContent,categoryMD5sumFile.c_str()) != 0) {
			cerr << "cannot read file MD5SUM" << endl;
			return -1;
		}
		remove(categoryMD5sumFile.c_str());
		for ( vector<string>::iterator i = MD5SUMFileContent.begin();i!=MD5SUMFileContent.end();++i) {
			string val = *i;
			if ( i->size() <  34 ) {
				cerr << "missing info" << endl;
				continue;
			}
			if ( val[32] != ':' ) {
				cerr << "wrong  info" << endl;
				continue;
			}
			string::size_type pos = val.find(':');
			if (pos != string::npos) {
				fL.md5SUM = stripWhiteSpace(val.substr(0,pos));
			}
			string tmpName = val.substr(pos+1);
			pos = tmpName.find('@');
			if (pos != string::npos) {
				fL.basePackageName = stripWhiteSpace(tmpName.substr(0,pos));
				fL.version = stripWhiteSpace(tmpName.substr(pos+1));
			} else {
				fL.basePackageName = stripWhiteSpace(tmpName);
				fL.version = "";
			}
			pD.basePackageList.push_back(fL);
		}
		m_packageList.push_back(pD);
	}
#ifndef NDEBUG
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		PortsDirectory pD = *i;
		for (std::vector<FileList>::iterator j = pD.basePackageList.begin(); j != pD.basePackageList.end();++j) {
			cerr << i->Url << " "<< i->Dir << " " << j->basePackageName  << " " << j->version << " "  << j->md5SUM << endl;
		}
	}
#endif
	return 0;
}
int ConfigParser::parseCategoryDirectory()
{
	parseConfig(m_configFileName);
	for (vector<DirUrl>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		PortsDirectory pD;
		FileList fL;
		pD.Dir = i->Dir;
		set<string> localPackagesList;
		string::size_type pos;
		if ( findFile( localPackagesList, pD.Dir) != 0 ) {
			cerr << "cannot read directory "  << pD.Dir <<endl;
			return -1;
		}
		
		for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); ++li) {
			pos = li->find('@');
			if (pos != string::npos) {
				fL.basePackageName = li->substr(0,pos);
				fL.version = li->substr(pos+1);
			} else {
				fL.basePackageName = *li;
				fL.version = "";
			}
			pD.basePackageList.push_back(fL);
		}
		m_packageList.push_back(pD);
		localPackagesList.clear();
	}
	return 0;
}
int ConfigParser::parsePortsList()
{
	InfoFile downloadFile;
	vector<InfoFile> downloadFilesList;

	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			/*
				We should check if the MD5SUM of the port is available
				MD5SUMFile is /var/lib/pkg/saravane/server/alsa-lib/MD5SUM
			*/
			if ( ! checkFileExist(i->Dir + "/" + j->basePackageName  + "/MD5SUM") ) {
				downloadFile.url = i->Url + "/" + j->basePackageName  + "/MD5SUM";
				downloadFile.dirname = i->Dir + "/" + j->basePackageName;
				downloadFile.filename = "/MD5SUM";
				downloadFile.md5sum = j-> md5SUM;
				downloadFilesList.push_back(downloadFile);
#ifndef NDEBUG
				cerr << i->Dir + "/" + j->basePackageName  << endl;
#endif
			}
		}
	}
	if ( downloadFilesList.size() > 0 ) {
		FileDownload FD(downloadFilesList,false);
	}
	return 0;
}
set<string> ConfigParser::getListOfPackagesFromDirectory(const string& path)
{
	set<string> listOfPackages; 
	// For each category activate in cards.conf
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		// For each directory found in this category
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( path != i->Dir + "/" + j->basePackageName)
				continue;
			// If we are dealing with the correct path ...
			for (std::vector<PackageFilesList>::iterator p = j->packageFilesList.begin(); p != j ->packageFilesList.end();++p) {
				if ( ( p->arch == "any" ) || ( p->arch == m_config.arch) )
					listOfPackages.insert(p->name);
			}
		}
	}
	return listOfPackages;	
}
int ConfigParser::parseBasePackageList()
{
/*
 From here we can check if the binaries are available
 We have to parse the file 
 /var/lib/pkg/saravane/server/alsa-lib/MD5SUM
*/
	// For each category activate in cards.conf
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
#ifndef NDEBUG
		cerr << i->Dir << endl;
#endif
		// For each directory found in this category
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			// MD5SUMFile = /var/lib/pkg/saravane/server/alsa-lib/MD5SUM
			string MD5SUMFile = i->Dir + "/" + j->basePackageName  + "/MD5SUM";
			vector<string> MD5SUMFileContent;
			if ( parseFile(MD5SUMFileContent,MD5SUMFile.c_str()) != 0) {
				cerr << "Cannot read the file: " << MD5SUMFile << endl;
				cerr << " ... continue with next" << endl;
				continue;
			}
			j->buildDate = 0;
			j->extention = "";
			PackageFilesList PFL;
			for (vector<string>::const_iterator i = MD5SUMFileContent.begin();i != MD5SUMFileContent.end(); ++i) {
				string input = *i;
				if (input.size() < 11) {
					cerr << "[" << input << "]: Wrong format field to small" << endl;
					continue;
				}
				if ( input[10] != ':' ) {	// It's not the first line ...
					PFL.md5SUM = input.substr(0,32);
					string fileNameArch = input.substr(33);
					string::size_type pos = fileNameArch.find(':');
					if ( pos != std::string::npos) { // format is: md5sum:name:arch
						PFL.name = input.substr(33,pos);
						PFL.arch = input.substr(pos+34);
					} else {
						PFL.arch = "";
						PFL.name = input.substr(33); // format is: md5sum:name means not a binary
					}
#ifndef NDEBUG
					cerr << j->basePackageName << ": "<< PFL.md5SUM << " " << PFL.name << " " << PFL.arch << endl;
#endif
				}

				/*
				 * Let's check if the first line is something like:
				 * 1401638336:.cards.tar.xz:4.14.1
				*/
				if (input[10] == ':' ) {	// There is a chance to find what we are looking for
					j->buildDate = strtoul(input.substr(0,10).c_str(),NULL,0);
					string extension_version = input.substr(11,input.size());
					string::size_type pos = extension_version.find(':');
					if ( pos != std::string::npos) {
						j->extention = extension_version.substr(0,pos);
						j->version = extension_version.substr(pos+1);
					} else {
						j->extention = input.substr(11,input.size());
						j->version = "N.A.";
					}
#ifndef NDEBUG
					cerr << j->basePackageName << ": " << j->buildDate << " " << j->extention << endl;
#endif
				}
				j->packageFilesList.push_back(PFL);
			}
		}
	}
	return 0;
}
set<string> ConfigParser::getListOutOfDate()
{
	set<string> listOfPackages;
	// For each category activate in cards.conf
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		// For each directory found in this category
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->extention == "" ) {
				listOfPackages.insert(j->basePackageName);
			}
		}
	}
	return listOfPackages;
}
int ConfigParser::parsePackagePkgfileList()
{
	InfoFile downloadFile;
	vector<InfoFile> downloadFilesList;
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PackageFilesList>::iterator p = j->packageFilesList.begin(); p != j ->packageFilesList.end();++p) {
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
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
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
						}
					}
				}
			}
		} 
	}
	return 0;
}
void ConfigParser::downloadPackageFileName(const std::string& packageName)
{
	if ( m_p->name == packageName) {
#ifndef NDEBUG
		cerr << packageName << endl;
#endif
		string build = static_cast<ostringstream*>( &(ostringstream() <<  m_j->buildDate ))->str();
		string url = m_i->Url + "/" + m_j->basePackageName + "/" + m_p->name + build + m_p-> arch + m_j->extention;
		string dir = m_i->Dir + "/" + m_j->basePackageName;
		string fileName = m_p->name + build + m_p-> arch + m_j->extention;
#ifndef NDEBUG
		cerr << url << " " 
		<< dir << " " 
		<< fileName << " "
		<< m_p->md5SUM << " "
		<< endl;
#endif
		//TODO Check MD5SUM
		FileDownload FD(url,dir,fileName,true);
		FD.downloadFile();
	}
}
unsigned int ConfigParser::getBinaryPackageList()
{
	unsigned int numberOfBinaries = 0;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		// For each directory found in this category
#ifndef NDEBUG
		cerr << i->Dir << " " << i->Url << endl;
#endif
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
#ifndef NDEBUG
			cerr << j->basePackageName << " " 
					<< j->description << " " 
					<< j->md5SUM << " " 
					<< j->version << " " 
					<< j->buildDate << " " 
					<< j->extention << endl ;
#endif
			// For each package found 
			for (std::vector<PackageFilesList>::iterator p = j->packageFilesList.begin(); p != j ->packageFilesList.end();++p) {
#ifndef NDEBUG
				cerr << p->md5SUM << " " << p->name << " " << p->arch << endl << endl;
#endif
				if ( p->arch != "") {
					numberOfBinaries++;
					cout << p->name << " " << j->version << endl;
				}
			}	
		}
	}
	return numberOfBinaries;
}
unsigned int ConfigParser::getPortsList()
{
	unsigned int numberOfPorts = 0;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		// For each directory found in this category
#ifndef NDEBUG
		cerr << i->Dir << " " << i->Url << endl;
#endif
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
#ifndef NDEBUG
			cerr << j->basePackageName << " "
				<< j->description << " "
				<< j->md5SUM << " "
				<< j->version << " "
				<< j->buildDate << " "
				<< j->extention << endl ;
#endif
			cout << j->basePackageName << " " << j->version << endl;
			numberOfPorts++;
		}
	}
	return numberOfPorts;
}
bool ConfigParser::getPortInfo(const string& portName)
{
	bool found = false;
	// For each defined category
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		// For each directory found in this category
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
				found = true;
				cout << "Name           : " << portName << endl
					<< "Description    : " << j->description << endl
					<< "URL            : " << j->URL << endl
					<< "Version        : " << j->version << endl
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
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		// For each directory found in this category
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PackageFilesList>::iterator p = j->packageFilesList.begin(); p != j ->packageFilesList.end();++p) {	
				if ( p->name == packageName ) {
					char * c_time_s = ctime(&j->buildDate);
					found = true;
					cout << "Name           : " << packageName << endl
						<< "Description    : " << j->description << endl
						<< "Version        : " << j->version << endl
						<< "Build date     : " << c_time_s
						<< "Arch           : " << p->arch << endl
						<< "Url            : " << i->Url << endl
						<< "Local Directory: " << i->Dir << endl;
				}
			}
		}
	}
	return found;
}
string ConfigParser::getPortDir (const std::string& portName)
{
	string portDir = "";
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
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
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PackageFilesList>::iterator k = j->packageFilesList.begin(); k != j ->packageFilesList.end();++k) {
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
	string version = "";
	bool found = false;
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			if ( j->basePackageName == portName ) {
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
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
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
	bool found = false;
	for (m_i = m_packageList.begin();m_i !=  m_packageList.end();++m_i) {
		for (m_j = m_i->basePackageList.begin(); m_j != m_i->basePackageList.end();++m_j) {
			for (m_p = m_j->packageFilesList.begin(); m_p != m_j ->packageFilesList.end();++m_p) {
				if ( m_p->name == packageName ) {
					m_packageFileName =  m_i ->Dir + "/" + m_j->basePackageName + "/" + m_p->name 
					+ static_cast<ostringstream*>( &(ostringstream() <<  m_j->buildDate ))->str() + m_p-> arch + m_j->extention;
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
	return found;
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
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
			for (std::vector<PackageFilesList>::iterator p = j->packageFilesList.begin(); p != j ->packageFilesList.end();++p) {
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
	for (std::vector<PortsDirectory>::iterator i = m_packageList.begin();i !=  m_packageList.end();++i) {
		for (std::vector<FileList>::iterator j = i->basePackageList.begin(); j != i->basePackageList.end();++j) {
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
