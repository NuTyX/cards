//
//  pkginst.cxx
//
//  Copyright (c) 2015-2017 by NuTyX team (http://nutyx.org)
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

using namespace std;

Pkginst::Pkginst(const string& commandName,const char *configFileName)
	: Pkgadd(commandName), Repodwl(configFileName)
{
}
void Pkginst::generateDependencies(const std::pair<std::string,time_t>& packageName)
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
#ifndef NDEBUG
			cerr << "getPackageDependencies(" << m_packageFileName << ")" << endl;
#endif
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
		for ( auto sit : directDependencies) {
			if ( sit.first == PackageTime.first )
				continue;
			for ( vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end();++vit) {
				if ( sit == *vit) {
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
	for ( std::vector<pair <string,time_t> >::reverse_iterator vrit = depencenciestoSort.rbegin();
		vrit != depencenciestoSort.rend();
		++vrit) {
		found = false ;
		for (auto i : m_dependenciesList) {
			if ( i == vrit->first) {
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
// vim:set ts=2 :
