/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgrepo.h"

namespace cards {
	
pkgrepo::pkgrepo(const std::string& fileName)
    : m_configFileName(fileName)
	, pkgadd("cards install")
{
	parse();
}
void pkgrepo::generateDependencies
	(const std::pair<std::string,time_t>& packageName)
{
	m_packageName=packageName.first;
	generateDependencies();
}
void pkgrepo::generateDependencies()
{
	std::vector< std::pair<std::string,time_t> > dependenciesWeMustAdd,depencenciestoSort;
	std::pair<std::string,time_t> PackageTime;
	PackageTime.first=m_packageName;
	PackageTime.second=0;

	/* Always insert the final package first (twice) */
	dependenciesWeMustAdd.push_back(PackageTime);
	dependenciesWeMustAdd.push_back(PackageTime);

	std::vector< std::pair<std::string,time_t> >::iterator vit;
	std::set< std::pair<std::string,time_t> >::iterator sit;
	std::string packageNameSignature, packageName, packageFileName;

	/* Main while loop */
	while ( ! dependenciesWeMustAdd.empty() ) {
		vit = dependenciesWeMustAdd.begin();
		packageName = vit->first;
		PackageTime = *vit;

		/* Erase the current treated packageName */
		dependenciesWeMustAdd.erase(vit);

		std::set< std::pair<std::string,time_t> > directDependencies;
		if ( getListOfPackages().find(packageName) != getListOfPackages().end() ) {
			directDependencies = getListOfPackages()[packageName].dependencies();
		} else {

			/* The packageName is not found in the listOfPackages */

			if ( checkBinaryExist(packageName)) {
				/* The packageName binary is found.
				 * Directs dependencies are not availables yes
				 * We need to get PackageFileName of the packageName
				 * And check it's signature
				 */

				packageFileName = getPackageFileName(packageName);
				packageNameSignature = getPackageFileNameSignature(packageName);

				/* archive PackageName exist
				 * Let's see if we need to download it */

			} else {
				/*
				 * FIXME
				m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_EXIST;
				treatErrors (packageName);
				*/
			}

			/* If the binary archive is not yet downloaded or is corrupted */
			if (!checkFileSignature(packageFileName, packageNameSignature)) {
				/* Try to get it */
				downloadPackageFileName(packageName);
			}
			directDependencies = getPackageDependencies(packageFileName);
		}

		/* If the package is not yet installed or not uptodate */
		if (!checkPackageNameBuildDateSame(PackageTime))
		{
			/* checkPackageNameBuildDateSame is no then add it */
			depencenciestoSort.push_back(PackageTime);
		}

		/* else checkPackageNameBuildDateSame is yes*/

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

	/* Let's revert the order of dependencies
	 * We need the last founds (lower level) first
	 */
	bool found = false ;
	for ( std::vector<std::pair<std::string,time_t>>::reverse_iterator vrit = depencenciestoSort.rbegin();
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
			m_dependenciesList.push_back(*vrit);
		}

		/* else no deps founds */

	}
}
void pkgrepo::downloadPackageFileName(const std::string& packageName)
{
	/* FIXME */
}
std::vector<std::pair<std::string,time_t>>&
pkgrepo::getDependenciesList()
{
	return m_dependenciesList;
}
void pkgrepo::addDependenciesList
	(std::pair<std::string,time_t>& name)
{
	m_dependenciesList.push_back(name);
}
std::set<std::string>& pkgrepo::getBinaryPackageList() {
	parse();
	std::string packageName;
	for (auto p : m_listOfPackages) {
		packageName = "("
		+ p.second.collection()
		+ ") "
		+ p.first
		+ " "
		+ p.second.version()
		+ " " 
		+ p.second.description();
		m_binaryPackageList.insert(packageName);
	}
	return m_binaryPackageList;
}
std::string& pkgrepo::getBinaryPackageInfo(const std::string& name)
{
	parse();
	if (m_listOfPackages.find(name) != m_listOfPackages.end()) {
		m_binaryPackageInfo = _("Name           : ")
			+ name + '\n'
			+ _("Description    : ")
			+ m_listOfPackages[name].description()
			+ '\n'
			+ _("Groups         : ")
			+ m_listOfPackages[name].group()
			+ '\n'
			+ _("URL            : ")
			+ m_listOfPackages[name].url()
			+ '\n'
			+ _("Version        : ")
			+ m_listOfPackages[name].version()
			+ '\n'
			+ _("Maintainer(s)  : ")
			+ m_listOfPackages[name].maintainer()
			+ '\n'
			+ _("Packager       : ")
			+ m_listOfPackages[name].packager()
			+ '\n';
	}
	return m_binaryPackageInfo;
}
void pkgrepo::parse()
{
	if (m_listOfPackages.size() > 0)
		return;

	cards:cache info;
	

	for (auto i : m_config.dirUrl()) {
		std::string repoFile = i.depot 
			+ "/"
			+ i.collection
			+ PKG_REPO;

		
		info.collection(i.collection);
		std::vector<std::string> repoFileContent;

		if (parseFile(repoFileContent, repoFile.c_str()) != 0) {
			std::cerr << "Cannot read the file: "
				<< repoFile
				<< std::endl
				<< "... continue with next"
				<< std::endl;
			continue; 
		}
		bool pkgFound = false;
		std::string::size_type pos;
		std::string pkgName;
		std::set<std::string> pkgSet;
		for (auto p : repoFileContent) {
			if (p[0] == '@')  {
				pos = p.find(".cards-");
				if ( pos != std::string::npos) {
					pkgName = p.substr(1,pos - 1);
					info.version(p.substr(pos + 7));
					info.fileName(p.substr(1));
					pkgFound = true;
				}
			}
			if (pkgFound)
				if (p[0] == 'D')
					info.description(p.substr(1));
			if (pkgFound)
				if (p[0] == 'U')
					info.url(p.substr(1));
			if (pkgFound)
				if (p[0] == 'L')
					info.license(p.substr(1));
			if (pkgFound)
				if (p[0] == 'M')
					info.maintainer(p.substr(1));
			if (pkgFound)
				if (p[0] == 'C')
					info.contributors(p.substr(1));
			if (pkgFound)
				if (p[0] == 'c')
					info.collection(p.substr(1));
			if (pkgFound)
				if (p[0] == 'g')
					info.group(p.substr(1));
			if (pkgFound)
				if (p[0] == 'P')
					info.packager(p.substr(1));
			if (pkgFound)
				if (p[0] == 's') {
					pkgSet.insert(p.substr(1));
				}
			if (pkgFound)
				if (p.size() == 0) {
					pkgFound = false;
					info.sets(pkgSet);
					pkgSet.clear();
					m_listOfPackages[pkgName] = info;
				}
		}
	}
}
repo_t& pkgrepo::getListOfPackages()
{
    if (m_listOfPackages.size() == 0)
        parse();

    return m_listOfPackages;
}
std::set<std::string>& pkgrepo::getListOfPackagesFromSet(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
        parse();

	for (auto p: m_listOfPackages) {
		for (auto s : m_listOfPackages[p.first].sets()) {
			if (s == name) {
				m_binarySetList.insert(p.first);
				break;
			}
		}
	}

	return m_binarySetList;
}
std::set<std::string>& pkgrepo::getListOfPackagesFromCollection(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
        parse();

	for (auto p: m_listOfPackages) {
		if (m_listOfPackages[p.first].collection() == name) {
			m_binaryCollectionList.insert(p.first);
		}
	}

	return m_binaryCollectionList;
}
bool pkgrepo::checkBinaryExist(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
        parse();

	return (m_listOfPackages.find(name) != m_listOfPackages.end());
}
std::string& pkgrepo::getPackageFileName(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

    m_packageFileName = m_listOfPackages[name].fileName();

    return m_packageFileName;
}
std::string& pkgrepo::getPackageFileNameSignature(const std::string& name)
{
    if (m_listOfPackages.size() == 0)
        parse();

    if (m_listOfPackages[name].signature().size() == 0)
        m_packageFileNameSignature = m_listOfPackages[name].sha256SUM();
    else
        m_packageFileNameSignature = m_listOfPackages[name].signature();

    return m_packageFileNameSignature;
}
std::vector<std::string> pkgrepo::getListofGroups()
{
	return m_config.groups();
}
time_t pkgrepo::getBinaryBuildTime (const std::string& name)
{

	if (m_listOfPackages.size() == 0)
		parse();

	return m_listOfPackages[name].build();
}
std::string& pkgrepo::getPackageVersion(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
		parse();

	m_packageVersion = m_listOfPackages[name].version();
	return m_packageVersion;
}
unsigned short int pkgrepo::getPackageRelease(const std::string& name)
{
	if (m_listOfPackages.size() == 0)
		parse();

	return m_listOfPackages[name].release();
}
} // end of 'cards' namespace
