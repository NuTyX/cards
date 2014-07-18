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


#include "cards_info.h"

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
}
void CardsInfo::infoBinary()
{
}
void CardsInfo::infoPort()
{
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
