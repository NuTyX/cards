//
//  pkginst.cxx
//
//  Copyright (c) 2015 - 2023 by NuTyX team (http://nutyx.org)
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

Pkginst::Pkginst
	(const std::string& commandName,const char *configFileName)
	: Pkgadd(commandName), Repodwl(configFileName)
{
}
void Pkginst::generateDependencies
	(const std::pair<std::string,time_t>& packageName)
{
	m_packageName=packageName.first;
	generateDependencies();
}
void Pkginst::generateDependencies()
{
	std::vector< std::pair<std::string,time_t> > dependenciesWeMustAdd,depencenciestoSort;
	std::pair<std::string,time_t> PackageTime;
	PackageTime.first=m_packageName;
	PackageTime.second=0;
	dependenciesWeMustAdd.push_back(PackageTime);	// Insert the final package first
	dependenciesWeMustAdd.push_back(PackageTime);
	std::vector< std::pair<std::string,time_t> >::iterator vit;
	std::set< std::pair<std::string,time_t> >::iterator sit;
	std::string packageNameSignature, packageName, packageFileName;

	while ( ! dependenciesWeMustAdd.empty() ) { // Main WHILE
#ifndef NDEBUG
		for (auto i : dependenciesWeMustAdd)
			std::cerr << i.first << " " << i.second << std::endl;
#endif
		vit = dependenciesWeMustAdd.begin();
		packageName = vit->first;
		PackageTime = *vit;
		dependenciesWeMustAdd.erase(vit);   // Erase the current treated package name
		std::set< std::pair<std::string,time_t> > directDependencies;
		if ( m_listOfRepoPackages.find(packageName) != m_listOfRepoPackages.end() ) {
#ifndef NDEBUG
			std::cerr << packageName
				<< " found in m_listOfRepoPackages"
				<< std::endl;
#endif
			directDependencies = m_listOfRepoPackages[packageName].dependencies();
#ifndef NDEBUG
			for (auto i : directDependencies )
				std::cerr << i.first
					<< i.second << " ";
#endif
		} else {
#ifndef NDEBUG
			std::cerr << packageName
				<< " not found in m_listOfRepoPackages"
				<< std::endl;
#endif
			if ( checkBinaryExist(packageName)) { // directs deps if not yet availables
#ifndef NDEBUG
				std::cerr << packageName
					<< " binary found "
					<< std::endl;
#endif
				packageFileName = getPackageFileName(packageName);
				packageNameSignature = getPackageFileNameSignature(packageName);

#ifndef NDEBUG
				std::cerr << packageFileName
					<< " archive found "
					<< std::endl;
#endif
			} else {
				m_actualError = PACKAGE_NOT_EXIST;
				treatErrors (packageName);

			}
			if ( ! checkFileSignature(packageFileName, packageNameSignature)) // Binary Archive not yet downloaded or corrupted
				downloadPackageFileName(packageName); // Get it
#ifndef NDEBUG
			std::cerr << "getPackageDependencies("
				<< packageFileName << ")"
				<< std::endl;
#endif
			directDependencies = getPackageDependencies(packageFileName);
#ifndef NDEBUG
			for (auto i : directDependencies )
				std::cerr << i.first << " ";
#endif
		}
		if ( ! checkPackageNameBuildDateSame(PackageTime)) // If not yet install or not up to dated
		{
#ifndef NDEBUG
			std::cerr << "checkPackageNameBuildDateSame no:"
				<<PackageTime.first
				<< PackageTime.second
				<< std::endl;
#endif
			depencenciestoSort.push_back(PackageTime); // Add it
		}
#ifndef NDEBUG
			 else
		{
			std::cerr << "checkPackageNameBuildDateSame yes: "
				<< PackageTime.first
				<< PackageTime.second
				<< std::endl;
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
	for ( std::vector<std::pair <std::string,time_t> >::reverse_iterator vrit = depencenciestoSort.rbegin();
		vrit != depencenciestoSort.rend();
		++vrit) {
		found = false ;
		for (auto i : m_dependenciesList) {
			if ( i.first == vrit->first) {
				found = true ;
				break;
			}
		}
		if (!found) {
#ifndef NDEBUG
			std::cerr << "m_dependenciesList.push_back "
				<< vrit->first
				<< " "
				<< vrit->second
				<< std::endl;
#endif
			m_dependenciesList.push_back(*vrit);
		}
#ifndef NDEBUG
		else std::cerr << "no deps founds" << std::endl;
#endif
	}
}
// vim:set ts=2 :
