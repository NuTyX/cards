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
set<string> CardsInstall::findPackages(const string& path)
{
	set<string> packageList;
	if (findFile(packageList, path) != 0) {
		m_actualError = CANNOT_READ_DIRECTORY;
		treatErrors(path);
	}
	return packageList;
}
set<string> CardsInstall::getDirectDependencies()
{

	pkginfo_t infoDeps;
	set<string> packageNameDeps;
#ifndef NDEBUG
	cerr << "----> Begin of Direct Dependencies" << endl;
#endif
	if ( checkPackageNameExist(m_packageName)) {
#ifndef NDEBUG
		cerr << m_packageName << " allready installed" << endl;
		cerr << "----> NO Direct Dependencies" << endl;
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
#ifndef NDEBUG
		cerr << "Direct Deps of : " << m_packageFileName << endl;
#endif
		for (std::set<string>::iterator it = packageNameBuildNDeps.begin();it != packageNameBuildNDeps.end();it++) {
			string Name = *it;
#ifndef NDEBUG
			cerr << Name.substr(0,Name.size()-10)  << " is a direct dep of " << m_packageFileName << endl;
#endif
			infoDeps.dependencies.insert(Name.substr(0,Name.size()-10));
		}
	} else {
		/*
			We consider that this package doens't have any deps 
			Just add his name
		*/
#ifndef NDEBUG
		cerr << m_packageName << " allready installed" << endl;
#endif
	}

	if(!infoDeps.dependencies.empty())
		m_listOfDepotPackages[m_packageName] = infoDeps;
#ifndef NDEBUG
	cerr << "----> End of Direct Dependencies" << endl;
#endif
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
		m_packageName = *vit;
#ifndef NDEBUG
		cerr << "--> Begin of Main WHILE\n m_packageName = " << m_packageName  << endl;
#endif
		/* If m_packageName is allready installed no need checkit*/
		if ( checkPackageNameExist(m_packageName)) {
#ifndef NDEBUG
			cerr << m_packageName << " is allready installed no need checkit" << endl;
#endif			
			dependenciesWeMustAdd.erase(vit);
			continue;
		}
		dependenciesWeMustAdd.erase(vit); /* Erase the first one in the dependenciesWeMustAdd list */
		set<string> directDependencies = getDirectDependencies();
		/* If m_packageName is allready in the depencenciestoSort list  AND ...*/
		bool found = false;
		for ( vit = depencenciestoSort.begin(); vit != depencenciestoSort.end();++vit) {
			if ( m_packageName == *vit ) {
				found=true;
				break;
			}
		}
		/* ... AND m_packageName have dependencies, no need to check it again */

		if ( (found) && (! directDependencies.empty() ) ) {
#ifndef NDEBUG
			cerr << m_packageName << " is allready in the depencenciestoSort list and HAVE some dependencies, no need to check it again" << endl;
#endif
//			dependenciesWeMustAdd.erase(vit);
			continue;
		}
		if ( ! checkPackageNameExist(m_packageName)) {
#ifndef NDEBUG
			for (std::set<string>::iterator it = directDependencies.begin();it != directDependencies.end();it++) {
				cerr << "dd: " << *it << " ";
			}
			cerr << endl;
#endif
			if ( ! checkPackageNameExist(m_packageName)) {
#ifndef NDEBUG
				cerr << m_packageName << " push back in dependenciestoSort" << endl;
#endif
				depencenciestoSort.push_back(m_packageName);
			}
		}
		for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
			if ( *sit == m_packageName ) {
#ifndef NDEBUG
				cerr << m_packageName << " allready found in directDependencies" << endl;
#endif
				continue;
			}
			for ( vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end();++vit) {
				if ( *sit == *vit) {
#ifndef NDEBUG
					cerr << *vit << " erase from dependenciesWeMustAdd" << endl;
#endif
					dependenciesWeMustAdd.erase(vit);
					break;
				}
			}
		}
		if (  ! directDependencies.empty() ) {
			for ( sit = directDependencies.begin(); sit != directDependencies.end();sit++) {
				if ( m_packageName != *sit ) {
					if ( ! checkPackageNameExist(*sit)) {
#ifndef NDEBUG
						cerr << *sit << " not installed, push back in dependenciesWeMustAdd" << endl;
#endif

						dependenciesWeMustAdd.push_back(*sit);
					}
				}
			}
		}
#ifndef NDEBUG
		cerr << "dependenciesWeMustAdd: " << endl;
		for ( vit = dependenciesWeMustAdd.begin(); vit != dependenciesWeMustAdd.end();++vit) {
			cerr << *vit << ", ";
		}
		cerr << endl << "depencenciestoSort: " << endl;
		for ( vit = depencenciestoSort.begin(); vit != depencenciestoSort.end();++vit) {
			cerr << *vit << ", ";
		}
		
		cerr << endl << "--> End of Main WHILE" << endl << endl;
#endif
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
		treatErrors("listed file(s) already installed, cannot continue... ");	
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

	// Add the locales if any defined
	if (m_config.locale.size() > 0) {
		vector<string> tmpList;
		string packageName = "";
		for (std::vector<string>::iterator i = m_config.locale.begin();i != m_config.locale.end();++i) {
			for (std::vector<string>::iterator j = m_dependenciesList.begin(); j != m_dependenciesList.end();j++) {
				packageName = *j + "." + *i;
				if ( m_configParser->checkBinaryExist(packageName)) {
    			m_packageFileName = m_configParser->getPackageFileName(packageName);
    			if ( ! checkFileExist(m_packageFileName)) {
      			m_configParser->downloadPackageFileName(packageName);
    			}
					tmpList.push_back(packageName);
				}
			}
		}
		if (tmpList.size() > 0 ) {
			for (std::vector<string>::iterator i = tmpList.begin();i != tmpList.end();++i) {
				m_dependenciesList.push_back(*i);
			}
		}
	}
	addPackagesList();
}
void CardsInstall::addPackagesList()
{
	// Checking the rules
	readRulesFile();

	set<string> keep_list;

	// Let's go
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		m_packageName = *it;
		m_packageFileName = m_configParser->getPackageFileName(m_packageName);

		if  ( checkPackageNameExist(m_packageName) ) {
			continue;
		}

		if ( ! m_configParser->checkBinaryExist(m_packageName)) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
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
void CardsInstall::install(const vector<string>& dependenciesList)
{

	Db_lock lock(m_root, true);

	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);
	m_configParser = new ConfigParser("/etc/cards.conf");
	m_configParser->parseCategoryDirectory();
	m_configParser->parsePortsList();
	m_configParser->parseBasePackageList();
	std::set<string> listOfPackages;

	for (std::vector<string>::const_iterator it = dependenciesList.begin(); it != dependenciesList.end();it++) {
		listOfPackages = m_configParser->getListOfPackagesFromDirectory(*it);
		for (std::set<string>::const_iterator i = listOfPackages.begin(); i != listOfPackages.end();i++) {
			if ( ! checkPackageNameExist(*i) ) {
				if ( m_configParser->checkBinaryExist(*i)) {
					cout << "ADD FOR INSTALL: " << *i << endl;
					m_dependenciesList.push_back(*i);
				}
			}
		}
	}
	addPackagesList();
	
}
void CardsInstall::create()
{
	m_packageName = const_cast<char*>(m_argParser.otherArguments()[0].c_str());
	ConfigParser::parseConfig("/etc/cards.conf", m_config);
	m_configParser = new ConfigParser("/etc/cards.conf");
	m_configParser->parseCategoryDirectory();
	m_configParser->parsePortsList();
	string pkgdir = m_configParser->getPortDir(m_packageName);
	if (pkgdir == "" ) {
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(m_packageName);
	}
	string timestamp;
	string commandName = "cards create: ";
	string message;
	int fdlog = -1;

	if ( m_config.logdir != "" ) {
		if ( ! createRecursiveDirs(m_config.logdir) ) {
			m_actualError = CANNOT_CREATE_DIRECTORY;
			treatErrors(m_config.logdir);
		}
		string logFile = m_config.logdir + "/" + m_packageName + ".log";
		unlink( logFile.c_str() );
		fdlog = open(logFile.c_str(),O_APPEND | O_WRONLY | O_CREAT, 0666 );
		if ( fdlog == -1 ) {
			m_actualError = CANNOT_OPEN_FILE;
			treatErrors(logFile);
		}
	}
	message = commandName + pkgdir + " package(s)";
	cout << message << endl;

	if ( m_config.logdir != "" ) {		
		write( fdlog, message.c_str(), message.length());
		time_t startTime;
		time(&startTime);
		timestamp = ctime(&startTime);
		timestamp = " starting build " + timestamp;
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
	}
	chdir( pkgdir.c_str() );

	string runscriptCommand = "sh";
	string cmd = "pkgmk";
	string args = "-d ";

	process makeprocess( cmd, args, fdlog );
	int result = 0 ;
	result = makeprocess.executeShell();
#ifndef NDEBUG
	cerr << result << endl;
#endif
	switch ( result )
	{
			case 1:
				throw runtime_error("General error");
				break;
			case 2:
				throw runtime_error("Error Invalid Pkgfile");
				break;
			case 3:
				throw runtime_error("Error Sources /build directory missing or missing read/write permission");
				break;
			case 4:
				throw runtime_error("Error during download of sources file(s)");
				break;
			case 5:
				throw runtime_error("Error during unpacking of sources file(s)");
				break;
			case 6:
				throw runtime_error("Error md5sum from sources Checking");
				break;
			case 7:
				throw runtime_error("Error footprint Checking");
				break;
			case 8:
				throw runtime_error("Error while running 'build()'");
				break;
			case 10:
				throw runtime_error("Error searching runtime dependancies");
				break;
			case 11:
				throw runtime_error(pkgdir + "/" + m_packageName +".info not found");
				break;
	}
	if (result > 0) { //TODO find out why return code is wrong
		m_actualError = CANNOT_PARSE_FILE;
		treatErrors(pkgdir+ "/Pkgfile");
	} 
	if ( m_config.logdir != "" ) {
		time_t endTime;
		time(&endTime);
		timestamp = ctime(&endTime);
		timestamp = commandName + "build done " + timestamp;

		write( fdlog, "\n", 1 );
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
	}

	// Let's install them
	m_configParser->parseBasePackageList();
	
	m_dependenciesList.clear();
	// Let's install them now
	std::set<string> listOfPackages = m_configParser->getListOfPackagesFromDirectory(pkgdir);
	for (std::set<string>::const_iterator i = listOfPackages.begin(); i != listOfPackages.end();i++) {
		if ( ! checkPackageNameExist(*i) ) {
			if ( m_configParser->checkBinaryExist(*i)) {
				message = "ADD FOR INSTALL: " + *i ;
				cout << message << endl;
				if ( m_config.logdir != "" ) {
					write( fdlog, message.c_str(), message.length());
					write( fdlog, "\n", 1 );
				}
				m_dependenciesList.push_back(*i);
			}
		} else {
				message = "WARNING: " + *i + " is ALLREADY installed";
				cout << message << endl;
				if ( m_config.logdir != "" ) {
					write( fdlog, message.c_str(), message.length()); 
					write( fdlog, "\n", 1 );
				}
		}
	}

	addPackagesList();

	if ( m_config.logdir != "" ) {
		time_t finishTime;
		time(&finishTime);
		timestamp = ctime(&finishTime);
		timestamp = commandName + "finish " + timestamp;
		write( fdlog, "\n", 1 );
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
		close ( fdlog );
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
