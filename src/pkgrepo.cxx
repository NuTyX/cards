/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkgrepo.h"

namespace cards {
	
pkgrepo::pkgrepo(const std::string& fileName)
    : m_configFileName(fileName)
{
}
std::set<std::string>& pkgrepo::getBinaryPackageList() {
	parseRepofile();
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
	parseRepofile();
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
void pkgrepo::parseRepofile()
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
				if (p[0] == 'R') {
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

} // end of 'cards' namespace
