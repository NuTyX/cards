// cards_install.cc
//
// Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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
#include "archive_utils.h"
#include "cards_install.h"

CardsInstall::CardsInstall(const CardsArgumentParser& argParser)
	: m_argParser(argParser)
{
}
void CardsInstall::run(int argc, char** argv)
{
	
	m_packageName = m_argParser.otherArguments()[0];
	ConfigParser::parseConfig("/etc/cards.conf", m_config);
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	getListOfPackages("/"); //TODO need to adjust o_root ...
	getInstalledPackages(false);
	string url,categoryDir,categoryMD5sumFile, remotePackageDirectory ;
	cout << "Synchronizing start ..." << endl;
	for (vector<string>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		string val = *i ;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			categoryDir = stripWhiteSpace(val.substr(0,pos));
			url = stripWhiteSpace(val.substr(pos+1));
			categoryMD5sumFile = categoryDir + "/MD5SUM" ;
			// Download the MD5SUM file of the category silently
			FileDownload MD5Sum(url + "/MD5SUM",
				categoryDir,
				"MD5SUM", false);
			MD5Sum.downloadFile();
			vector<string> MD5SUMFileContent;
			if ( parseFile(MD5SUMFileContent,categoryMD5sumFile.c_str()) != 0) {
				m_actualError = CANNOT_READ_FILE;
				treatErrors(categoryMD5sumFile);
			}
			for ( vector<string>::iterator i = MD5SUMFileContent.begin();i!=MD5SUMFileContent.end();++i) {
				m_MD5packagesNameVersionList.push_back(val + "|" + *i);
			}
		} else {
			continue;
		}
		remove(categoryMD5sumFile.c_str());
	}
	cout << "Synchronizing done." << endl;
#ifndef NDEBUG
	for (vector<string>::iterator i = m_MD5packagesNameVersionList.begin();i!=m_MD5packagesNameVersionList.end();++i) {
		cerr << *i << endl ;
	}
	cerr << "Number of Packages: " << MD5packagesNameVersionList.size() << endl;
#endif
}
vector<string> CardsInstall::getDirectDependencies()
{

	vector<string> packageNameDeps;

	if ( checkPackageNameExist(m_packageName)) {
		packageNameDeps.push_back(m_packageName);
		cout << m_packageName << " allready installed" << endl;
		return packageNameDeps;
	}
	if ( getPackageFileName()) {
		vector<string> packageNameBuildNDeps;
		ArchiveUtils packageArchive(m_packageFileName);
		packageNameBuildNDeps = packageArchive.listofDependencies();
		for (std::vector<string>::iterator it = packageNameBuildNDeps.begin();it != packageNameBuildNDeps.end();it++) {
			string Name = *it;
			packageNameDeps.push_back(Name.substr(0,Name.size()-10));
		}
	} else {
		/*
			We consider that this package doens't have any deps 
			Just add his name
		*/
		packageNameDeps.push_back(m_packageName);
	}
	return packageNameDeps;
}
void CardsInstall::printDependenciesList()
{
	getDependencies();
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		cout << *it << endl;
	}
}
void CardsInstall::getDependencies()
{
	vector<string> dependenciesWeMustAdd, 
		packageDepschecked,
		depencenciestoSort; 
	// Insert the final package first
	dependenciesWeMustAdd.push_back(m_packageName);
	std::vector<string>::iterator it;
	while ( ! dependenciesWeMustAdd.empty() ) {
		it = dependenciesWeMustAdd.begin();
		dependenciesWeMustAdd.erase(it);
		bool found = false;
		m_packageName = *it;
		for ( std::vector<string>::iterator i = packageDepschecked.begin(); i != packageDepschecked.end();i++) {
			if (m_packageName == *i) {
#ifndef NDEBUG
				cerr << "allready check: " << *i << endl;
#endif
				found = true;
				break;
			}
		}
		if (! found ) {
			packageDepschecked.push_back(m_packageName);
			vector<string> directDependencies = getDirectDependencies();
			for ( it = directDependencies.begin(); it != directDependencies.end();it++) {
				depencenciestoSort.push_back(*it);
			}
			it = directDependencies.begin();
			directDependencies.erase(it);
			if ( ! directDependencies.empty() ) {
				for ( it = directDependencies.begin(); it != directDependencies.end();it++) {
					dependenciesWeMustAdd.push_back(*it);
				}
			}
		}
	}
	std::vector<string>::reverse_iterator rit;
	bool found = false ;
	for ( rit = depencenciestoSort.rbegin(); rit != depencenciestoSort.rend();++rit) {
		found = false ;
		for ( it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
			if ( *rit == *it ) {
				found = true ;
				break;
			}
		}
		if (!found)
			m_dependenciesList.push_back(*rit);
	}
}
bool CardsInstall::getPackageFileName()
{
	string basePackageName, packageFileName, categoryDir, url, PackageDirectory; 	
	string::size_type pos = m_packageName.rfind('.');
	bool found = false;

	if (pos != string::npos) {
		basePackageName=m_packageName.substr(0,pos);
	} else {
		basePackageName=m_packageName;
	}

	for (vector<string>::iterator i = m_MD5packagesNameVersionList.begin();i != m_MD5packagesNameVersionList.end(); ++i) {
		string val = *i ;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			categoryDir = stripWhiteSpace(val.substr(0,pos));
#ifndef NDEBUG
			cerr << categoryDir << endl;
#endif
		}
		string tmpUrl = val.substr(pos+1);
		pos = tmpUrl.find('|');
		if (pos != string::npos) {
			url = stripWhiteSpace(tmpUrl.substr(0,pos));
#ifndef NDEBUG
			cout << url << endl;
#endif
		}
		PackageDirectory = tmpUrl.substr(pos+1);
		if ( PackageDirectory.size() < 34 ) {
			m_actualError = CANNOT_PARSE_FILE;
			treatErrors(PackageDirectory + ": missing info");
		}
		if ( PackageDirectory[32] != ':' ) {
			m_actualError = CANNOT_PARSE_FILE;
			treatErrors(PackageDirectory + ": wrong format");
		}
		pos = PackageDirectory.find('@');
		if (pos != string::npos) {
			string packageName = PackageDirectory.substr(33,pos - 33);
			if ( basePackageName == packageName ) {
				found = true;
				break;
			}
		}
	}
	if (found) {
	/*
		If found, the port should exist
		We should check if the binary package is available
		MD5SUMFile is /var/lib/pkg/saravane/server/alsa-lib@1.2.3.4/MD5SUM
	*/
		string packageDirectoryName = categoryDir + "/" + PackageDirectory.substr(33)+"/";
		string MD5SUMFile = packageDirectoryName + "/MD5SUM";
		if ( ! checkFileExist(MD5SUMFile) ) {
			FileDownload MD5Sum(url + "/" + PackageDirectory.substr(33)+"/MD5SUM",
				categoryDir+ "/" + PackageDirectory.substr(33)+"/",
				"MD5SUM", false);
			MD5Sum.downloadFile();
		}
	/*
		We have the /var/lib/pkg/saravane/server/alsa-lib@1.2.3.4/MD5SUM file 
		Need to check if the binary is available
	*/
		bool found = false;
		vector<string> MD5SUMFileContent;
		if ( parseFile(MD5SUMFileContent,MD5SUMFile.c_str()) != 0) {
			m_actualError = CANNOT_READ_FILE;
			treatErrors(MD5SUMFile);
		}
		string packageExtension = "";
		string packageBuildDate = "";
		for (vector<string>::const_iterator i = MD5SUMFileContent.begin();i != MD5SUMFileContent.end(); ++i) {
			string input = *i;
			if (input.size() < 11) {
				cout << "Wrong Format, field to small: " << input << endl;
				continue;
			}
			if (input[10] == ':' ) {	// There is a chance to find what we are looking for
				packageBuildDate = input.substr(0,10);
				packageExtension = input.substr(11,input.size());
				found = true;
				continue;
			}
			if (found) {
		/*
			They are some binaries, the one I need exist ?
		*/
				string packageNameMD5SUM = input.substr(0,32);
				string fileNameArch = input.substr(33);
				string::size_type pos = fileNameArch.find(':');
				if ( pos != std::string::npos) { // format input OK (md5sum:name:arch)
					string myFileName;
					string fileName = input.substr(33,pos) +
						packageBuildDate +
						input.substr(34+pos) +
						packageExtension;
					if ( m_packageName == basePackageName ) {
					/*
						package we are looking for
						IS the base package means it's
						arch is define in /etc/cards.conf
					*/
						myFileName = m_packageName +
							packageBuildDate +
							m_config.arch +
							packageExtension;
							
					} else {
					/*
						package we are loocking for
						IS a sub package with "any" as arch
					*/
						myFileName = input.substr(33,pos) +
							packageBuildDate +
							"any" +
							packageExtension;
					}
					if ( myFileName == fileName ) {	// Finally that's the one we need
						m_packageFileName = packageDirectoryName + fileName;
						if (! checkFileExist(m_packageFileName)) {
						// Ok need to download it
							FileDownload packageFileNameArchive(url + "/" +
								PackageDirectory.substr(33) + "/" +
								fileName,
								categoryDir+ "/" + PackageDirectory.substr(33)+"/",
								fileName,true);
							packageFileNameArchive.downloadFile();
						}
						// YEEESSS we got it
						return true;
					}
				}
			}	
		}
		/*
			if we arrive here it means that they was no binaries found yet
			So we assume the package is exist but has no runtime dependencies
			which can be the case if allready install and/or from the basic
			system
		*/
			cerr << "WARNING " << m_packageName << " not yet compiled... " << endl;
		return false;
	} else {
		/*
			not found in any category this package name don't exist yet
			TODO Need to make it clear for the user, at the moment just return false
		 throwing an error and interrupt the process is a good idea ?
		*/
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(m_packageName);
		return false;
	}
}
void CardsInstall::install()
{
	getDependencies();
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		if	( checkPackageNameExist(*it)) {
			cout << *it << " is allready installed, will not be reinstalled" << endl;
			continue;
		}
	}
}
void CardsInstall::printHelp() const
{
	cout << endl
		<< "  You should make shure you have activate the available directories" << endl
		<< " containing the available binaries otherwise you will end up with" << endl
		<< " missing binaries. As a result your NuTyX wont be working as it" << endl
		<< " should." << endl << endl
		<< "  Be also aware that the install scenario will never try to compile" << endl
		<< " any port on it's own, this is not the same scenario." << endl << endl
		<< "usage: cards install <package name>" << endl
		<< "options:" << endl
		<< "   -H, --info       print this help and exit" << endl;
}
// vim:set ts=2 :
