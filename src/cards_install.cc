// cards_install.cc
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
#include "archive_utils.h"
#include "cards_install.h"

CardsInstall::CardsInstall(const CardsArgumentParser& argParser)
	: m_argParser(argParser), m_root("/")
{
}
void CardsInstall::run(int argc, char** argv)
{
	m_configParser = new ConfigParser("/etc/cards.conf");
	m_packageName = m_argParser.otherArguments()[0];
	ConfigParser::parseConfig("/etc/cards.conf", m_config);
	m_configParser->parseMD5sumCategoryDirectory();
        m_configParser->parsePortsList();
        m_configParser->parseBasePackageList();
}
set<string> CardsInstall::getDirectDependencies()
{

	pkginfo_t infoDeps;
	set<string> packageNameDeps;

	if ( checkPackageNameExist(m_packageName)) {
		infoDeps.dependencies.insert(m_packageName);
		packageNameDeps.insert(m_packageName);
#ifndef NDEBUG
		cerr << m_packageName << " allready installed" << endl;
#endif
		return packageNameDeps;
	}
	if ( m_listOfDepotPackages.find(m_packageName) != m_listOfDepotPackages.end() )
		return  m_listOfDepotPackages[m_packageName].dependencies;
	if ( m_configParser->checkBinaryExist(m_packageName)) {
		m_packageFileName = m_configParser->getPackageFileName(m_packageName);
		if ( ! checkFileExist(m_packageFileName)) {
			m_configParser->downloadPackageFileName(m_packageName);
		}
		set<string> packageNameBuildNDeps;
		ArchiveUtils packageArchive(m_packageFileName);
		packageNameBuildNDeps = packageArchive.listofDependencies();
		for (std::set<string>::iterator it = packageNameBuildNDeps.begin();it != packageNameBuildNDeps.end();it++) {
			string Name = *it;
			infoDeps.dependencies.insert(Name.substr(0,Name.size()-10));
		}
	} else {
		/*
			We consider that this package doens't have any deps 
			Just add his name
		*/
		infoDeps.dependencies.insert(m_packageName);
	}

	if(!infoDeps.dependencies.empty())
		m_listOfDepotPackages[m_packageName] = infoDeps;
	return infoDeps.dependencies;
}
void CardsInstall::printDependenciesList()
{
	getListOfPackageNames(m_root);
	generateDependencies();
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		cout << *it << endl;
	}
}
void CardsInstall::generateDependencies()
{
	vector<string> dependenciesWeMustAdd, 
		depencenciestoSort; 
	// Insert the final package first
	dependenciesWeMustAdd.push_back(m_packageName);
	std::vector<string>::iterator vit;
	std::set<string>::iterator sit;
	while ( ! dependenciesWeMustAdd.empty() ) {
		vit = dependenciesWeMustAdd.begin();
		dependenciesWeMustAdd.erase(vit);
		m_packageName = *vit;
		set<string> directDependencies = getDirectDependencies();
		depencenciestoSort.push_back(m_packageName);
		for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
			if ( *sit == m_packageName )
				continue;
			for ( vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end();++vit) {
				if ( *sit == *vit) {
					dependenciesWeMustAdd.erase(vit);
					break;
				}
			}
		}
		if ( ! directDependencies.empty() ) {
			for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
				if ( m_packageName != *sit )
					dependenciesWeMustAdd.push_back(*sit);
			}
		}
	}
	bool found = false ;
	for ( std::vector<string>::reverse_iterator vrit = depencenciestoSort.rbegin(); vrit != depencenciestoSort.rend();++vrit) {
		found = false ;
		for ( vit = m_dependenciesList.begin(); vit != m_dependenciesList.end();vit++) {
			if ( *vrit == *vit ) {
				found = true ;
				break;
			}
		}
		if (!found)
			m_dependenciesList.push_back(*vrit);
	}
}
void CardsInstall::update()
{
	Db_lock lock(m_root, true);

	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);

	if ( ! checkPackageNameExist(m_packageName) ) {
		m_actualError = PACKAGE_NOT_PREVIOUSLY_INSTALL;
		treatErrors(m_packageName);
	}
	if ( m_configParser->checkBinaryExist(m_packageName)) {
		m_packageFileName = m_configParser->getPackageFileName(m_packageName);
		if ( ! checkFileExist(m_packageFileName)) {
			m_configParser->downloadPackageFileName(m_packageName);
		}
	} else {
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(m_packageName);
	}
	m_packageFileName = m_configParser->getPackageFileName(m_packageName);
	set<string> keep_list;

	// Reading the archiving to find a list of files
	pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);

	// Checking the rules
	readRulesFile();

	set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);

	set<string> conflicting_files = getConflictsFilesList(package.first, package.second);

	if (! conflicting_files.empty()) {
		copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
		 m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
	}

	// Remove metadata about the package removed
	removePackageFilesRefsFromDB(package.first);

	keep_list = getKeepFileList(package.second.files, m_actionRules);

	removePackageFiles(package.first, keep_list);

#ifndef NDEBUG
	cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
	// Run pre-install if exist
	extractAndRunPREfromPackage(m_packageFileName);


	// Installation progressInfo of the files on the HD
	installArchivePackage(m_packageFileName, keep_list, non_install_files);
	// Post install
	if (checkFileExist(PKG_POST_INSTALL)) {
		m_actualAction = PKG_POSTINSTALL_START;
		progressInfo();
		process postinstall(SHELL,PKG_POST_INSTALL, 0 );
		if (postinstall.executeShell()) {
			cerr << "WARNING Run post-install FAILED." << endl;
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
void CardsInstall::install()
{
	Db_lock lock(m_root, true);

	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);

	if  ( checkPackageNameExist(m_packageName) ) {
		m_actualError = PACKAGE_ALLREADY_INSTALL;
		treatErrors (m_packageName);
	}

	// Generate the dependencies
	generateDependencies();

	// Checking the rules
	readRulesFile();

	set<string> keep_list;

	// Let's go
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		m_packageName = *it;

		if ( ! m_configParser->checkBinaryExist(m_packageName)) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
		}
		m_packageFileName = m_configParser->getPackageFileName(m_packageName);

		if  ( checkPackageNameExist(m_packageName) ) {
			continue;
		}

		// Reading the archiving to find a list of files
		pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);
		set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);
#ifndef NDEBUG
		cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
		// Run pre-install if exist
		extractAndRunPREfromPackage(m_packageFileName);

		set<string> conflicting_files = getConflictsFilesList(package.first, package.second);

		if (! conflicting_files.empty()) {
			copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
			m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
			treatErrors("listed file(s) already installed, cannot continue... ");
		}

		// Installation progressInfo of the files on the HD
		installArchivePackage(m_packageFileName, keep_list, non_install_files);

		// Post install
		if (checkFileExist(PKG_POST_INSTALL)) {
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
set<string> CardsInstall::getKeepFileList(const set<string>& files, const vector<rule_t>& rules)
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
set<string> CardsInstall::applyInstallRules(const string& name, pkginfo_t& info, const vector<rule_t>& rules)
{
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
void CardsInstall::getInstallRulesList(const vector<rule_t>& rules, rule_event_t event, vector<rule_t>& found) const
{
	for (vector<rule_t>::const_iterator i = rules.begin(); i != rules.end(); i++) {
		if (i->event == event)
			found.push_back(*i);
	}
}
void CardsInstall::printHelp() const
{
	cout << endl
		<< "  You should make shure you have activate the available directories" << endl
		<< " containing the available binaries otherwise you will end up with" << endl
		<< " missing binaries. As a result your NuTyX wont be working as it" << endl
		<< " should." << endl << endl
		<< "  Be also aware that the install scenario will never try to compile" << endl
		<< " any port on it's own, this is not the same scenario." << endl << endl
		<< "usage: cards install <package name>" << endl
		<< "options:" << endl
		<< "   -H, --info       print this help and exit" << endl;
}
// vim:set ts=2 :
