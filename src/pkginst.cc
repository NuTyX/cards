//  pkginst.cc
//
//  Copyright (c) 2015 by NuTyX team (http://nutyx.org)
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
#include "pkginst.h"

Pkginst::Pkginst(const string& commandName, const string& configFileName, const bool& silent)
	: Pkgdbh(commandName), Pkgrepo(configFileName)
{
	getListOfPackageNames("/");
	buildDatabaseWithDetailsInfos(silent);
	parsePkgRepoCollectionFile();
}
void Pkginst::generateDependencies(const pair<string,time_t>& packageName)
{
	m_packageName=packageName.first;
	generateDependencies();
}
void Pkginst::generateDependencies()
{
	vector< pair<string,time_t> > dependenciesWeMustAdd,depencenciestoSort;
	pair<string,time_t> PackageTime;
	PackageTime.first=m_packageName;
	PackageTime.second=0;
	dependenciesWeMustAdd.push_back(PackageTime);	// Insert the final package first
	dependenciesWeMustAdd.push_back(PackageTime);
	std::vector< pair<string,time_t> >::iterator vit;
	std::set< pair<string,time_t> >::iterator sit;
	while ( ! dependenciesWeMustAdd.empty() ) { // Main WHILE
#ifndef NDEBUG
		for (auto i : dependenciesWeMustAdd) cerr << i.first << " " << i.second << endl;
#endif
		vit = dependenciesWeMustAdd.begin();
		m_packageName = vit->first;
		PackageTime = *vit;
		dependenciesWeMustAdd.erase(vit);   // Erase the current treated package name
		set< pair<string,time_t> > directDependencies;
		if ( m_listOfDepotPackages.find(m_packageName) != m_listOfDepotPackages.end() ) {
#ifndef NDEBUG
			cerr << m_packageName << " found in m_listOfDepotPackages" << endl;
#endif
			directDependencies = m_listOfDepotPackages[m_packageName].dependencies;
#ifndef NDEBUG
			for (auto i : directDependencies ) cerr << i.first << i.second << " ";
#endif
		} else {
#ifndef NDEBUG
			cerr << m_packageName << " not found in m_listOfDepotPackages" << endl;
#endif
			if ( checkBinaryExist(m_packageName)) { // directs deps if not yet availables
#ifndef NDEBUG
				cerr << m_packageName << " binary found " << endl;
#endif
				m_packageFileName = getPackageFileName(m_packageName);
#ifndef NDEBUG
				cerr << m_packageFileName << " archive found " << endl;
#endif
			}
			if ( ! checkFileExist(m_packageFileName)) // Binary Archive not yet downloaded
				downloadPackageFileName(m_packageName); // Get it
			directDependencies = getPackageDependencies(m_packageFileName);
#ifndef NDEBUG
			for (auto i : directDependencies ) cerr << i.first << " ";
#endif
		}
		if ( ! checkPackageNameBuildDateSame(PackageTime)) // If not yet install or not up to dated
		{
#ifndef NDEBUG
			cerr << "checkPackageNameBuildDateSame no:" <<PackageTime.first << PackageTime.second << endl;
#endif
			depencenciestoSort.push_back(PackageTime); // Add it
		}
#ifndef NDEBUG
			 else
		{
			cerr << "checkPackageNameBuildDateSame yes: " << PackageTime.first << PackageTime.second << endl;
		}
#endif
		for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
			if ( sit->first == PackageTime.first )
				continue;
			for ( vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end();++vit) {
				if ( *sit == *vit) {
					dependenciesWeMustAdd.erase(vit);
					break;
				}
			}
		}
		for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
			if ( PackageTime.first != sit->first ) {
				if ( ! checkPackageNameBuildDateSame(*sit))
					dependenciesWeMustAdd.push_back(*sit);
			}
		}
	}
	bool found = false ;
	for ( std::vector<pair <string,time_t> >::reverse_iterator vrit = depencenciestoSort.rbegin(); vrit != depencenciestoSort.rend();++vrit) {
		found = false ;
		for (vector<string>::const_iterator i = m_dependenciesList.begin();i != m_dependenciesList.end();i++) {
			if (*i == vrit->first) {
				found = true ;
				break;
			}
		}
		if (!found) {
#ifndef NDEBUG
			cerr << "m_dependenciesList.push_back " << vrit->first << endl;
#endif
			m_dependenciesList.push_back(vrit->first);
		}
#ifndef NDEBUG
		else cerr << "no deps founds" << endl;
#endif
	}
}
void Pkginst::addPackage(const bool& force)
{
	Db_lock  * pLock = NULL;	// New LockDatabase pointer
	readRulesFile();					// Checking the rules
	pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);
	readRulesFile();  // Checking the rules
	bool upgrade = checkPackageNameExist(package.first);
	set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);

	if (!upgrade) {
#ifndef NDEBUG
		cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
		extractAndRunPREfromPackage(m_packageFileName);	// Run pre-install if exist
	}
	set<string> conflicting_files = getConflictsFilesList(package.first, package.second);
	if (! conflicting_files.empty()) {
		if (force) {
			pLock = new Db_lock(m_root, true);
			set<string> keep_list;
			if (upgrade)	// Don't remove files matching the rules in configuration
				keep_list = getKeepFileList(conflicting_files, m_actionRules);
			removePackageFilesRefsFromDB(conflicting_files, keep_list); // Remove unwanted conflicts
			delete pLock;
		} else {
			copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
			m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
			treatErrors("'" + m_packageName + "': listed file(s) already installed, cannot continue... ");
		}
	}
	set<string> keep_list;
	if (upgrade) {
		pLock = new Db_lock(m_root, true);	// Lock the database, any interruption forbidden
		removePackageFilesRefsFromDB(package.first);	// Remove metadata about the package removed
		keep_list = getKeepFileList(package.second.files, m_actionRules);
		removePackageFiles(package.first, keep_list);
		delete pLock;
#ifndef NDEBUG
		cerr << "Run extractAndRunPREfromPackage after upgrade" << endl;
#endif
		extractAndRunPREfromPackage(m_packageFileName);	// Run pre-install if exist
	}
	pLock = new Db_lock(m_root, true);
	installArchivePackage(m_packageFileName, keep_list, non_install_files);	// Installation progressInfo of the files on the HD
	moveMetaFilesPackage(package.first,package.second);	// Add the metadata about the package to the DB
	addPackageFilesRefsToDB(package.first, package.second); // Add the info about the files to the DB
	delete pLock;
	if (checkFileExist(PKG_POST_INSTALL)) { // Post install
		m_actualAction = PKG_POSTINSTALL_START;
		progressInfo();
		process postinstall(SHELL,PKG_POST_INSTALL, 0 );
		if (postinstall.executeShell())
			cerr << "WARNING Run post-install FAILED. continue" << endl;
		m_actualAction = PKG_POSTINSTALL_END;
		progressInfo();
		removeFile(m_root,PKG_POST_INSTALL);
	}
	runLdConfig();
}
void Pkginst::addPackagesList(const bool& force)
{
#ifndef NDEBUG
	cerr << "Number of Packages : " << m_dependenciesList.size()<< endl;
#endif
	buildDatabaseWithDetailsInfos(false);	// Retrieving info about all the packages
	for (auto i : m_dependenciesList) {
		m_packageName = i;
#ifndef NDEBUG
		cerr << i << endl;
#endif
		m_packageFileName = getPackageFileName(m_packageName);
#ifndef NDEBUG
		cerr << m_packageFileName << endl;
#endif
		if ( ! checkBinaryExist(m_packageName)) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
		}
		addPackage(force);
	}
}
// vim:set ts=2 :
