//
//  pkgadd.cxx
//
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
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

using namespace std;

Pkgadd::Pkgadd()
	: Pkgdbh("pkgadd"),
	m_runPrePost(true),
	m_upgrade(false),
	m_force(false)
{
	// Checking the rules
	readRulesFile();

}
Pkgadd::Pkgadd(const std::string& commandName)
	: Pkgdbh(commandName),
	m_runPrePost(true),
	m_upgrade(false),
	m_force(false)
{
	// Checking the rules
	readRulesFile();

}
void Pkgadd::run(int argc, char** argv)
{
	parseArguments(argc, argv);
	run();
}
void Pkgadd::parseArguments(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			m_root = argv[i + 1];
			i++;
		} else if (option == "-i" || option == "--ignore") {
			m_runPrePost = false;
		} else if (option == "-u" || option == "--upgrade") {
			m_upgrade = true;
		} else if (option == "-f" || option == "--force") {
			m_force = true;
		} else if (option[0] == '-' || !m_packageArchiveName.empty()) {
			m_actualError = INVALID_OPTION;
			treatErrors(option);
		} else {
			m_packageArchiveName = option;
		}
	}
	if (m_packageArchiveName.empty()) {
		m_actualError = OPTION_MISSING;
		treatErrors("");
	}
	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	// Check UID
	if (getuid())
	{
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

}
void Pkgadd::preRun()
{
	// Check UID
	if (getuid())
	{
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	extractAndRunPREfromPackage(m_packageArchiveName);
}
void Pkgadd::run()
{
	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildCompleteDatabase(false);

	// Reading the archiving to find a list of files
	pair<string, pkginfo_t> package = openArchivePackage(m_packageArchiveName);

	readRulesFile();

	bool installed = checkPackageNameExist(package.first);
	if (installed && !m_upgrade)
	{
		m_actualError = PACKAGE_ALLREADY_INSTALL;
		treatErrors (package.first);
	}
	else if (!installed && m_upgrade)
	{
		m_actualError = PACKAGE_NOT_PREVIOUSLY_INSTALL;
		treatErrors(package.first);
	}

	set<string> non_install_files = applyInstallRules(package.first,
		package.second, m_actionRules);
	if (!m_upgrade) {
#ifndef NDEBUG
		cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
		if (m_runPrePost) preRun();
	}
	set<string> conflicting_files = getConflictsFilesList(package.first,
		package.second);
	if (!conflicting_files.empty()) {
		if (m_force) {
			Db_lock lock(m_root, true);
			set<string> keep_list;
			// Don't remove files matching the rules in configuration
			if (m_upgrade)
				keep_list = getKeepFileList(conflicting_files, m_actionRules);
			// Remove unwanted conflicts
			removePackageFilesRefsFromDB(conflicting_files, keep_list);
		} else {
			copy(conflicting_files.begin(), conflicting_files.end(),
				ostream_iterator<string>(cerr, "\n"));
			m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
			treatErrors(package.first);
		}
	}

	set<string> keep_list;
	if (m_upgrade) {
#ifndef NDEBUG
		cerr << "Run extractAndRunPREfromPackage with upgrade" << endl;
#endif

		if ( checkDependency(package.first) )
			setDependency ();
		if (m_runPrePost) preRun();
		Db_lock lock(m_root, true);
		// Remove metadata about the package removed
		removePackageFilesRefsFromDB(package.first);
		keep_list = getKeepFileList(package.second.files, m_actionRules);
		removePackageFiles(package.first, keep_list);
	}
	{
		Db_lock lock(m_root, true);
		// Installation progressInfo of the files on the HD
		installArchivePackage(m_packageArchiveName,
			keep_list, non_install_files);

		// Add the metadata about the package to the DB
		moveMetaFilesPackage(package.first,package.second);

		// Add the info about the files to the DB
		addPackageFilesRefsToDB(package.first, package.second);
	}
	if (m_runPrePost)
		postRun();

}
void Pkgadd::postRun()
{
	if (checkFileExist(PKG_POST_INSTALL))
	{
		m_actualAction = PKG_POSTINSTALL_START;
		progressInfo();
		process postinstall(SHELL,PKG_POST_INSTALL, 0 );
		if (postinstall.executeShell()) {
			cerr << _("WARNING Run post-install FAILED, continue") << endl;
		}
		m_actualAction = PKG_POSTINSTALL_END;
		progressInfo();
		removeFile(m_root,PKG_POST_INSTALL);
	}
}
void Pkgadd::printHelp() const
{
	cout << USAGE << m_utilName << " [options] <file>" << endl
	<< OPTIONS << endl
	<< "  -i, --ignore        "
	<< _("do not execute pre/post install scripts")
	<< endl
	<< "  -u, --upgrade       "
	<< _("upgrade package with the same name")
	<< endl
	<< "  -f, --force         "
	<< _("force install, overwrite conflicting files")
	<< endl
	<< "  -r, --root <path>   "
	<< _("specify alternative installation root")
	<< endl
	<< "  -v, --version       "
	<< _("print version and exit")
	<< endl
	<< "  -h, --help          "
	<< _("print help and exit")
	<< endl;
}

set<string> Pkgadd::getKeepFileList(const set<string>& files,
	const vector<rule_t>& rules)
{
	set<string> keep_list;
	vector<rule_t> found;

	getInstallRulesList(rules, UPGRADE, found);

	for (auto i : files) {
		for (vector<rule_t>::reverse_iterator j = found.rbegin();
		j != found.rend();
		j++) {
			if (checkRuleAppliesToFile(*j, i)) {
				if (!(*j).action)
					keep_list.insert(keep_list.end(), i);

				break;
			}
		}
	}

#ifndef NDEBUG
	cerr << "Keep list:" << endl;
	for (auto j : keep_list) cerr << "   " << j << endl;
	cerr << endl;
#endif
	return keep_list;
}

set<string> Pkgadd::applyInstallRules(const string& name,
	pkginfo_t& info,
	const vector<rule_t>& rules)
{
	// TODO: better algo(?)
	set<string> install_set;
	set<string> non_install_set;

	vector<rule_t> files_found;
	getInstallRulesList(rules, INSTALL, files_found);

	vector<rule_t> scripts_found;
	getPostInstallRulesList(rules, scripts_found);

	for (auto i : info.files) {

		bool install_file = true;
		for (vector<rule_t>::reverse_iterator j = files_found.rbegin();
		j != files_found.rend();
		j++) {
			if (checkRuleAppliesToFile(*j, i)) {
				install_file = (*j).action;
				break;
			}
		}
		if (install_file)
			install_set.insert(install_set.end(), i);
		else
			non_install_set.insert(i);

		pair <string, int> a;
		for (vector<rule_t>::reverse_iterator j = scripts_found.rbegin();
		j != scripts_found.rend();
		j++) {
			a.first=i;
			a.second=(*j).event;

			if (checkRuleAppliesToFile((*j), i)) {
				install_file = (*j).action;
				break;
				}
		}
		if (install_file) {
			if ( a.second == LDCONF ) {
				a.first="";
			} else {
				a.first=i;
			}
			m_postInstallList.insert(a);
		}
	}
	scripts_found.clear();
	info.files.clear();
	info.files = install_set;

#ifndef NDEBUG
	cerr << "PostInstall set:" << endl;
	for (auto i : m_postInstallList) cerr << "  " << i.first << " " << i.second << endl;
	cerr << "Install set:" << endl;
	for  (auto i : info.files) cerr << "   " << i << endl;
	cerr << endl;
	cerr << "Non-Install set:" << endl;
	for (auto i : non_install_set) cerr << "   " << i << endl;
	cerr << endl;
#endif

	return non_install_set;
}
void Pkgadd::applyPostInstallRules(const string& name,
	pkginfo_t& info,
	const vector<rule_t>& rules)
{
	vector<rule_t> found;

	getPostInstallRulesList(rules, found);

	for (auto i : info.files) {
		bool install_file = true;
		for (vector<rule_t>::reverse_iterator j = found.rbegin();
		j != found.rend();
		j++) {
			pair <string, int> a;
			a.first=(*j).event;
			a.second=(*j).action;

			if (checkRuleAppliesToFile((*j), i)) {
				install_file = (*j).action;
				if (install_file) {
					if ( a.second == LDCONF ) {
						a.first="";
					} else {
						a.first=i;
					}
					m_postInstallList.insert(a);
				}
				break;
			}
		}
	}
}
void Pkgadd::getInstallRulesList(const vector<rule_t>& rules,
	rule_event_t event, 
	vector<rule_t>& found) const
{
	for (auto i : rules ) {
		if (i.event == event)
			found.push_back(i);
	}
}
void Pkgadd::getPostInstallRulesList(const vector<rule_t>& rules,
	vector<rule_t>& found) const
{
	for (auto i : rules ) {
		if ( (i.event != INSTALL) && (i.event != UPGRADE))
			found.push_back(i);
	}
}
// vim:set ts=2 :
