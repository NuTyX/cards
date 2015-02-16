//  pkgadd.cc
//
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
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
#include "file_utils.h"
#include "pkgadd.h"
#include "process.h"

#include <algorithm>

#include <fstream>
#include <iterator>
#include <cstdio>
#include <regex.h>
#include <unistd.h>

void Pkgadd::run(int argc, char** argv)
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
		m_actualError = OPTION_MISSING;
		treatErrors("");
	}
	//
	// Check UID
	//
	if (getuid())
	{
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	//
	// Install/upgrade package
	//
	{
		Db_lock lock(o_root, true);
		// Get the list of installed packages
		getListOfPackageNames(o_root);

		// Retrieving info about all the packages
		buildDatabaseWithDetailsInfos(false);

		// Reading the archiving to find a list of files
		pair<string, pkginfo_t> package = openArchivePackage(o_package);

		// Checking the rules
		readRulesFile();

		bool installed = checkPackageNameExist(package.first);
		if (installed && !o_upgrade)
		{
			m_actualError = PACKAGE_ALLREADY_INSTALL;
			treatErrors (package.first);
		}
		else if (!installed && o_upgrade)
		{
			m_actualError = PACKAGE_NOT_PREVIOUSLY_INSTALL;
			treatErrors(package.first);
		}

		set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);
		if (!o_upgrade) {
#ifndef NDEBUG
			cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
			// Run pre-install if exist
			extractAndRunPREfromPackage(o_package);
		}

		set<string> conflicting_files = getConflictsFilesList(package.first, package.second);

		if (!conflicting_files.empty()) {
			if (o_force) {
				set<string> keep_list;
				if (o_upgrade) // Don't remove files matching the rules in configuration
					keep_list = getKeepFileList(conflicting_files, m_actionRules);
				removePackageFilesRefsFromDB(conflicting_files, keep_list); // Remove unwanted conflicts
			} else {
				copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
				m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
				treatErrors("listed file(s) already installed (use -f to ignore and overwrite)");
			}
		}

		set<string> keep_list;

		if (o_upgrade) {

			// Remove metadata about the package removed
			removePackageFilesRefsFromDB(package.first);

			keep_list = getKeepFileList(package.second.files, m_actionRules);
			removePackageFiles(package.first, keep_list);

#ifndef NDEBUG
			cerr << "Run extractAndRunPREfromPackage after upgrade" << endl;
#endif
			// Run pre-install if exist
			extractAndRunPREfromPackage(o_package);
		}
		// Installation progressInfo of the files on the HD
		installArchivePackage(o_package, keep_list, non_install_files);
		// Post install
		if (checkFileExist(PKG_POST_INSTALL))
		{
			m_actualAction = PKG_POSTINSTALL_START;
			progressInfo();
			process postinstall(SHELL,PKG_POST_INSTALL, 0 );
			if (postinstall.executeShell()) {
				cerr << "WARNING Run post-install FAILED. continue" << endl;
			}
			m_actualAction = PKG_POSTINSTALL_END;
			progressInfo();
		}
		// Add the metadata about the package to the DB
		moveMetaFilesPackage(package.first,package.second);

		// Add the info about the files to the DB
		addPackageFilesRefsToDB(package.first, package.second);

		runLdConfig();
	}
}
void Pkgadd::printHelp() const
{
	cout << "usage: " << m_utilName << " [options] <file>" << endl
	     << "options:" << endl
	     << "  -u, --upgrade       upgrade package with the same name" << endl
	     << "  -f, --force         force install, overwrite conflicting files" << endl
	     << "  -r, --root <path>   specify alternative installation root" << endl
	     << "  -v, --version       print version and exit" << endl
	     << "  -h, --help          print help and exit" << endl;
}

set<string> Pkgadd::getKeepFileList(const set<string>& files, const vector<rule_t>& rules)
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

set<string> Pkgadd::applyInstallRules(const string& name, pkginfo_t& info, const vector<rule_t>& rules)
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

void Pkgadd::getInstallRulesList(const vector<rule_t>& rules, rule_event_t event, vector<rule_t>& found) const
{
	for (vector<rule_t>::const_iterator i = rules.begin(); i != rules.end(); i++)
		if (i->event == event)
			found.push_back(*i);
}
// vim:set ts=2 :
