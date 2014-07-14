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
}
void CardsInfo::listInstalled()
{
	getListOfPackageNames(m_root);
	for(set<string>::const_iterator i = m_packageNamesList.begin(); i !=  m_packageNamesList.end(); ++i) {
		cout << *i <<  endl;
	}
}
void CardsInfo::listBinaries()
{
	cout << "List of Binaries" << endl;
}
void CardsInfo::listPorts()
{
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
	unsigned int  numberOfPorts = 0;
	set<string> localPackagesList;
	for (vector<string>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
		string val = *i ;
		string prtDir;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			prtDir = stripWhiteSpace(val.substr(0,pos));
		} else {
			prtDir = val;
		}
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
// vim:set ts=2 :
