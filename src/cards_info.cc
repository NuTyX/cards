// cards_info.cc
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

#include <iomanip>
#include <sstream>

#include "cards_info.h"

using namespace std;

CardsInfo::CardsInfo(const CardsArgumentParser& argParser)
	:  m_argParser(argParser), m_root("/")
{
}
void CardsInfo::run(int argc, char** argv)
{
	getListOfPackageNames(m_root);
	buildDatabaseWithDetailsInfos(false);
}
void CardsInfo::listInstalled()
{
	for (packages_t::const_iterator i = m_listOfInstPackages.begin(); i != m_listOfInstPackages.end(); ++i) {
		cout << i->first << " " << i->second.version << endl;
	}
	cout << endl << "Number of installed packages: " <<  m_packageNamesList.size() << endl << endl;
}
void CardsInfo::listBinaries()
{
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("list of binaries: ");
	}
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseMD5sumCategoryDirectory();
	cp->parsePortsList();
	cp->parseBasePackageList();
	cout << endl << "Number of availables binaries: " << cp->getBinaryPackageList() << endl;
	
}
void CardsInfo::listPorts()
{
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
	unsigned int  numberOfPorts = 0;
	set<string> localPackagesList;
	for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
		string prtDir = i->Dir ;
		if ( findFile(localPackagesList, prtDir) != 0 ) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
	}
	string name, version;
	for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); li++) {
		string val = *li;
		string::size_type pos = val.find('@');
		if (pos != string::npos) {
			name = val.substr(0,pos);
			version = val.substr(pos+1);
			cout << name + " " + version << endl;
		}
	}
	numberOfPorts = numberOfPorts + localPackagesList.size();
	if ( ( config.dirUrl.size() == 0 ) || (localPackagesList.size() == 0 ) ) {
		cout << "You need to cards sync first or/and check your /etc/cards.conf file." << endl;
	} else {
		cout << endl << "Number of available ports: " << numberOfPorts << endl << endl;
	}
}
void CardsInfo::infoInstall()
{
	if (checkPackageNameExist(m_argParser.otherArguments()[0])) {
		char * c_time_s = ctime(&m_listOfInstPackages[m_argParser.otherArguments()[0]].build);
		cout << "Name           : " << m_argParser.otherArguments()[0] << endl
			<< "Description    : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].description << endl
			<< "Version        : " << m_listOfInstPackages[m_argParser.otherArguments()[0]].version << endl
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
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("info on binaries: ");
	}
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseMD5sumCategoryDirectory();
	cp->parsePortsList();
	cp->parseBasePackageList();
	cp->parsePackageInfoList();
	if ( ! cp->getBinaryPackageInfo(m_argParser.otherArguments()[0]) ) {
		cout << m_argParser.otherArguments()[0] << " not found " << endl;
	}
}
void CardsInfo::infoPort()
{
	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseMD5sumCategoryDirectory();
	cp->parsePortsList();
	cp->parsePackageInfoList();
	if ( ! cp->getPortInfo(m_argParser.otherArguments()[0]) ) {
		cout << m_argParser.otherArguments()[0] << " not found " << endl;
	}
}
void CardsInfo::diffPorts()
{
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("diff on ports: ");
	}

	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseMD5sumCategoryDirectory();

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
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("diff on binaries: ");
	}

	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
	cp->parseMD5sumCategoryDirectory();
	cp->parsePortsList();
	cp->parseBasePackageList();

	vector<pair<string, string > > result;

	result.push_back(pair<string, string>("Package",
	"Installed                  Available in the depot of binaries"));
	result.push_back(pair<string, string>("", ""));
	unsigned int widthPackageName = result.begin()->first.length();

	for (packages_t::const_iterator i = m_listOfInstPackages.begin(); i != m_listOfInstPackages.end(); ++i) {
		if (! cp->checkBinaryExist(i->first))
			continue;
		string newVersion = cp->getPortVersion(i->first);
		if ( i->second.version != newVersion ) {
			result.push_back(pair<string, string>(i->first, i->second.version + "   " + newVersion));
			if (i->first.length() > widthPackageName)
				widthPackageName = i->first.length();
		} else {
			time_t newVersion = cp->getBinaryBuildTime(i->first);

			if ( i->second.build < newVersion ) {
				char * c_time_s = ctime(&i->second.build);
				c_time_s[strlen(c_time_s)-1]='\0';
				string instVers = c_time_s;
				c_time_s = ctime(&newVersion);
				c_time_s[strlen(c_time_s)-1]='\0';
				string availVers = c_time_s;
				result.push_back(pair<string, string>(i->first, instVers + "   " + availVers));
				if (i->first.length() > widthPackageName)
					widthPackageName = i->first.length();
			}
		}
	}
	if (result.size() > 2) {
		cout << endl << "Differences between installed packages and the depot of binaries:" << endl << endl;
		for (vector<pair<string, string> >::const_iterator i = result.begin(); i != result.end(); ++i) {
			cout << left << setw(widthPackageName + 2) << i->first << i->second << endl;
		}
		cout << endl << result.size() - 2 << " packages are differents." << endl << endl;
	} else {
		cout << "no differences found with the binaries" << endl<<endl;
	}
}
void CardsInfo::search()
{
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("search on binaries: ");
	}

	ConfigParser  * cp = new ConfigParser("/etc/cards.conf");

	cp->parseMD5sumCategoryDirectory();
	cp->parsePortsList();
	cp->parseBasePackageList();
	cp->parsePackageInfoList();

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
