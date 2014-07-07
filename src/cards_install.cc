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
	
	m_packageName = m_argParser.otherArguments()[0];
	ConfigParser::parseConfig("/etc/cards.conf", m_config);
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	string url,categoryDir,categoryMD5sumFile, remotePackageDirectory ;
#ifndef NDEBUG
	cerr << "Synchronizing start ..." << endl;
#endif
	for (vector<string>::iterator i = m_config.dirUrl.begin();i != m_config.dirUrl.end(); ++i) {
		string val = *i ;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			categoryDir = stripWhiteSpace(val.substr(0,pos));
			url = stripWhiteSpace(val.substr(pos+1));
			categoryMD5sumFile = categoryDir + "/MD5SUM" ;
			// Download the MD5SUM file of the category silently
			FileDownload MD5Sum(url + "/MD5SUM",
				categoryDir,
				"MD5SUM", false);
			MD5Sum.downloadFile();
			vector<string> MD5SUMFileContent;
			if ( parseFile(MD5SUMFileContent,categoryMD5sumFile.c_str()) != 0) {
				m_actualError = CANNOT_READ_FILE;
				treatErrors(categoryMD5sumFile);
			}
			for ( vector<string>::iterator i = MD5SUMFileContent.begin();i!=MD5SUMFileContent.end();++i) {
				m_MD5packagesNameVersionList.push_back(val + "|" + *i);
			}
		} else {
			continue;
		}
		remove(categoryMD5sumFile.c_str());
	}
#ifndef NDEBUG
	cout << "Synchronizing done." << endl;
	for (vector<string>::iterator i = m_MD5packagesNameVersionList.begin();i!=m_MD5packagesNameVersionList.end();++i) {
		cerr << *i << endl ;
	}
	cerr << "Number of Packages: " << MD5packagesNameVersionList.size() << endl;
#endif
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
	if ( getPackageFileName()) {
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
bool CardsInstall::getPackageFileName()
{
	string basePackageName, packageFileName, categoryDir, url, PackageDirectory; 	
	string::size_type pos = m_packageName.rfind('.');
	bool found = false;

	if (pos != string::npos) {
		basePackageName=m_packageName.substr(0,pos);
	} else {
		basePackageName=m_packageName;
	}

	for (vector<string>::iterator i = m_MD5packagesNameVersionList.begin();i != m_MD5packagesNameVersionList.end(); ++i) {
		string val = *i ;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			categoryDir = stripWhiteSpace(val.substr(0,pos));
#ifndef NDEBUG
			cerr << categoryDir << endl;
#endif
		}
		string tmpUrl = val.substr(pos+1);
		pos = tmpUrl.find('|');
		if (pos != string::npos) {
			url = stripWhiteSpace(tmpUrl.substr(0,pos));
#ifndef NDEBUG
			cout << url << endl;
#endif
		}
		PackageDirectory = tmpUrl.substr(pos+1);
		if ( PackageDirectory.size() < 34 ) {
			m_actualError = CANNOT_PARSE_FILE;
			treatErrors(PackageDirectory + ": missing info");
		}
		if ( PackageDirectory[32] != ':' ) {
			m_actualError = CANNOT_PARSE_FILE;
			treatErrors(PackageDirectory + ": wrong format");
		}
		pos = PackageDirectory.find('@');
		if (pos != string::npos) {
			string packageName = PackageDirectory.substr(33,pos - 33);
			if ( basePackageName == packageName ) {
				found = true;
				break;
			}
		}
	}
	if (checkPackageNameExist(m_packageName)) {
		return false;
	}
	if (found) {
	/*
		If found, the port should exist
		We should check if the binary package is available
		MD5SUMFile is /var/lib/pkg/saravane/server/alsa-lib@1.2.3.4/MD5SUM
	*/
		string packageDirectoryName = categoryDir + "/" + PackageDirectory.substr(33)+"/";
		string MD5SUMFile = packageDirectoryName + "/MD5SUM";
		if ( ! checkFileExist(MD5SUMFile) ) {
			FileDownload MD5Sum(url + "/" + PackageDirectory.substr(33)+"/MD5SUM",
				categoryDir+ "/" + PackageDirectory.substr(33)+"/",
				"MD5SUM", false);
			MD5Sum.downloadFile();
		}
	/*
		We have the /var/lib/pkg/saravane/server/alsa-lib@1.2.3.4/MD5SUM file 
		Need to check if the binary is available
	*/
		bool found = false;
		vector<string> MD5SUMFileContent;
		if ( parseFile(MD5SUMFileContent,MD5SUMFile.c_str()) != 0) {
			m_actualError = CANNOT_READ_FILE;
			treatErrors(MD5SUMFile);
		}
		string packageExtension = "";
		string packageBuildDate = "";
		for (vector<string>::const_iterator i = MD5SUMFileContent.begin();i != MD5SUMFileContent.end(); ++i) {
			string input = *i;
			if (input.size() < 11) {
				cout << "Wrong Format, field to small: " << input << endl;
				continue;
			}
			if (input[10] == ':' ) {	// There is a chance to find what we are looking for
				packageBuildDate = input.substr(0,10);
				packageExtension = input.substr(11,input.size());
				found = true;
				continue;
			}
			if (found) {
		/*
			They are some binaries, the one I need exist ?
		*/
				string packageNameMD5SUM = input.substr(0,32);
				string fileNameArch = input.substr(33);
				string::size_type pos = fileNameArch.find(':');
				if ( pos != std::string::npos) { // format input OK (md5sum:name:arch)
					string myFileName;
					string fileName = input.substr(33,pos) +
						packageBuildDate +
						input.substr(34+pos) +
						packageExtension;
					if ( m_packageName == basePackageName ) {
					/*
						package we are looking for
						IS the base package means it's
						arch is define in /etc/cards.conf
					*/
						myFileName = m_packageName +
							packageBuildDate +
							m_config.arch +
							packageExtension;
							
					} else {
					/*
						package we are loocking for
						IS a sub package with "any" as arch
					*/
						myFileName = input.substr(33,pos) +
							packageBuildDate +
							"any" +
							packageExtension;
					}
					if ( myFileName == fileName ) {	// Finally that's the one we need
						m_packageFileName = packageDirectoryName + fileName;
						if (! checkFileExist(m_packageFileName)) {
						// Ok need to download it
							FileDownload packageFileNameArchive(url + "/" +
								PackageDirectory.substr(33) + "/" +
								fileName,
								categoryDir+ "/" + PackageDirectory.substr(33)+"/",
								fileName,true);
							packageFileNameArchive.downloadFile();
						}
						// YEEESSS we got it
						return true;
					}
				}
			}	
		}
		/*
			if we arrive here it means that they was no binaries found yet
			So we assume the package is exist but has no runtime dependencies
			which can be the case if allready install and/or from the basic
			system
		*/
		if ( ! checkPackageNameExist(m_packageName)) 
			cerr << "WARNING " << m_packageName << " not yet compiled... " << endl;
		return false;
	} else {
		/*
			not found in any category this package name don't exist yet
			TODO Need to make it clear for the user, at the moment just return false
			throwing an error and interrupt the process is a good idea ?
		*/
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(m_packageName);
		return false;
	}
}
void CardsInstall::install()
{
	Db_lock lock(m_root, true);

	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);

	// Generate the dependencies
	generateDependencies();

	// Checking the rules
	readRulesFile();

	// Lets go
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		m_packageName = *it;
		if ( checkPackageNameExist(m_packageName))
			continue;
		if ( ! getPackageFileName() ) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
		};
		// Reading the archiving to find a list of files
		pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);

		if  ( checkPackageNameExist(m_packageName)) {
			cout << *it << " is allready installed, will not be reinstalled" << endl;
			continue;
		}
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

		set<string> keep_list;

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
