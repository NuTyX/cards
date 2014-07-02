// cards_install.h
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

	void printDependenciesList();
	void install();

	virtual void run(int argc, char** argv);
	virtual void printHelp() const;

private:
	vector<string> getDirectDependencies();
	set<string> applyInstallRules(const string& name, pkginfo_t& info, const vector<rule_t>& rules);
	void getInstallRulesList(const vector<rule_t>& rules, rule_event_t event, vector<rule_t>& found) const;

	void getSignatures();
	void generateDependencies();
	bool getPackageFileName();

	string m_packageName;
	string m_packageFileName;
	vector<string> m_MD5packagesNameVersionList;
	vector<string> m_dependenciesList;
	vector<string> m_packageNameList;
	Config m_config;

	
	const CardsArgumentParser& m_argParser;
	// TODO make configurable from the command line
	const string m_root; // absolute path of installation
};
#endif
// vim:set ts=2 :
