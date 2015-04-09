// cards_info.cc
//
// Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
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

#include <iomanip>
#include <sstream>

#include "cards_info.h"
#include "cards_depends.h"

using namespace std;

CardsInfo::CardsInfo(const CardsArgumentParser& argParser)
	:  m_argParser(argParser), m_root("/")
{
}
void CardsInfo::run(int argc, char** argv)
{
	getListOfPackageNames(m_root);
	buildDatabaseWithNameVersion();
}
void CardsInfo::getOwner()
{
	getListOfPackageNames(m_root);
	buildDatabaseWithDetailsInfos(false);

	string sfile = m_argParser.otherArguments()[0];
	regex_t preg;
	if (regcomp(&preg, sfile.c_str(), REG_EXTENDED | REG_NOSUB)) {
		m_actualError = CANNOT_COMPILE_REGULAR_EXPRESSION;
		treatErrors(sfile);
	}
	vector<pair<string, string> > result;
	result.push_back(pair<string, string>("Package", "File"));
	unsigned int width = result.begin()->first.length(); // Width of "Package"
#ifndef NDEBUG
	cerr << sfile << endl;
#endif
	for (packages_t::const_iterator i = m_listOfInstPackages.begin(); i != m_listOfInstPackages.end(); ++i) {
		for (set<string>::const_iterator j = i->second.files.begin(); j != i->second.files.end(); ++j) {
			const string file('/' + *j);
			if (!regexec(&preg, file.c_str(), 0, 0, 0)) {
				result.push_back(pair<string, string>(i->first, *j));
				if (i->first.length() > width) {
					width = i->first.length();
				}
			}
		}
	}
	regfree(&preg);
	if (result.size() > 1) {
		for (vector<pair<string, string> >::const_iterator i = result.begin(); i != result.end(); ++i) {
			cout << left << setw(width + 2) << i->first << i->second << endl;
		}
	} else {
			cout << m_utilName << ": no owner(s) found" << endl;
	}
}
void CardsInfo::listOfFiles()
{
	getListOfPackageNames(m_root);
	buildDatabaseWithDetailsInfos(false);
	if (checkPackageNameExist(m_argParser.otherArguments()[0])) {
		copy(m_listOfInstPackages[m_argParser.otherArguments()[0]].files.begin(), m_listOfInstPackages[m_argParser.otherArguments()[0]].files.end(), ostream_iterator<string>(cout, "\n"));
	}
}
void CardsInfo::listInstalled()
{
	getListOfPackageNames(m_root);
	buildDatabaseWithNameVersion();

	for (packages_t::const_iterator i = m_listOfInstPackages.begin(); i != m_listOfInstPackages.end(); ++i) {
		cout << i->first << " " << i->second.version << endl;
	}
	cout << endl << "Number of installed packages: " <<  m_packageNamesList.size() << endl << endl;
}
void CardsInfo::listBinaries()
{
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseCategoryDirectory();
	cp->parsePortsList();
	cp->parseBasePackageList();
	cout << endl << "Number of availables binaries: " << cp->getBinaryPackageList() << endl<< endl;
	
}
void CardsInfo::listPorts()
{
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	unsigned int  numberOfPorts = 0;
  cp->parseCategoryDirectory();
  cp->parsePackagePkgfileList();
	numberOfPorts = cp->getPortsList();
	cout << endl << "Number of available ports: " << numberOfPorts << endl << endl;
}
void CardsInfo::listOutOfDate()
{
	
  ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
  cp->parseCategoryDirectory();
  cp->parsePortsList();
  cp->parseBasePackageList();
  set<string> result = cp->getListOutOfDate();
	
  if (result.size() > 0) {
		string packageName = "";
		string nameToFind = "";
		std::string::size_type pos;
		CardsDepends CD(m_argParser);
		vector<LevelName> packagesLevelList = CD.getLevel();
		if (packagesLevelList.size() > 0) {
			for (vector<LevelName>::iterator i = packagesLevelList.begin();i != packagesLevelList.end();i++) {
				for (set<string>::const_iterator j = result.begin(); j != result.end(); ++j) {
					packageName = "/" + *j + "/";
					
#ifndef NDEBUG
					cerr << *j << endl;
					cerr << packageName << endl;
					cerr << i->l << " " << i->name << endl;
#endif
					nameToFind = i->name + "/";
					pos = nameToFind.find(packageName);
					if (pos != std::string::npos) {
						cout << i->l << ": " << *j << endl;
						break;
					}
				} 
    	}
			cout << endl << "Number of ports which need to be updated: " << result.size() << endl << endl;
		} else {
			cout << "Check dependencies with cards level command ..." << endl;
		}
	}
}
void CardsInfo::infoInstall()
{
	getListOfPackageNames(m_root);
	buildDatabaseWithDetailsInfos(false);
	if (checkPackageNameExist(m_argParser.otherArguments()[0])) {
		char * c_time_s = ctime(&m_listOfInstPackages[m_argParser.otherArguments()[0]].build);
		cout << "Name           : " << m_argParser.otherArguments()[0] << endl
			<< "Description    : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].description << endl
			<< "URL            : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].url << endl
			<< "Maintainer(s)  : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].maintainer << endl
			<< "Packager(s)    : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].packager << endl
			<< "Version        : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].version << endl
			<< "Release        : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].release << endl
			<< "Build date     : " << c_time_s
			<< "Size           : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].size << endl
			<< "Number of Files: " << m_listOfInstPackages[m_argParser.otherArguments()[0]].files.size()<< endl
			<< "Arch           : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].arch << endl;
	} else {
		cout << m_argParser.otherArguments()[0] << " not found " << endl;
	}
}
void CardsInfo::infoBinary()
{
	// FIXME Not working any more
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseCategoryDirectory();
	cp->parseBasePackageList();
	if ( ! cp->getBinaryPackageInfo(m_argParser.otherArguments()[0]) ) {
		cout << m_argParser.otherArguments()[0] << " not found " << endl;
	}
}
void CardsInfo::infoPort()
{
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseCategoryDirectory();
	cp->parsePackagePkgfileList();
	if ( ! cp->getPortInfo(m_argParser.otherArguments()[0]) ) {
		cout << m_argParser.otherArguments()[0] << " not found " << endl;
	}
}
void CardsInfo::diffPorts()
{
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseCategoryDirectory();
	cp->parsePackagePkgfileList();
	vector<pair<string, DiffVers > > result;
	DiffVers DV;
	DV.installed="Installed";
	DV.available="Available in the ports tree";
	
	result.push_back(pair<string, DiffVers >("Package", DV));
	DV.installed="";
	DV.available="";
	result.push_back(pair<string, DiffVers>("", DV));
	unsigned int widthPackageName = result.begin()->first.length();
	unsigned int widthInstalled = result.begin()->second.installed.length();

	for (packages_t::const_iterator i = m_listOfInstPackages.begin(); i != m_listOfInstPackages.end(); ++i) {
		if (! cp->checkPortExist(i->first))
			continue;
		string newVersion = cp->getPortVersion(i->first);
#ifndef NDEBUG
		cerr << i->first << " " << i->second.version << " " << newVersion << endl;	
#endif
		if ( i->second.version != newVersion ) {
			DV.installed = i->second.version;
			DV.available = newVersion;
			result.push_back(pair<string, DiffVers> (i->first, DV));
			if (i->first.length() > widthPackageName)
				widthPackageName = i->first.length();
			if (i->second.version.length() > widthInstalled)
				widthInstalled = i->second.version.length();
			
		}
	}
	if (result.size() > 2) {
		cout << endl << "Differences between installed packages and ports tree:" << endl << endl;
		for (vector<pair<string, DiffVers> >::const_iterator i = result.begin(); i != result.end(); ++i) {
			cout << left << setw(widthPackageName + 2) << i->first << setw(widthInstalled + 2)<< i->second.installed << " " << i->second.available << endl;
		}
		cout << endl << result.size() - 2 << " packages are differents." << endl << endl;
	} else {
		cout << "no differences found with the ports" << endl<<endl;
	}	
}
void CardsInfo::diffBinaries()
{
	buildDatabaseWithNameVersion();
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parsePkgRepoCategoryDirectory();
	vector<pair<string, DiffVers > > result;
	DiffVers DV;
	DV.installed="Installed";
	DV.available="Available in the depot of binaries"	;

	result.push_back(pair<string, DiffVers >("Package", DV));
	DV.installed="";
	DV.available="";
	result.push_back(pair<string, DiffVers >("", DV));
	unsigned int widthPackageName = result.begin()->first.length();
	unsigned int widthInstalled = result.begin()->second.installed.length();

	for (packages_t::const_iterator i = m_listOfInstPackages.begin(); i != m_listOfInstPackages.end(); ++i) {
		if (! cp->checkBinaryExist(i->first))
			continue;
		string baseName = cp->getBasePackageName(i->first);
		string newVersion = cp->getPortVersion(i->first);
		if ( i->second.version != newVersion ) {
			DV.installed = i->second.version;
			DV.available = newVersion;
			result.push_back(pair<string, DiffVers> (i->first, DV));
			if (i->first.length() > widthPackageName)
				widthPackageName = i->first.length();
		}
/* FIXME
		} else {
			time_t newVersion = cp->getBinaryBuildTime(i->first);

			if ( i->second.build < newVersion ) {
				char * c_time_s = ctime(&i->second.build);
				c_time_s[strlen(c_time_s)-1]='\0';
				string instVers = c_time_s;
				c_time_s = ctime(&newVersion);
				c_time_s[strlen(c_time_s)-1]='\0';
				string availVers = c_time_s;
				DV.installed=instVers;
				DV.available=availVers;
				result.push_back(pair<string, DiffVers>(i->first, DV));
				if (i->first.length() > widthPackageName)
					widthPackageName = i->first.length();
			}
		} */
	}
	if (result.size() > 2) {
		cout << endl << "Differences between installed packages and the depot of binaries:" << endl << endl;
		for (vector<pair<string, DiffVers> >::const_iterator i = result.begin(); i != result.end(); ++i) {
			cout << left << setw(widthPackageName + 2) << i->first << setw(widthInstalled + 2) << i->second.installed << " " << i->second.available << endl;
		}
		cout << endl << result.size() - 2 << " packages are differents." << endl << endl;
	} else {
		cout << "no differences found with the binaries" << endl<<endl;
	}
}
void CardsInfo::search()
{
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseCategoryDirectory();
	cp->parsePackagePkgfileList();

	if ( ! cp->search(m_argParser.otherArguments()[0]) ) {
		cout << "no occurence found" << endl;
	}
}
void CardsInfo::printHelp() const
{
}
unsigned int CardsInfo::getRemotePackages(const string& md5sumFile)
{
	m_remotePackagesList.clear();
	if ( parseFile(m_remotePackagesList,md5sumFile.c_str()) != 0) {
		 m_actualError = CANNOT_READ_FILE;
		treatErrors(md5sumFile);
	}
	return m_remotePackagesList.size();
}
// vim:set ts=2 :
