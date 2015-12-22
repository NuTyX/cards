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

Pkginst::Pkginst(const string& commandName, const string& configFileName)
	: Pkgdbh(commandName), Pkgrepo(configFileName)
{
	m_force=false;
	getListOfPackageNames("/");
	buildDatabaseWithDetailsInfos(false);
	parsePkgRepoCollectionFile();
}
void Pkginst::generateDependencies(const string& packageName)
{
	m_packageName=packageName;
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
		for (auto i : dependenciesWeMustAdd) cout << i.first << " " << i.second << endl;
		vit = dependenciesWeMustAdd.begin();
		m_packageName = vit->first;
		PackageTime = *vit;
		dependenciesWeMustAdd.erase(vit);   // Erase the current treated package name
		set< pair<string,time_t> > directDependencies;
		if ( m_listOfDepotPackages.find(m_packageName) != m_listOfDepotPackages.end() ) {
			cout << m_packageName << " found " << endl;
			directDependencies = m_listOfDepotPackages[m_packageName].dependencies;
		} else {
			cout << m_packageName << " not  found " << endl;
			if ( checkBinaryExist(m_packageName)) { // directs deps if not yet availables
				cout << m_packageName << " binary found " << endl;
				m_packageFileName = getPackageFileName(m_packageName);
				cout << m_packageFileName << " archive found " << endl;
			}
			if ( ! checkFileExist(m_packageFileName)) // Binary Archive not yet downloaded
				downloadPackageFileName(m_packageName); // Get it
			directDependencies = getPackageDependencies(m_packageFileName);
			for (auto i : directDependencies ) cout << i.first << endl;
		}
		if ( ! checkPackageNameBuildDateSame(PackageTime)) // If not yet install or not up to dated
		{
			cout << PackageTime.first << PackageTime.second << endl;
			depencenciestoSort.push_back(PackageTime); // Add it
		} else
		{
			cout << PackageTime.first << PackageTime.second << endl;
		}
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
			cout << vrit->first << endl;
			m_dependenciesList.push_back(vrit->first);
		}
	}
}
void Pkginst::addPackage(const bool& installed)
{
	// New LockDatabase pointer
	Db_lock  * pLock = NULL;	// New LockDatabase pointer
	readRulesFile();					// Checking the rules
	set<string> keep_list;
	pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);
	set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);
#ifndef NDEBUG
	cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
	extractAndRunPREfromPackage(m_packageFileName);	// Run pre-install if exist
	set<string> conflicting_files = getConflictsFilesList(package.first, package.second);
	if (! conflicting_files.empty()) {
		if (m_force) {
			set<string> keep_list;
			removePackageFilesRefsFromDB(conflicting_files, keep_list); // Remove unwanted conflicts
		} else {
			copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
			m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
			treatErrors("'" + m_packageName + "': listed file(s) already installed, cannot continue... ");
		}
	}
	pLock = new Db_lock(m_root, true);	// Lock the database, any interruption forbidden
	installArchivePackage(m_packageFileName, keep_list, non_install_files);	// Installation progressInfo of the files on the HD
	moveMetaFilesPackage(package.first,package.second);	// Add the metadata about the package to the DB
	
}
void Pkginst::addPackagesList()
{
#ifndef NDEBUG
	cerr << "Number of Packages : " << m_dependenciesList.size()<< endl;
#endif
	for (auto i : m_dependenciesList) {
		m_packageName = i;
#ifndef NDEBUG
		cerr << i << endl;
#endif
		m_packageFileName = getPackageFileName(m_packageName);
#ifndef NDEBUG
		cerr << m_packageFileName << endl;
#endif
		if  ( checkPackageNameExist(m_packageName) ) {
			addPackage(true);
			continue;
		}

		if ( ! checkBinaryExist(m_packageName)) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
		}
		addPackage(false);
	}
}
// vim:set ts=2 :
