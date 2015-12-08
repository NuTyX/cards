// cards_install.cc
//
// Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
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
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	if ( m_argParser.isSet(CardsArgumentParser::OPT_FORCE)) {
		m_force=true;
	} else {
		m_force=false;
	}

	m_pkgrepo = new Pkgrepo("/etc/cards.conf");

	if  ( m_argParser.otherArguments().size() > 1 ) {
		m_archive=false;
		Pkgrepo::parseConfig("/etc/cards.conf", m_config);
		// Get the list of installed packages
		getListOfPackageNames(m_root);

		// Retrieving info about all the packages
		buildDatabaseWithDetailsInfos(false);

		for(std::vector<std::string>::const_iterator it = m_argParser.otherArguments().begin(); it != m_argParser.otherArguments().end();it++) {
			if ( m_argParser.isSet(CardsArgumentParser::OPT_UPDATE)) {
				updatePackage();
			} else {
				if  ( checkPackageNameExist(*it) ) {
					m_actualError = PACKAGE_ALLREADY_INSTALL;
					treatErrors (*it);
				}
			}
		}
		for(std::vector<std::string>::const_iterator it = m_argParser.otherArguments().begin(); it != m_argParser.otherArguments().end();it++) {
			if ( ( m_pkgrepo->getListOfPackagesFromCollection(*it).size() > 0 ) &&
				( ! m_pkgrepo->checkBinaryExist(*it)) ) {
				set<string> ListOfPackage = m_pkgrepo->getListOfPackagesFromCollection(*it);
				for(std::set<std::string>::iterator jt = ListOfPackage.begin();jt != ListOfPackage.end();jt++) {
					m_packageName = *jt;
					generateDependencies();
				}
			} else {
				m_packageName = *it ;
				generateDependencies();
			}
		}
		getLocalePackagesList();
		addPackagesList();
	} else if ( ( m_pkgrepo->getListOfPackagesFromCollection(m_argParser.otherArguments()[0]).size() > 0 ) &&
	( ! m_pkgrepo->checkBinaryExist( m_argParser.otherArguments()[0]  )) ) {
		Pkgrepo::parseConfig("/etc/cards.conf", m_config);
		// Get the list of installed packages
		getListOfPackageNames(m_root);

		// Retrieving info about all the packages
		buildDatabaseWithDetailsInfos(false);

		set<string> ListOfPackage = m_pkgrepo->getListOfPackagesFromCollection(m_argParser.otherArguments()[0]);
		for(std::set<std::string>::iterator it = ListOfPackage.begin();it != ListOfPackage.end();it++) {
			m_packageName = *it;
			generateDependencies();
		}
		getLocalePackagesList();
		addPackagesList();
#ifndef NDEBUG
		for (std::vector<string>::iterator j = m_dependenciesList.begin(); j != m_dependenciesList.end();j++) {
			cerr << *j << endl;
		}
#endif
	} else {
		//TODO get rid of thoses useless arguments
//		run(0, NULL);
	}
}
CardsInstall::CardsInstall(const CardsArgumentParser& argParser,
	const string& packageName)
	: m_argParser(argParser), m_packageName(packageName), m_root("/")
{
}
CardsInstall::CardsInstall (const CardsArgumentParser& argParser,
	const vector<string>& listOfPackages)
	:m_argParser(argParser), m_dependenciesList(listOfPackages), m_root("/")
{
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	for (std::vector<string>::const_iterator it = listOfPackages.begin(); it != listOfPackages.end();it++) {
		m_packageName = basename(const_cast<char*>(it->c_str()));
		createBinaries();
	}
}
void CardsInstall::run(int argc, char** argv)
{
	// Is it a file (an archive) or a packagename
	string::size_type pos = m_argParser.otherArguments()[0].find("cards.tar");

	if ( pos != std::string::npos) {
		m_archive=true;
		m_packageFileName = m_argParser.otherArguments()[0];
	} else if ( (checkFileExist(m_argParser.otherArguments()[0])) &&
			! (checkFileEmpty(m_argParser.otherArguments()[0])) &&
			(checkRegularFile(m_argParser.otherArguments()[0])) ) {
				m_archive=true;
				m_packageFileName = m_argParser.otherArguments()[0];
	} else {
		m_archive=false;
		// Its a packagename
		m_packageName = m_argParser.otherArguments()[0];
		Pkgrepo::parseConfig("/etc/cards.conf", m_config);
		// get following datas
		// 6467b1da053830c331e1a97458c4f385#1428614644#firefox#37.0.1#1#Standalone web browser from mozilla.org#http://www.mozilla.com/firefox/#n.a#pierre at nutyx dot org,tnut at nutyx dot org#.cards.tar.xz
	}
	if ( m_argParser.isSet(CardsArgumentParser::OPT_UPDATE)) {
		updatePackage();
	} else {
		install();
	}
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
void CardsInstall::getLocalePackagesList()
{
	// Add the locales if any defined
	if (m_config.locale.size() == 0) {
		return;
	}
	vector<string> tmpList;
	string packageName = "";
	for (std::vector<string>::iterator i = m_config.locale.begin();i != m_config.locale.end();++i) {
		for (std::vector<string>::iterator j = m_dependenciesList.begin(); j != m_dependenciesList.end();j++) {
			packageName = *j + "." + *i;
#ifndef NDEBUG
			cerr << packageName << endl;
#endif
			if ( m_pkgrepo->checkBinaryExist(packageName)) {
				m_packageFileName = m_pkgrepo->getPackageFileName(packageName);
				if ( ! checkFileExist(m_packageFileName)) {
					m_pkgrepo->downloadPackageFileName(packageName);
				}
				tmpList.push_back(packageName);
			}
		}
		if (tmpList.size() > 0 ) {
			for (std::vector<string>::iterator i = tmpList.begin();i != tmpList.end();++i) {
				m_dependenciesList.push_back(*i);
			}
		}
	}
#ifndef NDEBUG
	for (std::vector<string>::iterator j = m_dependenciesList.begin(); j != m_dependenciesList.end();j++) {
		cout << *j << endl;
	}
#endif
}
void CardsInstall::generateDependencies()
{
	vector< pair<string,time_t> > dependenciesWeMustAdd,
		depencenciestoSort;
	pair<string,time_t> PackageTime;
	PackageTime.first=m_packageName;
	PackageTime.second=0;
	// Insert the final package first
	dependenciesWeMustAdd.push_back(PackageTime);
	std::vector< pair<string,time_t> >::iterator vit;
	std::set< pair<string,time_t> >::iterator sit;
	while ( ! dependenciesWeMustAdd.empty() ) {	// Main WHILE
		vit = dependenciesWeMustAdd.begin();
		m_packageName = vit->first;
		PackageTime = *vit;
		dependenciesWeMustAdd.erase(vit); 	// Erase the current treated package name
		if ( m_listOfDepotPackages.find(m_packageName) != m_listOfDepotPackages.end() )
			continue; // We treat this one already
		if ( m_pkgrepo->checkBinaryExist(m_packageName)) // directs deps if not yet availables
			m_packageFileName = m_pkgrepo->getPackageFileName(m_packageName);
		if ( ! checkFileExist(m_packageFileName))	// Binary Archive not yet downloaded
			m_pkgrepo->downloadPackageFileName(m_packageName); // Get it
		set< pair<string,time_t> > directDependencies = getPackageDependencies(m_packageFileName);
		if ( ! checkPackageNameBuildDateSame(PackageTime)) // If not yet install or not up to dated
			depencenciestoSort.push_back(PackageTime); // Add it
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
			if ( PackageTime.first != sit->first )
				dependenciesWeMustAdd.push_back(*sit);
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
		if (!found)
			m_dependenciesList.push_back(vrit->first);
	}
}
void CardsInstall::updatePackage()
{
	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);
	if ( ! m_archive ) {
		if ( ! checkPackageNameExist(m_packageName) ) {
			m_actualError = PACKAGE_NOT_PREVIOUSLY_INSTALL;
			treatErrors(m_packageName);
		}
		if ( m_pkgrepo->checkBinaryExist(m_packageName)) {
			m_packageFileName = m_pkgrepo->getPackageFileName(m_packageName);
			if ( ! checkFileExist(m_packageFileName)) {
				m_pkgrepo->downloadPackageFileName(m_packageName);
			}
		} else {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
		}
		m_packageFileName = m_pkgrepo->getPackageFileName(m_packageName);
	}

	set<string> keep_list;

	// Reading the archiving to find a list of files
	pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);

	bool installed = checkPackageNameExist(package.first);

	if (!installed) {
		m_actualError = PACKAGE_NOT_PREVIOUSLY_INSTALL;
		treatErrors(package.first);
	}
	// Checking the rules
	readRulesFile();

	set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);

	set<string> conflicting_files = getConflictsFilesList(package.first, package.second);

	// Lock the database, any interruption forbidden
	Db_lock lock(m_root, true);

	if (! conflicting_files.empty()) {
		if (m_force) {
			set<string> keep_list;
			keep_list = getKeepFileList(conflicting_files, m_actionRules);
			removePackageFilesRefsFromDB(conflicting_files, keep_list); // Remove unwanted conflicts
		} else {
			copy(conflicting_files.begin(), conflicting_files.end(), ostream_iterator<string>(cerr, "\n"));
			m_actualError = LISTED_FILES_ALLREADY_INSTALLED;
			treatErrors("listed file(s) already installed, cannot continue... ");	
		}
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
	// Get the list of installed packages
	getListOfPackageNames(m_root);
	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);

	if (m_archive) {
	// no need more action just install the archive if it is OK
		addPackage();
	} else  {
		if  ( checkPackageNameExist(m_packageName) ) {
			m_actualError = PACKAGE_ALLREADY_INSTALL;
			treatErrors (m_packageName);
		}
#ifndef NDEBUG
		cerr << "Check for dependencies" << endl;
#endif
		// Generate the dependencies
		generateDependencies();
		// Add the locales if any defined
		getLocalePackagesList();
#ifndef NDEBUG
		for (std::vector<string>::iterator j = m_dependenciesList.begin(); j != m_dependenciesList.end();j++) {
			cout << *j << endl;
		}
#endif
		addPackagesList();
	}
}
void CardsInstall::addPackage()
{
	// New LockDatabase pointer
	Db_lock  * pLock = NULL;
	// Checking the rules
	readRulesFile();

	set<string> keep_list;
	
	// Reading the archive to find a list of files
	pair<string, pkginfo_t> package = openArchivePackage(m_packageFileName);
	set<string> non_install_files = applyInstallRules(package.first, package.second, m_actionRules);
#ifndef NDEBUG
	cerr << "Run extractAndRunPREfromPackage without upgrade" << endl;
#endif
	// Run pre-install if exist
	extractAndRunPREfromPackage(m_packageFileName);

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

	// Lock the database, any interruption forbidden
	pLock = new Db_lock(m_root, true);
	// Installation progressInfo of the files on the HD
	installArchivePackage(m_packageFileName, keep_list, non_install_files);

	// Add the metadata about the package to the DB
	moveMetaFilesPackage(package.first,package.second);

	// Add the info about the files to the DB
	addPackageFilesRefsToDB(package.first, package.second);
	delete pLock;

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
		removeFile(m_root,PKG_POST_INSTALL);
	}

	runLdConfig();
}
void CardsInstall::addPackagesList()
{
#ifndef NDEBUG
	cerr << "Number of Packages : " << m_dependenciesList.size()<< endl;
#endif
	for (std::vector<string>::iterator it = m_dependenciesList.begin(); it != m_dependenciesList.end();it++) {
		m_packageName = *it;
#ifndef NDEBUG
		cerr << m_packageName << endl;
#endif
		m_packageFileName = m_pkgrepo->getPackageFileName(m_packageName);
#ifndef NDEBUG
		cerr << m_packageFileName << endl;
#endif
		if  ( checkPackageNameExist(m_packageName) ) {
			updatePackage();
			continue;
		}

		if ( ! m_pkgrepo->checkBinaryExist(m_packageName)) {
			m_actualError = PACKAGE_NOT_FOUND;
			treatErrors(m_packageName);
		}
		addPackage();
	}
}
void CardsInstall::install(const vector<string>& dependenciesList)
{
	// Get the list of installed packages
	getListOfPackageNames(m_root);

	// Retrieving info about all the packages
	buildDatabaseWithDetailsInfos(false);
	string packageName;
	for (std::vector<string>::const_iterator it = dependenciesList.begin(); it != dependenciesList.end();it++) {
#ifndef NDEBUG
		cerr << *it << endl;
#endif
		packageName = basename(const_cast<char*>(it->c_str()));
		if ( packageName == m_argParser.otherArguments()[0])
			break;
		std::set<string> listOfPackages = findPackages(*it);
		for (std::set<string>::const_iterator i = listOfPackages.begin(); i != listOfPackages.end();i++) {
			if (i->find("cards.tar")== string::npos )
				continue;
#ifndef NDEBUG
			cerr << *i << endl;
#endif
			m_packageFileName = *it + "/" + *i;
			ArchiveUtils packageArchive(m_packageFileName.c_str());
			string name = packageArchive.name();
			if ( ! checkPackageNameExist(name )) {
#ifndef NDEBUG
				cerr << "Add for install: " << name << endl;
#endif
				addPackage();
			}
		}
	}
}
void CardsInstall::createBinaries(const string& packageName)
{
	m_packageName = packageName;
	createBinaries();
}
void CardsInstall::createBinaries()
{
	Pkgrepo::parseConfig("/etc/cards.conf", m_config);
	m_pkgrepo = new Pkgrepo("/etc/cards.conf");
	cout << "create of " << m_packageName << endl;
	string pkgdir = m_pkgrepo->getPortDir(m_packageName);
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

	std::set<string> listOfPackages = findPackages(pkgdir);
	// Let's install the found binaries now
	for (std::set<string>::const_iterator i = listOfPackages.begin(); i != listOfPackages.end();i++) {
		if (i->find("cards.tar")== string::npos )
			continue;
		m_packageFileName = pkgdir + "/" + *i;
		ArchiveUtils packageArchive(m_packageFileName.c_str());
		string name = packageArchive.name();
		string version = packageArchive.version();
		if ( ! checkPackageNameExist(name) ) {
				message = "ADD FOR INSTALL: " + name + " " + version;
				m_packageFileName = pkgdir + "/" + *i;
				addPackage();
		} else {
				message = "WARNING: " + name + " is ALLREADY installed";
		}
		cout << message << endl;
		if ( m_config.logdir != "" ) {
			write( fdlog, message.c_str(), message.length());
			write( fdlog, "\n", 1 );
		}
	}

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
