// cards_info.h
// 
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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
#ifndef CARDSINFO_H
#define CARDSINFO_H

#include <string>
#include <list>
#include <vector>

#include "file_download.h"
#include "file_utils.h"

#include <curl/curl.h>

#include "config_parser.h"
#include "cards_argument_parser.h"
#include "pkgdbh.h"
#include "process.h"


class CardsInfo : public Pkgdbh {
public:
	CardsInfo (const CardsArgumentParser& argParser);

	void printDependenciesList();
	void listInstalled();
	void listBinaries();
	void listPorts();
	void infoInstall();
	void infoBinary();
	void infoPort();
	void showDiff();
	void showDetails(const string& packageName);

	virtual void run(int argc, char** argv);
	virtual void printHelp() const;

private:

	void getSignatures();
	void generateDependencies();
	bool getPackageFileName();

	unsigned int getRemotePackages(const string& md5sumFile);

	string m_packageName;
	string m_packageFileName;
	vector<string> m_MD5packagesNameVersionList;
	vector<string> m_dependenciesList;
	vector<string> m_packageNameList;
	set<string> m_remotePackagesList;
	Config m_config;

	
	const CardsArgumentParser& m_argParser;
	// TODO make configurable from the command line
	const string m_root; // absolute path of installation
};
#endif
// vim:set ts=2 :
