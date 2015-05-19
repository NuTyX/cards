// cards_install.h
// 
//  Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
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
#ifndef CARDSINSTALL_H
#define CARDSINSTALL_H

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


class CardsInstall : public Pkgdbh {
public:
	CardsInstall (const CardsArgumentParser& argParser);
	CardsInstall (const CardsArgumentParser& argParser, const string& packageName);
	CardsInstall (const CardsArgumentParser& argParser, const vector<string>& listOfPackages);

	void printDependenciesList();
	void update();
	void install();
	void install(const vector<string>& dependenciesList);
	void createBinariesOf(const string& packageName);
	virtual void run(int argc, char** argv);
	virtual void printHelp() const;
	
private:
	/* 
	 * this methode is only checking what available in the path passed as argument
	 * It only call  by the public method install(dependenciesList) 
	 * It is used by packagers and for the bot.
	*/
	set<string> findPackages(const string& path);
	set<string> getDirectDependencies();
	set<string> applyInstallRules(const string& name, pkginfo_t& info, const vector<rule_t>& rules);
	set<string> getKeepFileList(const set<string>& files, const vector<rule_t>& rules);

	void getInstallRulesList(const vector<rule_t>& rules, rule_event_t event, vector<rule_t>& found) const;

	void createBinaries();
	void getSignatures();
	void generateDependencies();
	bool getPackageFileName();
	void addPackagesList();
	void addPackage();

	const CardsArgumentParser& m_argParser;
	string m_packageName;
	string m_packageFileName;
	vector<string> m_MD5packagesNameVersionList;
	vector<string> m_dependenciesList;
	vector<string> m_packageNameList;

	ConfigParser  * m_configParser;
	Config m_config;

	// TODO make configurable from the command line
	const string m_root; // absolute path of installation
	bool m_force;
	bool m_archive;
};
#endif
// vim:set ts=2 :
