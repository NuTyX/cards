/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkginst.h"

pkginst::pkginst
	(const std::string& commandName,const char *configFileName)
	: pkgadd(commandName), repodwl(configFileName)
{
}
void pkginst::generateDependencies
	(const std::pair<std::string,time_t>& packageName)
{
	m_packageName=packageName.first;
	generateDependencies();
}
void pkginst::generateDependencies()
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
				m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_EXIST;
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
