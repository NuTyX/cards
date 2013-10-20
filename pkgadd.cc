//  pkgadd.cc
// 
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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

#include "pkgadd.h"
#include <fstream>
#include <iterator>
#include <cstdio>
#include <regex.h>
#include <unistd.h>

void pkgadd::run(int argc, char** argv)
{
	//
	// Check command line options
	//
	string o_root;
	string o_package;
	bool o_upgrade = false;
	bool o_force = false;

	for (int i = 1; i < argc; i++) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option == "-u" || option == "--upgrade") {
			o_upgrade = true;
		} else if (option == "-f" || option == "--force") {
			o_force = true;
		} else if (option[0] == '-' || !o_package.empty()) {
			throw runtime_error("invalid option " + option);
		} else {
			o_package = option;
		}
	}

	if (o_package.empty())
	{
		actualError = OPTION_MISSING;
		treatErrors("");
	}
	//
	// Check UID
	//
	if (getuid())
	{
		actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	//
	// Install/upgrade package
	//
	{
		db_lock lock(o_root, true);
		getListOfPackages(o_root);
		//Retrieving info about all the packages
		getInstalledPackages();
		// Reading the archiving to find a list of files
		pair<string, pkginfo_t> package = openArchivePackage(o_package);

		// checking the rules
		vector<rule_t> config_rules = readRulesFile();

		bool installed = checkPackageNameExist(package.first);
		if (installed && !o_upgrade)
		{
			actualError = PACKAGE_ALLREADY_INSTALL;
			treatErrors (package.first);
		}
		else if (!installed && o_upgrade)
		{
			actualError = PACKAGE_NOT_PREVIOUSLY_INSTALL;
			treatErrors(package.first);
		}
		set<string> non_install_files = applyInstallRules(package.first, package.second, config_rules);
		set<string> conflicting_files = getConflictsFilesList(package.first, package.second);
      
		if (!conflicting_files.empty()) {
			if (o_force) {
				set<string> keep_list;
				if (o_upgrade) // Don't remove files matching the rules in configuration
					keep_list = getKeepFileList(conflicting_files, config_rules);
				removePackageFilesRefsFromDB(conflicting_files, keep_list); // Remove unwanted conflicts
			} else {
				copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
				actualError = LISTED_FILES_ALLREADY_INSTALLED;
				treatErrors("listed file(s) already installed (use -f to ignore and overwrite)");
			}
		}
   
		set<string> keep_list;

		if (o_upgrade) {
			keep_list = getKeepFileList(package.second.files, config_rules);
			removePackageFilesRefsFromDB(package.first, keep_list);
		}

		// Installation progressInfo of the files on the HD
		installArchivePackage(o_package, keep_list, non_install_files);

		// Add the metadata about the package to the DB
		moveMetaFilesPackage(package.first,package.second);

		// Add the info about the files to the DB
		addPackageFilesRefsToDB(package.first, package.second);

		runLdConfig();
	}
}
void pkgadd::printHelp() const
{
	cout << "usage: " << utilName << " [options] <file>" << endl
	     << "options:" << endl
	     << "  -u, --upgrade       upgrade package with the same name" << endl
	     << "  -f, --force         force install, overwrite conflicting files" << endl
	     << "  -r, --root <path>   specify alternative installation root" << endl
	     << "  -v, --version       print version and exit" << endl
	     << "  -h, --help          print help and exit" << endl;
}

vector<rule_t> pkgadd::readRulesFile()
{
	vector<rule_t> rules;
	unsigned int linecount = 0;
	const string filename = root + PKGADD_CONF;
	ifstream in(filename.c_str());

	if (in) {
		while (!in.eof()) {
			string line;
			getline(in, line);
			linecount++;
			if (!line.empty() && line[0] != '#') {
				if (line.length() >= PKGADD_CONF_MAXLINE)
				{
					actualError = PKGADD_CONFIG_LINE_TOO_LONG;
					treatErrors(filename + ":" + itos(linecount));
				}
				char event[PKGADD_CONF_MAXLINE];
				char pattern[PKGADD_CONF_MAXLINE];
				char action[PKGADD_CONF_MAXLINE];
				char dummy[PKGADD_CONF_MAXLINE];
				if (sscanf(line.c_str(), "%s %s %s %s", event, pattern, action, dummy) != 3)
				{
					actualError = PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS;
					treatErrors(filename + ":" + itos(linecount));
				}
				if (!strcmp(event, "UPGRADE") || !strcmp(event, "INSTALL")) {
					rule_t rule;
					rule.event = strcmp(event, "UPGRADE") ? INSTALL : UPGRADE;
					rule.pattern = pattern;
					if (!strcmp(action, "YES")) {
						rule.action = true;
					} else if (!strcmp(action, "NO")) {
						rule.action = false;
					} else
					{
						actualError = PKGADD_CONFIG_UNKNOWN_ACTION;
						treatErrors(filename + ":" + itos(linecount) + ": '" +
								    string(action));
					}

					rules.push_back(rule);
				} else
				{
					actualError = PKGADD_CONFIG_UNKNOWN_EVENT;
					treatErrors(filename + ":" + itos(linecount) + ": '" +
							    string(event));
				}
			}
		}
		in.close();
	}

#ifndef NDEBUG
	cerr << "Configuration:" << endl;
	for (vector<rule_t>::const_iterator j = rules.begin(); j != rules.end(); j++) {
		cerr << "\t" << (*j).pattern << "\t" << (*j).action << endl;
	}
	cerr << endl;
#endif

	return rules;
}

set<string> pkgadd::getKeepFileList(const set<string>& files, const vector<rule_t>& rules)
{
	set<string> keep_list;
	vector<rule_t> found;

	getInstallRulesList(rules, UPGRADE, found);

	for (set<string>::const_iterator i = files.begin(); i != files.end(); i++) {
		for (vector<rule_t>::reverse_iterator j = found.rbegin(); j != found.rend(); j++) {
			if (checkRuleAppliesToFile(*j, *i)) {
				if (!(*j).action)
					keep_list.insert(keep_list.end(), *i);

				break;
			}
		}
	}

#ifndef NDEBUG
	cerr << "Keep list:" << endl;
	for (set<string>::const_iterator j = keep_list.begin(); j != keep_list.end(); j++) {
		cerr << "   " << (*j) << endl;
	}
	cerr << endl;
#endif

	return keep_list;
}

set<string> pkgadd::applyInstallRules(const string& name, pkginfo_t& info, const vector<rule_t>& rules)
{
	// TODO: better algo(?)
	set<string> install_set;
	set<string> non_install_set;
	vector<rule_t> found;

	getInstallRulesList(rules, INSTALL, found);

	for (set<string>::const_iterator i = info.files.begin(); i != info.files.end(); i++) {
		bool install_file = true;

		for (vector<rule_t>::reverse_iterator j = found.rbegin(); j != found.rend(); j++) {
			if (checkRuleAppliesToFile(*j, *i)) {
				install_file = (*j).action;
				break;
			}
		}

		if (install_file)
			install_set.insert(install_set.end(), *i);
		else
			non_install_set.insert(*i);
	}

	info.files.clear();
	info.files = install_set;

#ifndef NDEBUG
	cerr << "Install set:" << endl;
	for (set<string>::iterator j = info.files.begin(); j != info.files.end(); j++) {
		cerr << "   " << (*j) << endl;
	}
	cerr << endl;

	cerr << "Non-Install set:" << endl;
	for (set<string>::iterator j = non_install_set.begin(); j != non_install_set.end(); j++) {
		cerr << "   " << (*j) << endl;
	}
	cerr << endl;
#endif

	return non_install_set;
}

void pkgadd::getInstallRulesList(const vector<rule_t>& rules, rule_event_t event, vector<rule_t>& found) const
{
	for (vector<rule_t>::const_iterator i = rules.begin(); i != rules.end(); i++)
		if (i->event == event)
			found.push_back(*i);
}

bool pkgadd::checkRuleAppliesToFile(const rule_t& rule, const string& file)
{
	regex_t preg;
	bool ret;

	if (regcomp(&preg, rule.pattern.c_str(), REG_EXTENDED | REG_NOSUB))
	{
		actualError = CANNOT_COMPILE_REGULAR_EXPRESSION;
		treatErrors(rule.pattern);
	}
	ret = !regexec(&preg, file.c_str(), 0, 0, 0);
	regfree(&preg);

	return ret;
}
// vim:set ts=2 :
