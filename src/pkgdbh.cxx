/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "pkgdbh.h"

using __gnu_cxx::stdio_filebuf;

pkgdbh::pkgdbh(const std::string& name)
	: m_utilName(name), m_DB_Empty(true), m_miniDB_Empty(true)
{
	openlog(m_utilName.c_str(),LOG_CONS,LOG_LOCAL7);
}
pkgdbh::pkgdbh()
	: m_utilName("pkgdbh"), m_DB_Empty(true), m_miniDB_Empty(true)
{
	m_root="/";
}
pkgdbh::~pkgdbh()
{
#ifdef DEBUG
		syslog(LOG_INFO,"Closing log...");
#endif
		runLastPostInstall();
		void closelog(void);
}
void pkgdbh::parseArguments(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		std::string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			m_root = argv[i + 1];
			i++;
		} else if (option[0] == '-' || !m_packageName.empty()) {
			m_actualError = cards::ERROR_ENUM_INVALID_OPTION;
			treatErrors(option);
		} else {
			m_packageName = option;
		}
	}
	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
}
void pkgdbh::run(int argc, char** argv)
{
	parseArguments(argc,argv);
	run();
}

void pkgdbh::treatErrors(const std::string& s) const
{
	using namespace cards;
	switch ( m_actualError )
	{
		case ERROR_ENUM_CANNOT_FIND_DEPOT:
			throw std::runtime_error(_("cannot find any depot, check your /etc/cards.conf file") + s);
			break;
		case ERROR_ENUM_CANNOT_CREATE_DIRECTORY:
		case ERROR_ENUM_CANNOT_GENERATE_LEVEL:
			break;
		case ERROR_ENUM_CANNOT_DOWNLOAD_FILE:
			throw std::runtime_error(_("could not download ") + s);
			break;
		case ERROR_ENUM_CANNOT_CREATE_FILE:
			throw RunTimeErrorWithErrno(_("could not created ") + s);
			break;
		case ERROR_ENUM_CANNOT_OPEN_FILE:
			throw RunTimeErrorWithErrno(_("could not open ") + s);
			break;
		case ERROR_ENUM_CANNOT_FIND_FILE:
			throw RunTimeErrorWithErrno(_("could not find ") + s);
			break;
		case ERROR_ENUM_CANNOT_READ_FILE:
			throw std::runtime_error(_("could not read ") + s);
			break;
		case ERROR_ENUM_CANNOT_PARSE_FILE:
			throw std::runtime_error(_("could not parse ") + s);
			break;
		case ERROR_ENUM_CANNOT_READ_DIRECTORY:
			throw RunTimeErrorWithErrno(_("could not read directory ") + s);
			break;
		case ERROR_ENUM_CANNOT_WRITE_FILE:
			throw RunTimeErrorWithErrno(_("could not write file ") + s);
			break;
		case ERROR_ENUM_CANNOT_SYNCHRONIZE:
			throw RunTimeErrorWithErrno(_("could not synchronize ") + s);
			break;
		case ERROR_ENUM_CANNOT_RENAME_FILE:
			throw RunTimeErrorWithErrno(_("could not rename ") + s);
			break;
		case ERROR_ENUM_CANNOT_COPY_FILE:
			throw RunTimeErrorWithErrno(_("could not copy ") + s);
			break;
		case ERROR_ENUM_CANNOT_DETERMINE_NAME_BUILDNR:
			throw RunTimeErrorWithErrno(_("could not determine name / build number ") + s);
			break;
		case ERROR_ENUM_WRONG_ARCHITECTURE:
			throw std::runtime_error(s + _(": wrong architecture") );
			break;
		case ERROR_ENUM_EMPTY_PACKAGE:
			throw RunTimeErrorWithErrno(_("empty package ") + s);
			break;
		case ERROR_ENUM_CANNOT_FORK:
			throw RunTimeErrorWithErrno(_("fork() failed ") + s);
			break;
		case ERROR_ENUM_WAIT_PID_FAILED:
			throw RunTimeErrorWithErrno(_("waitpid() failed ") + s);
			break;
		case ERROR_ENUM_DATABASE_LOCKED:
			throw RunTimeErrorWithErrno(_("Database  ") + s + _(" locked by another user"));
			break;
		case ERROR_ENUM_CANNOT_LOCK_DIRECTORY:
			throw RunTimeErrorWithErrno(_("could not lock directory ") + s);
			break;
		case ERROR_ENUM_CANNOT_REMOVE_FILE:
			throw RunTimeErrorWithErrno(_("could not remove file ") + s);
			break;
		case ERROR_ENUM_CANNOT_RENAME_DIRECTORY:
			throw RunTimeErrorWithErrno(_("could not rename directory ") + s);
			break;
		case ERROR_ENUM_OPTION_ONE_ARGUMENT:
			throw RunTimeErrorWithErrno(s + _(" require one argument"));
			break;
		case ERROR_ENUM_INVALID_OPTION:
			throw std::runtime_error(s + _(" invalid option") );
			break;
		case ERROR_ENUM_OPTION_MISSING:
			throw std::runtime_error(s + _(" option missing"));
			break;
		case ERROR_ENUM_TOO_MANY_OPTIONS:
			throw std::runtime_error(s+ _(": to many options"));
			break;
		case ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
			throw std::runtime_error(s + _(" only root can install / upgrade / remove packages"));
			break;
		case ERROR_ENUM_PACKAGE_ALLREADY_INSTALL:
			throw std::runtime_error(_("package ") + s + _(" already installed (use -u to upgrade)"));
			break;
		case ERROR_ENUM_PACKAGE_NOT_INSTALL:
			throw std::runtime_error(_("package ") + s + _(" not yet installed"));
			break;
		case ERROR_ENUM_PACKAGE_NOT_PREVIOUSLY_INSTALL:
			throw std::runtime_error(_("package ") + s + _(" not previously installed (skip -u to install)"));
			break;
		case ERROR_ENUM_LISTED_FILES_ALLREADY_INSTALLED:
			throw std::runtime_error(s + _(": listed file(s) already installed (use -f to ignore and overwrite)"));
			break;
		case ERROR_ENUM_PKGADD_CONFIG_LINE_TOO_LONG:
			throw RunTimeErrorWithErrno(s + _(": line too long, aborting"));
			break;
		case ERROR_ENUM_PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS:
			throw RunTimeErrorWithErrno(s + _(": wrong number of arguments, aborting"));
			break;
		case ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_ACTION:
			throw RunTimeErrorWithErrno(s + _("': config unknown action, should be YES or NO, aborting"));
			break;
		case ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_EVENT:
			throw RunTimeErrorWithErrno(s + _("' unknown event, aborting"));
			break;
		case ERROR_ENUM_CANNOT_COMPILE_REGULAR_EXPRESSION:
			throw std::runtime_error(_("error compiling regular expression '") + s + _("', aborting"));
			break;
		case ERROR_ENUM_NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE:
			throw std::runtime_error(s + _(" is neither an installed package nor a package file"));
			break;
		case ERROR_ENUM_PACKAGE_NOT_FOUND:
			throw std::runtime_error(_("The package ") + s + _(" is not found"));
			break;
		case ERROR_ENUM_PACKAGE_NOT_EXIST:
			throw std::runtime_error(_("The package ") + s + _(" does not exist"));
			break;
		case ERROR_ENUM_PACKAGE_IN_USE:
			throw std::runtime_error(_("The package ") + s + _(" is in use"));
			break;
	}
}
void pkgdbh::progressInfo(cards::ActionEnum action)
{
	m_actualAction = action;
	progressInfo();
}
void pkgdbh::progressInfo()
{
	using namespace cards;
  static int j=0;
  switch (m_actualAction)
  {
		case ACTION_ENUM_PKG_DOWNLOAD_START:
			break;
		case ACTION_ENUM_PKG_DOWNLOAD_RUN:
			break;
		case ACTION_ENUM_PKG_DOWNLOAD_END:
			break;
		case ACTION_ENUM_PKG_MOVE_META_START:
			break;
		case ACTION_ENUM_PKG_MOVE_META_END:
			break;
		case ACTION_ENUM_DB_OPEN_START:
			std::cout << _("Retrieve info about the ")
			<< m_listOfPackagesNames.size() << _(" packages: ");
			break;
		case ACTION_ENUM_DB_OPEN_RUN:
			j++;
			printf("%3u%%\b\b\b\b",
				( j * 100 ) / m_listOfPackagesNames.size() );
			break;
		case ACTION_ENUM_DB_OPEN_END:
			printf("100 %%\n");
			j=0;
			break;
		case ACTION_ENUM_PKG_PREINSTALL_START:
			std::cout << _("pre-install: start") << std::endl;
			break;
		case ACTION_ENUM_PKG_PREINSTALL_END:
			std::cout << _("pre-install: finish") << std::endl;
			break;
		case ACTION_ENUM_PKG_INSTALL_START:
			j=0;
			std::cout << _("   ADD: (")
				<< m_packageArchiveCollection
				<< ") "
				<< m_packageName
				<< " "
				<< m_packageArchiveVersion
				<< "-"
				<< m_packageArchiveRelease
				<< ", "
				<< m_filesNumber << _(" files: ");
			break;
		case ACTION_ENUM_PKG_INSTALL_RUN:
			j++;
			printf("%3u%%\b\b\b\b",
				(m_installedFilesNumber * 100 ) /  m_filesNumber );
			break;
		case ACTION_ENUM_PKG_INSTALL_END:
			printf("100 %%\n");
			break;
		case ACTION_ENUM_PKG_POSTINSTALL_START:
			std::cout << _("post-install: start") << std::endl;
			break;
		case ACTION_ENUM_PKG_POSTINSTALL_END:
			std::cout << _("post-install: finish") << std::endl;
			break;
		case ACTION_ENUM_DB_ADD_PKG_START:
			break;
		case ACTION_ENUM_DB_ADD_PKG_END:
			break;
		case ACTION_ENUM_RM_PKG_FILES_START:
			j=0;
			std::cout << _("REMOVE: (")
				<< m_packageCollection
				<< ") "
				<< m_packageName
				<< " "
				<< m_packageVersion
				<< "-"
				<< m_packageRelease
				<< ", "
				<< m_filesList.size()
				<< _(" files: ");
			break;
		case ACTION_ENUM_RM_PKG_FILES_RUN:
			j++;
			printf("%3u%%\b\b\b\b",
				( j * 100 ) / m_filesList.size() );
			break;
		case ACTION_ENUM_RM_PKG_FILES_END:
			printf("100 %%\n");
			break;
		case ACTION_ENUM_LDCONFIG_START:
			break;
		case ACTION_ENUM_LDCONFIG_END:
			break;
  }
}

std::set<std::string>& pkgdbh::getListOfPackagesNames()
{
	if (m_listOfPackagesNames.empty())
		getListOfPackagesNames(m_root);
	return m_listOfPackagesNames;
}
std::string pkgdbh::getSingleItem(const std::string& PackageName, const char i) const
{
	const std::string metaFile = m_root + PKG_DB_DIR + PackageName + PKG_META;
	std::set<std::string> fileContent;
	parseFile(fileContent,metaFile.c_str());
	std::string item;
	for ( auto a : fileContent) {
		if ( a[0] == i ) {
			item = a.substr(1);
			break;
		}
	}
	return item;
}
std::set<std::string> pkgdbh::getSetOfItems(const std::string& PackageName, const char i) const
{
	const std::string metaFile = m_root + PKG_DB_DIR + PackageName + PKG_META;
	std::set<std::string> fileContent;
	parseFile(fileContent,metaFile.c_str());
	std::set<std::string> setOfItems;
	for ( auto a : fileContent) {
		if ( a[0] == i ) {
			setOfItems.insert(a.substr(1));
		}
	}
	return setOfItems;
}
std::string pkgdbh::getDescription(const std::string& name) const
{
	return getSingleItem(name,'D');
}
std::string pkgdbh::getVersion(const std::string& name) const
{
	return getSingleItem(name,'V');
}
std::string pkgdbh::getCollection(const std::string& name) const
{
	return getSingleItem(name,'c');
}
std::string pkgdbh::getSets(const std::string& name) const
{
	std::string s;
	for (auto i: getSetOfItems(name,'s'))
		s = s + " " + i;
	return s;
}
std::set<std::string> pkgdbh::getSetOfSets(const std::string& name) const
{
	return getSetOfItems(name,'s');
}
std::set<std::string> pkgdbh::getSetOfCategories (const std::string& name) const
{
	return getSetOfItems(name, 'T');
}
std::string pkgdbh::getCategories(const std::string& name) const
{
	std::string s;
	for (auto i: getSetOfItems(name,'T'))
		s = s + " " + i;
	return s;
}
std::string pkgdbh::getArch(const std::string& name) const
{
	return getSingleItem(name,'A');
}
std::string pkgdbh::getLicense(const std::string& name) const
{
	return getSingleItem(name,'L');
}
unsigned int pkgdbh::getSize(const std::string& name) const
{
	std::string r = getSingleItem(name,'S');
	return atoi(r.c_str());
}
unsigned int pkgdbh::getRelease(const std::string& name) const
{
	std::string r = getSingleItem(name,'r');
	return atoi(r.c_str());
}

// Append to the "DB" the number of packages founds
// (directory containing a file named files
int pkgdbh::getListOfPackagesNames (const std::string& path)
{
	if (! m_listOfPackagesNames.empty())
		return m_listOfPackagesNames.size();

	const std::string pathdb =  m_root + PKG_DB_DIR;
#ifdef DEBUG
	std::cerr << "pathdb: " << pathdb << std::endl;
#endif
	if ( findDir(m_listOfPackagesNames, pathdb) != 0 ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
		treatErrors(pathdb);
	}
#ifdef DEBUG
	std::cerr << "Number of Packages: "
		<< m_listOfPackagesNames.size()
		<< std::endl;
#endif
	return m_listOfPackagesNames.size();
}

// get details info of a package
std::pair<std::string, cards::db>
pkgdbh::getInfosPackage( const std::string& packageName )
{
	std::pair<std::string, cards::db> result;

	result.first = packageName;
	return result;
}

std::set<std::string>
pkgdbh::getSetOfFiles( const std::string& packageName )
{
	std::string name = m_listOfAlias[packageName];
	std::set<std::string> packageFiles;

	const std::string fileslist = m_root
		+ PKG_DB_DIR
		+ name
		+ PKG_FILES;
	int fd = open(fileslist.c_str(), O_RDONLY);
	if (fd == -1) {
		m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
		treatErrors(fileslist);
	}
	stdio_filebuf<char> listbuf(fd, std::ios::in, getpagesize());
	std::istream in(&listbuf);

	if (!in)
		throw RunTimeErrorWithErrno("could not read " + fileslist);

	while (!in.eof()){
	// read alls the files for alls the packages founds
	for (;;) {
			std::string file;
			getline(in, file);
			if (file.empty())
				break; // End of record
			packageFiles.insert(file);
		}
	}
	return packageFiles;
}

/**
 * Populate the database in following modes:
 * - if nothing specify only get the List of PackageNames
 *   and populate the alias list.
 * - if simple then only with name, version, release, collection
 *   build date, sets list and group name
 * - if all then all the availables attributes
 * - if files then all the files of the package(s)
 * - if packageName size() > 0 then we do just for the packageName
 *
 */
void
pkgdbh::buildDatabase(const bool& progress,
		const bool& simple,
		const bool& all,
		const bool& files,
		const std::string& packageName)
{
	using namespace cards;

          // This part is done in every cases
	cleanupMetaFiles(m_root);
	if (progress)
		progressInfo(ACTION_ENUM_DB_OPEN_START);

	if (m_listOfPackagesNames.empty() )
		getListOfPackagesNames (m_root);

	for ( auto name : m_listOfPackagesNames) {
		if (progress) {
			progressInfo(ACTION_ENUM_DB_OPEN_RUN);
		}
		const std::string metaFile = m_root
		+ PKG_DB_DIR
		+ name
		+ PKG_META;
		std::set<std::string> fileContent;
		parseFile(fileContent,metaFile.c_str());
		m_listOfAlias[name] = name;
		for ( auto s : fileContent) {
			if (s[0] != ALIAS)
				break;
			m_listOfAlias[s.substr(1)] = name;
		}
	}

	if ( !simple && !all && !files && (packageName.size() == 0) )
		return;

	if (packageName.size() > 0) {
		std::string name = m_listOfAlias[packageName];
		cards::db info;
		info.files=getSetOfFiles(name);
		m_listOfPackages[name] = info;
	}
	if ( !simple && !all && !files ) {
		if (progress) {
			progressInfo(ACTION_ENUM_DB_OPEN_END);
		}
		return;
	}

	std::set<std::string> sets;
	if (simple) {
		cards::db info;
		for ( auto pkgName : m_listOfPackagesNames) {
			if (progress) {
				progressInfo(ACTION_ENUM_DB_OPEN_RUN);
			}
			const std::string metaFile = m_root
			+ PKG_DB_DIR
			+ pkgName
			+ PKG_META;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
			m_listOfAlias[pkgName] = pkgName;
			unsigned short flags = 0;
			info.release(1);
			for (auto s : fileContent) {
				if (s[0] == BUILD) {
					info.build(strtoul(s.substr(1).c_str(),nullptr,0));
					flags++;
				}
				if (s[0] == VERSION){
					info.version(s.substr(1));
					flags=flags + 2;
				}
				if (s[0] == COLLECTION) {
					info.collection(s.substr(1));
					flags = flags + 4;
				}
				// As a group is not always present we cannot
				// depend on a found one to break
				if (s[0] == GROUP)
					info.group(s.substr(1));

				// As a std::set is not always present we cannot
				// depen on a found one to break
				if (s[0] == SETS) {
					sets.insert(s.substr(1));
				}
				if (s[0] == RELEASE) {
					info.release(atoi(s.substr(1).c_str()));
					flags = flags + 8;
				}
				if (flags == 15) {
					info.sets(sets);
					m_listOfPackages[pkgName] = info;
					break;
				}
			}
		}
	}
	if (progress)
            progressInfo(ACTION_ENUM_DB_OPEN_END);
}

/**
 * Populate m_listOfPackagesWithDeps with:
 * - Name of the package
 * - Dependencies list of the package
 * */
void
pkgdbh::buildSimpleDependenciesDatabase()
{
	if (m_listOfPackagesNames.empty() )
			getListOfPackagesNames (m_root);

	for (auto i : m_listOfPackagesNames) {
		std::pair < std::string, std::set<std::string> > packageWithDeps;
		packageWithDeps.first=i;
		const std::string metaFile = m_root + PKG_DB_DIR + i + PKG_META;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
		for (auto s : fileContent) {
			if (s[0] == RUNTIME_DEPENDENCY) {
				std::string dependency = s.substr(1,s.size()-11);
				packageWithDeps.second.insert(dependency);
			}
		}
		m_listOfPackagesWithDeps.insert(packageWithDeps);
	}
}

/**
 * Populate the database with:
 * - Name
 * - version
 * - release
 * - sets
 * - collection
 * - Build date
 * - Group name
 * - Packager name
 * */
void
pkgdbh::buildSimpleDatabase()
{
	if (m_miniDB_Empty) {
		if (m_listOfPackagesNames.empty() )
			getListOfPackagesNames (m_root);

		for ( auto i : m_listOfPackagesNames) {
			cards::db info;
			const std::string metaFile = m_root + PKG_DB_DIR + i + PKG_META;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
			info.release(1);
			info.dependency(false);
			std::set<std::string> sets;
			std::set<std::string> alias;
			m_listOfAlias[i] = i;
			for (auto s : fileContent) {
				if (s[0] == COLLECTION)
					info.collection(s.substr(1));

				if (s[0] == SETS)
					sets.insert(s.substr(1));

				if (s[0] == VERSION)
					info.version(s.substr(1));

				if (s[0] == RELEASE)
					info.release(atoi(s.substr(1).c_str()));

				if (s[0] == BUILD)
					info.build(strtoul(s.substr(1).c_str(),nullptr,0));

				if (s[0] == GROUP)
					info.group(s.substr(1));

				if (s[0] == ALIAS)
					alias.insert(s.substr(1));

				if (s[0] == PACKAGER)
					info.packager(s.substr(1));

				if ( s[0] == 'd' ) {
					if ( s == "d1" )
						info.dependency(true);
				}
			}
			info.sets(sets);
			info.alias(alias);
			m_listOfPackages[i] = info;
		}
		m_miniDB_Empty=false;
	}
#ifdef DEBUG
	for (auto i : m_listOfAlias)
		std::cerr << "Alias: "
			<< i.first
			<< ", Package: "
			<< i.second
			<< std::endl;
#endif
}
 /**
 * Populate the database with all details infos
 */
void pkgdbh::buildCompleteDatabase(const bool& progress)
{
	using namespace cards;
	cleanupMetaFiles(m_root);
	if (m_DB_Empty) {
		if (m_listOfPackagesNames.empty())
			getListOfPackagesNames (m_root);

		if (progress)
			progressInfo(ACTION_ENUM_DB_OPEN_START);

		for (auto i : m_listOfPackagesNames) {
			if (progress)
				progressInfo(ACTION_ENUM_DB_OPEN_RUN);

			cards::db info;
			const std::string metaFileDir = m_root + PKG_DB_DIR + i;
			const std::string metaFile = metaFileDir + PKG_META;
			info.install( getEpochModifyTimeFile(metaFileDir) );
			std::set<std::pair<std::string,time_t>> dependencies;
			std::set<std::string> fileContent;
			std::set<std::string> categories;
			std::set<std::string> sets;
			std::set<std::string> alias;
			parseFile(fileContent,metaFile.c_str());
			info.release(1);
			info.dependency(false);
			m_listOfAlias[i] = i;
			for (auto s : fileContent) {
				if (s[0] == CONTRIBUTORS)
					info.contributors(s.substr(1));

				if (s[0] == DESCRIPTION)
					info.description(s.substr(1)) ;

				if (s[0] == BUILD)
					info.build(strtoul(s.substr(1).c_str(),nullptr,0));

				if (s[0] == URL)
					info.url(s.substr(1));

				if (s[0] == LICENSE)
					info.license(s.substr(1));

				if (s[0] == MAINTAINER)
					info.maintainer(s.substr(1));

				if (s[0] == PACKAGER)
					info.packager( s.substr(1));

				if (s[0] == VERSION)
					info.version( s.substr(1));

				if ( s[0] == RELEASE) {
					info.release(atoi(s.substr(1).c_str()));
				}
				if (s[0] == ARCHITECTURE)
					info.arch(s.substr(1));

				if (s[0] == COLLECTION)
					info.collection(s.substr(1));

				if ( s[0] == SETS ) {
					sets.insert(s.substr(1));
				}
				if ( s[0] == GROUP ) {
					info.group( s.substr(1) );
				}
				if ( s[0] == 'd' ) {
					if ( s == "d1" )
						info.dependency(true);
				}
				if (s[0] == SPACE) {
					info.space( atoi(s.substr(1).c_str()) );
				}
				if (s[0] == ALIAS) {
					alias.insert(s.substr(1));
					m_listOfAlias[s.substr(1)] = i;
				}
				if (s[0] == CATEGORIES)
					categories.insert(s.substr(1));

				if (s[0] == RUNTIME_DEPENDENCY) {
					std::pair<std::string,time_t > NameEpoch;
					NameEpoch.first=s.substr(1,s.size()-11);
					NameEpoch.second=strtoul((s.substr(s.size()-10)).c_str(),nullptr,0);
					dependencies.insert(NameEpoch);
				}
			}
			info.alias(alias);
			info.sets(sets);
			info.categories(categories);
			info.dependencies(dependencies);
			// list of files
			const std::string filelist = m_root + PKG_DB_DIR + i + PKG_FILES;
			int fd = open(filelist.c_str(), O_RDONLY);
			if (fd == -1) {
				m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
				treatErrors(filelist);
			}
			stdio_filebuf<char> listbuf(fd, std::ios::in, getpagesize());
			std::istream in(&listbuf);
			if (!in)
				throw RunTimeErrorWithErrno("could not read " + filelist);

			while (!in.eof()){
				// read alls the files for alls the packages founds
				for (;;) {
					std::string file;
					getline(in, file);
					if (file.empty())
						break; // End of record
					info.files.insert(info.files.end(), file);
				}
				if (!info.files.empty()) {
					m_listOfPackages[i] = info;
				}
			}
		}
		if (progress)
			progressInfo(ACTION_ENUM_DB_OPEN_END);

		m_DB_Empty=false;
	}
}
void pkgdbh::moveMetaFilesPackage(const std::string& name, cards::db& info)
{
	std::set<std::string> metaFilesList;
	progressInfo(cards::ACTION_ENUM_PKG_MOVE_META_START);
	const std::string packagedir = m_root + PKG_DB_DIR ;
	const std::string packagenamedir = m_root + PKG_DB_DIR + name ;

	for (auto i: info.files)
	{
		if (i[0] == '.')
			metaFilesList.insert(metaFilesList.end(), i );

	}
	for ( auto i : metaFilesList) info.files.erase(i);
	removeFile ( m_root, "/.MTREE");
	metaFilesList.insert(METAFILE);
	std::set<std::string> fileContent;
	if ( parseFile(fileContent,METAFILE) == -1 ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_FIND_FILE;
		treatErrors(METAFILE);
	}

	mkdir(packagenamedir.c_str(),0755);

	for (auto i : metaFilesList)
	{
		char * destFile = const_cast<char*>(i.c_str());
		destFile++;
		std::string file = packagenamedir + "/" + destFile;
		if (rename(i.c_str(), file.c_str()) == -1) {
			m_actualError = cards::ERROR_ENUM_CANNOT_RENAME_FILE;
			treatErrors( i + " to " + file);
		}
		if ( i == PKG_POST_INSTALL ) {
			if (copyFile(i.c_str(), file.c_str()) == -1) {
				m_actualError = cards::ERROR_ENUM_CANNOT_COPY_FILE;
				treatErrors( file  + " to " + i);
			}
		}
	}
	if (m_dependency) {
		std::string file = packagenamedir + PKG_META;
		fileContent.insert("d1");
		std::ofstream out(file);
		for ( auto i: fileContent) out << i << std::endl;
		out.close();
		m_dependency=false;
	}
	progressInfo(cards::ACTION_ENUM_PKG_MOVE_META_END);
}
void pkgdbh::addPackageFilesRefsToDB(const std::string& name, const cards::db& info)
{

	m_listOfPackages[name] = info;
	const std::string packagedir = m_root + PKG_DB_DIR ;
	const std::string packagenamedir = m_root + PKG_DB_DIR + name ;
	mkdir(packagenamedir.c_str(),0755);
	const std::string fileslist = packagenamedir + PKG_FILES;
	const std::string fileslist_new = fileslist + ".imcomplete_transaction";
	int fd_new = creat(fileslist_new.c_str(),0644);
	if (fd_new == -1)
	{
		m_actualError = cards::ERROR_ENUM_CANNOT_CREATE_FILE;
		treatErrors(fileslist_new);
	}

	stdio_filebuf<char> filebuf_new(fd_new, std::ios::out, getpagesize());

	std::ostream db_new(&filebuf_new);
	copy(m_listOfPackages[name].files.begin(),
		m_listOfPackages[name].files.end(),
		std::ostream_iterator<std::string>(db_new, "\n"));

	db_new.flush();
	if (!db_new)
	{
		rmdir(packagenamedir.c_str());
		m_actualError = cards::ERROR_ENUM_CANNOT_WRITE_FILE;
		treatErrors(fileslist_new);
	}
	// Synchronize file to disk
	if (fsync(fd_new) == -1)
	{
		rmdir(packagenamedir.c_str());
		m_actualError = cards::ERROR_ENUM_CANNOT_SYNCHRONIZE;
		treatErrors(fileslist_new);
	}
	// Move new database into place
	if (rename(fileslist_new.c_str(), fileslist.c_str()) == -1)
	{
		rmdir(packagenamedir.c_str());
		m_actualError = cards::ERROR_ENUM_CANNOT_RENAME_FILE;
		treatErrors(fileslist_new + " to " + fileslist);
	}
}
bool pkgdbh::checkPackageNameExist(const std::string& name) const
{
	return (m_listOfAlias.find(name) != m_listOfAlias.end());
}
bool pkgdbh::checkDependency(const std::string& name)
{
	if  ( checkPackageNameExist(name) )
		return m_listOfPackages[name].dependency();
	return false;
}
void pkgdbh::setDependency()
{
	m_dependency=true;
}
void pkgdbh::resetDependency()
{
	m_dependency=false;
}
bool
pkgdbh::checkPackageNameUptodate(archive& archiveName)
{
	std::set<std::string>::iterator it = m_listOfPackagesNames.find(archiveName.name());
	if (it == m_listOfPackagesNames.end())
		return false;
	if (m_listOfPackages[archiveName.name()].version() !=  archiveName.version())
		return false;
	if (m_listOfPackages[archiveName.name()].release() !=  archiveName.release())
		return false;
	if (m_listOfPackages[archiveName.name()].build() < archiveName.build())
		return false;
	if (m_listOfPackages[archiveName.name()].collection() == "")
		return true;
	if (archiveName.collection() == "" )
		return true;
	if (m_listOfPackages[archiveName.name()].collection() != archiveName.collection())
		return false;
	return true;
}
bool
pkgdbh::checkPackageNameUptodate(std::pair<std::string, cards::db>& archiveName)
{
	std::set<std::string>::iterator it = m_listOfPackagesNames.find(archiveName.first);
	if (it == m_listOfPackagesNames.end())
		return false;
	if (m_listOfPackages[archiveName.first].version() !=  archiveName.second.version())
		return false;
	if (m_listOfPackages[archiveName.first].release() !=  archiveName.second.release())
		return false;
	if (m_listOfPackages[archiveName.first].build() < archiveName.second.build())
		return false;
	if (m_listOfPackages[archiveName.first].collection() == "")
		return true;
	if (archiveName.second.collection() == "" )
		return true;
	if (m_listOfPackages[archiveName.first].collection() != archiveName.second.collection())
		return false;
	return true;
}

bool pkgdbh::checkPackageNameBuildDateSame(const std::pair<std::string,time_t>& dependencieNameBuild)
{
	if (dependencieNameBuild.second == 0)
		return false;
	std::set<std::string>::iterator it = m_listOfPackagesNames.find(dependencieNameBuild.first);
	if (it == m_listOfPackagesNames.end())
		return false;
	if (m_listOfPackages[dependencieNameBuild.first].build() < dependencieNameBuild.second)
		return false;
	return true;
}
/* Remove meta data about the removed package */
void pkgdbh::removePackageFilesRefsFromDB(const std::string& name)
{
	if ( checkPackageNameExist(name)){
		m_packageVersion = m_listOfPackages[name].version();
		m_packageRelease = itos(m_listOfPackages[name].release());
		m_packageCollection = m_listOfPackages[name].collection();
	}
	std::set<std::string> metaFilesList;
	const std::string packagedir = m_root + PKG_DB_DIR ;
	const std::string arch = m_listOfPackages[name].arch();
	const std::string packagenamedir = m_root + PKG_DB_DIR + name;

	if ( findDir(metaFilesList, packagenamedir) != 0 ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
		treatErrors(packagenamedir);
	}
	if (metaFilesList.size() > 0) {
		for (std::set<std::string>::iterator i = metaFilesList.begin(); i != metaFilesList.end();++i) {
			const std::string filename = packagenamedir + "/" + *i;
			if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
				const char* msg = strerror(errno);
					std::cerr << m_utilName << ": could not remove " << filename << ": " << msg << std::endl;
				}
#ifdef DEBUG
				std::cerr  << "File: " << filename << " is removed"<< std::endl;
#endif
			}
	}
	if( remove(packagenamedir.c_str()) == -1) {
		const char* msg = strerror(errno);
		std::cerr << m_utilName << ": could not remove " << packagenamedir << ": " << msg << std::endl;
	}
#ifdef DEBUG
	std::cerr  << "Directory: " << packagenamedir << " is removed"<< std::endl;
#endif
}

/* Remove the physical files after followings some rules */
void pkgdbh::removePackageFiles(const std::string& name)
{
	m_filesList = m_listOfPackages[name].files;
	m_listOfPackages.erase(name);
	m_packageName =  name ;

#ifdef DEBUG
	std::cerr << "Removing package phase 1 (all files in package):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files that still have references
	for (auto i : m_listOfPackages)
		for (auto j : i.second.files)
			m_filesList.erase(j);

#ifdef DEBUG
	std::cerr << "Removing package phase 2 (files that still have references excluded):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	progressInfo(cards::ACTION_ENUM_RM_PKG_FILES_START);
	// Delete the files from bottom to up to make sure we delete the contents of any folder before
	for (std::set<std::string>::const_reverse_iterator i = m_filesList.rbegin(); i != m_filesList.rend(); ++i) {
		progressInfo(cards::ACTION_ENUM_RM_PKG_FILES_RUN);
		const std::string filename = m_root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			const char* msg = strerror(errno);
			std::cerr << m_utilName << ": could not remove " << filename << ": " << msg << std::endl;
		}
	}
	progressInfo(cards::ACTION_ENUM_RM_PKG_FILES_END);
}

void pkgdbh::removePackageFiles(const std::string& name, const std::set<std::string>& keep_list)
{
	m_filesList = m_listOfPackages[name].files;
	m_listOfPackages.erase(name);
	m_packageName =  name ;
#ifdef DEBUG
	std::cerr << "Removing package phase 1 (all files in package):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files found in the keep list
	for (auto i : keep_list) m_filesList.erase(i);

#ifdef DEBUG
	std::cerr << "Removing package phase 2 (files that is in the keep list excluded):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files that still have references
	for (auto i: m_listOfPackages)
		for (auto j: i.second.files)
			m_filesList.erase(j);

#ifdef DEBUG
	std::cerr << "Removing package phase 3 (files that still have references excluded):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Delete the files
	progressInfo(cards::ACTION_ENUM_RM_PKG_FILES_START);
	for (std::set<std::string>::const_reverse_iterator i = m_filesList.rbegin(); i != m_filesList.rend(); ++i) {
		progressInfo(cards::ACTION_ENUM_RM_PKG_FILES_RUN);
		const std::string filename = m_root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			std::cerr << m_utilName << ": could not remove " << filename << ": " << msg << std::endl;
		}
	}
	progressInfo(cards::ACTION_ENUM_RM_PKG_FILES_END);
}

void pkgdbh::removePackageFilesRefsFromDB(std::set<std::string> files, const std::set<std::string>& keep_list)
{
	if ( checkPackageNameExist(m_packageName)){
		m_packageVersion = m_listOfPackages[m_packageName].version();
		m_packageRelease = itos(m_listOfPackages[m_packageName].release());
		m_packageCollection = m_listOfPackages[m_packageName].collection();
	}
	// Remove all references
	for (auto i : m_listOfPackages) {
		for ( auto j : files ) {
			size_t s = i.second.files.size();
			i.second.files.erase(j);
			// If number of references have change, we refresh the disk copy
			if ( s != i.second.files.size())
				addPackageFilesRefsToDB(i.first,i.second);
		}
	}
#ifdef DEBUG
	std::cerr << "Removing files:" << std::endl;
	copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files found in the keep list
	for (std::set<std::string>::const_iterator i = keep_list.begin(); i != keep_list.end(); ++i)
		files.erase(*i);

	// Delete the files
	for (std::set<std::string>::const_reverse_iterator i = files.rbegin(); i != files.rend(); ++i) {
		const std::string filename = m_root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			std::cerr << m_utilName << ": could not remove " << filename << ": " << msg << std::endl;
		}
	}
}

std::set<std::string>
pkgdbh::getConflictsFilesList (const std::string& name, cards::db& info)
{
	std::set<std::string> files;

	// Find conflicting files in database
	for (auto i : m_listOfPackages) {
		if (i.first != name) {
			std::set_intersection(info.files.begin(), info.files.end(),
					 i.second.files.begin(), i.second.files.end(),
					 inserter(files, files.end()));
		}
	}

#ifdef DEBUG
	std::cerr << "Conflicts phase 1 (conflicts in database):" << std::endl;
	copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Find conflicting files in filesystem
	for (auto i : info.files) {
		const std::string filename = m_root + i;
		if (checkFileExist(filename) && files.find(i) == files.end())
			files.insert(files.end(), i);
	}

#ifdef DEBUG
	std::cerr << "Conflicts phase 2 (conflicts in filesystem added):" << std::endl;
	copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Exclude directories
	std::set<std::string> tmp = files;
	for (auto i : tmp) {
		if (i[i.length() - 1] == '/')
			files.erase(i);
	}

#ifdef DEBUG
	std::cerr << "Conflicts phase 3 (directories excluded):" << std::endl;
	copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// If this is an upgrade, remove files already owned by this package
	if (m_listOfPackages.find(name) != m_listOfPackages.end()) {
		for (auto i : m_listOfPackages[name].files)
			files.erase(i);

#ifdef DEBUG
		std::cerr << "Conflicts phase 4 (files already owned by this package excluded):" << std::endl;
		copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
		std::cerr << std::endl;
#endif
	}

	return files;
}
std::pair<std::string, cards::db>
pkgdbh::openArchivePackage(const std::string& filename)
{
	std::pair<std::string, cards::db> result;
	archive packageArchive(filename);

	std::string basename(filename, filename.rfind('/') + 1);
	m_filesNumber = packageArchive.size();
	if (m_filesNumber == 0 ) {
		m_actualError = cards::ERROR_ENUM_EMPTY_PACKAGE;
		treatErrors(basename);
	}

	if ( ( packageArchive.arch() != getMachineType() ) && ( packageArchive.arch() != "any" ) ) {
		if ( m_root.size() > 1 ) { // means it's in a chroot environment ... probably
			std::cerr << basename << ": Architecture is different. If "
				<<  m_root << " filesystem is a "
				<< getMachineType()
				<< " architecture, everything is OK..."
				<< std::endl;
		} else {
			m_actualError = cards::ERROR_ENUM_WRONG_ARCHITECTURE;
			treatErrors(basename);
		}
	}
	m_packageArchiveCollection = packageArchive.collection();
	m_packageArchiveVersion = packageArchive.version();
	m_packageArchiveRelease = itos(packageArchive.release());
	if (packageArchive.name().empty() ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_DETERMINE_NAME_BUILDNR;
		treatErrors(basename);
	}
	result.first = packageArchive.name();
	result.second.description(packageArchive.description());
	result.second.url(packageArchive.url());
	result.second.contributors(packageArchive.contributors());
	result.second.maintainer(packageArchive.maintainer());
	result.second.packager(packageArchive.packager());
	result.second.version(packageArchive.version());
	result.second.release(packageArchive.release());
	result.second.arch(packageArchive.arch());
	result.second.build(packageArchive.build());
	result.second.group(packageArchive.group());
	result.second.collection(packageArchive.collection());
	result.second.license(packageArchive.license());

	std::set<std::string> fileList =  packageArchive.setofFiles();
	for (auto i :fileList) {
		result.second.files.insert(i);
	}
	result.second.dependencies(packageArchive.listofDependenciesBuildDate());
	result.second.categories(packageArchive.listofCategories());
	result.second.sets(packageArchive.listofSets());
	result.second.alias(packageArchive.listofAlias());
	m_packageName = packageArchive.name();
	return result;
}
void pkgdbh::extractAndRunPREfromPackage(const std::string& filename)
{
	char buf[PATH_MAX];
	struct archive* archive;
	struct archive_entry* entry;
	archive = archive_read_new();
	INIT_ARCHIVE(archive);
	if (archive_read_open_filename(archive,
		filename.c_str(),
		DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
	{
		m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
		treatErrors(filename);
	}

	getcwd(buf, sizeof(buf));
	chdir(m_root.c_str());

	for (m_installedFilesNumber = 0; archive_read_next_header(archive, &entry) ==
		ARCHIVE_OK; ++m_installedFilesNumber)
	{
		const char *archive_filename = archive_entry_pathname(entry);
		if ( strcmp(archive_filename,PKG_PRE_INSTALL) == 0)
		{
			unsigned int flags = ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK;
			if (archive_read_extract(archive, entry, flags) != ARCHIVE_OK)
			{
				const char* msg = archive_error_string(archive);
				std::cerr << m_utilName
					<< _(": could not install ")
					<< archive_filename
					<< " : " << msg << std::endl;
				exit(EXIT_FAILURE);
			}
			break;

		}

	}
	FREE_ARCHIVE(archive);
	if (checkFileExist(PKG_PRE_INSTALL))
	{
		progressInfo(cards::ACTION_ENUM_PKG_PREINSTALL_START);
		process preinstall(SHELL,PKG_PRE_INSTALL, 0 );
		if (preinstall.executeShell()) {
			exit(EXIT_FAILURE);
		}
		removeFile(m_root,PKG_PRE_INSTALL);
		progressInfo(cards::ACTION_ENUM_PKG_PREINSTALL_END);
	}
	chdir(buf);
}
void pkgdbh::installArchivePackage
	(const std::string& filename, const std::set<std::string>& keep_list,
	const std::set<std::string>& non_install_list)
{
	struct archive* archive;
	struct archive_entry* entry;
	char buf[PATH_MAX];
	std::string absm_root;

	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
		{
			m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
			treatErrors(filename);
		}
	chdir(m_root.c_str());
	absm_root = getcwd(buf, sizeof(buf));
#ifdef DEBUG
	std::cout << "absm_root: " <<  absm_root  << " and m_root: " << m_root<< std::endl;
#endif
	progressInfo(cards::ACTION_ENUM_PKG_INSTALL_START);
	for (m_installedFilesNumber = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++m_installedFilesNumber) {

		progressInfo(cards::ACTION_ENUM_PKG_INSTALL_RUN);
		std::string archive_filename = archive_entry_pathname(entry);
		std::string reject_dir = trimFileName(absm_root + std::string("/") + std::string(PKG_REJECTED));
		std::string original_filename = trimFileName(absm_root + std::string("/") + archive_filename);
		std::string real_filename = original_filename;

		// Check if file is filtered out via INSTALL
		if (non_install_list.find(archive_filename) != non_install_list.end()) {
			mode_t mode;

			std::cout << m_utilName << ": ignoring " << archive_filename << std::endl;

			mode = archive_entry_mode(entry);

			if (S_ISREG(mode))
				archive_read_data_skip(archive);

			continue;
		}

		// Check if file should be rejected
		if (checkFileExist(real_filename) && keep_list.find(archive_filename) != keep_list.end())
			real_filename = trimFileName(reject_dir + std::string("/") + archive_filename);

		archive_entry_set_pathname(entry, const_cast<char*>
		                           (real_filename.c_str()));

		// Extract file
		unsigned int flags = ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK;
		if (archive_read_extract(archive, entry, flags) != ARCHIVE_OK) {
			// If a file fails to install we just print an error message and
			// continue trying to install the rest of the package.
			const char* msg = archive_error_string(archive);
			std::cerr << m_utilName << ": could not install " + archive_filename << ": " << msg << std::endl;
			continue;

		}

		// Check rejected file
		if (real_filename != original_filename) {
			bool remove_file = false;
			mode_t mode = archive_entry_mode(entry);

			// Directory
			if (S_ISDIR(mode))
				remove_file = checkPermissionsEqual(real_filename, original_filename);
			// Other files
			else
				remove_file = checkPermissionsEqual(real_filename, original_filename) &&
					(checkFileEmpty(real_filename) || checkFilesEqual(real_filename, original_filename));

			// Remove rejected file or signal about its existence
			if (remove_file)
				removeFile(reject_dir, real_filename);
			else
				std::cout << m_utilName << ": rejecting " << archive_filename << ", keeping existing version" << std::endl;
		}
	}
	progressInfo(cards::ACTION_ENUM_PKG_INSTALL_END);
	if (m_installedFilesNumber == 0) {
		if (archive_errno(archive) == 0)
		{
			m_actualError = cards::ERROR_ENUM_EMPTY_PACKAGE;
			treatErrors(filename);
		}
		else
		{
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
			treatErrors(filename);
		}
	}
	FREE_ARCHIVE(archive);
}
void pkgdbh::readRulesFile()
{
	m_actionRules.clear();
	unsigned int linecount = 0;
	const std::string filename = m_root + PKGADD_CONF;
	std::ifstream in(filename.c_str());

	if (in) {
		while (!in.eof()) {
			std::string line;
			getline(in, line);
			linecount++;
			if (!line.empty() && line[0] != '#') {
				if (line.length() >= PKGADD_CONF_MAXLINE) {
					m_actualError = cards::ERROR_ENUM_PKGADD_CONFIG_LINE_TOO_LONG;
					treatErrors(filename + ":" + itos(linecount));
				}
				char event[PKGADD_CONF_MAXLINE];
				char pattern[PKGADD_CONF_MAXLINE];
				char action[PKGADD_CONF_MAXLINE];
				char dummy[PKGADD_CONF_MAXLINE];
				if (sscanf(line.c_str(), "%s %s %s %s", event, pattern, action, dummy) != 3) {
					m_actualError = cards::ERROR_ENUM_PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS;
					treatErrors(filename + ":" + itos(linecount));
				}
				if (!strcmp(event, "UPGRADE") || !strcmp(event, "INSTALL") \
					|| !strcmp(event, "LDCONF") || !strcmp(event, "MIME_DB") \
					|| !strcmp(event, "INFO") || !strcmp(event, "ICONS") \
					|| !strcmp(event, "FONTS") \
					|| !strcmp(event, "QUERY_PIXBUF") || !strcmp(event, "GIO_QUERY") \
					|| !strcmp(event, "QUERY_IMOD3") || !strcmp(event, "QUERY_IMOD2") \
					|| !strcmp(event, "SCHEMAS") || !strcmp(event, "DESKTOP_DB")) {

					rule_t rule;
					if (!strcmp(event, "cards::UPGRADE"))
						rule.event = cards::UPGRADE;
					if (!strcmp(event, "cards::INSTALL"))
						rule.event = cards::INSTALL;
					if (!strcmp(event, "cards::INFO"))
						rule.event = cards::INFO;
					if (!strcmp(event, "cards::LDCONF"))
						rule.event = cards::LDCONF;
					if (!strcmp(event, "FONTS"))
						rule.event = cards::FONTS;
					if (!strcmp(event, "ICONS"))
						rule.event = cards::ICONS;
					if (!strcmp(event, "SCHEMAS"))
						rule.event = cards::SCHEMAS;
					if (!strcmp(event, "DESKTOP_DB"))
						rule.event = cards::DESKTOP_DB;
					if (!strcmp(event, "MIME_DB"))
						rule.event = cards::MIME_DB;
					if (!strcmp(event, "QUERY_PIXBUF"))
						rule.event = cards::QUERY_PIXBUF;
					if (!strcmp(event, "GIO_QUERY"))
						rule.event = cards::GIO_QUERY;
					if (!strcmp(event, "QUERY_IMOD3"))
						rule.event = cards::QUERY_IMOD3;
					if (!strcmp(event, "QUERY_IMOD2"))
						rule.event = cards::QUERY_IMOD2;


					rule.pattern = pattern;
					if (!strcmp(action, "YES")) {
						rule.action = true;
					} else if (!strcmp(action, "NO")) {
						rule.action = false;
					} else {
						m_actualError = cards::ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_ACTION;
						treatErrors(filename + ":" + itos(linecount) + ": '" +
							std::string(action));
					}
					m_actionRules.push_back(rule);
				} else {
					m_actualError = cards::ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_EVENT;
					treatErrors(filename + ":" + itos(linecount) + ": '" +
						std::string(event));
				}
			}
		}
		in.close();
	}
#ifdef DEBUG
	std::cerr << "Configuration:" << std::endl;
	for (auto j : m_actionRules )
		std::cerr << j.event << "\t" << j.pattern << "\t" << j.action << std::endl;
	std::cerr << std::endl;
#endif
}
void pkgdbh::runLastPostInstall()
{
	if ( m_postInstallList.size() > 0 ) {
		progressInfo(cards::ACTION_ENUM_PKG_POSTINSTALL_START);
#ifdef DEBUG
		for (auto i : m_postInstallList)
			std::cerr << i.second << " " << i.first << std::endl;
#endif
		process p;
		std::string args;
		for (auto i : m_postInstallList)
		switch ( i.second )
		{
			case cards::LDCONF:
				if (checkFileExist(m_root + LDCONFIG_CONF)) {
					args = LDCONFIG_CONF_ARGS + m_root;
					p.execute(m_root + LDCONFIG, args,0);
				}
			break;

			case cards::INFO:
				if (checkFileExist(m_root + INSTALL_INFO)) {
					args = INSTALL_INFO_ARGS + i.first;
					p.execute(m_root + INSTALL_INFO, args,0);
				}
			break;

			case cards::ICONS:
				if (checkFileExist(m_root + UPDATE_ICON)) {
					args = UPDATE_ICON_ARGS + i.first;
					p.execute(m_root + UPDATE_ICON,args,0);
				}
			break;

			case cards::FONTS:
				if (checkFileExist(m_root + MKFONTSCALE)) {
					args = MKFONTSCALE_ARGS + i.first;
					p.execute(m_root + MKFONTSCALE, args,0);
				}
				if (checkFileExist(m_root + MKFONTDIR)) {
					args = MKFONTDIR_ARGS + i.first;
					p.execute(m_root + MKFONTDIR, args,0);
				}
				if (checkFileExist(m_root + FC_CACHE)) {
					args = FC_CACHE_ARGS + i.first;
					p.execute(m_root + FC_CACHE, args,0);
				}
				break;

			case cards::SCHEMAS:
				if (checkFileExist(m_root + COMPILE_SCHEMAS)) {
					args = COMPILE_SCHEMAS_ARGS + i.first;
					p.execute(m_root + COMPILE_SCHEMAS, args,0);
				}
			break;

			case cards::DESKTOP_DB:
				if (checkFileExist(m_root + UPDATE_DESKTOP_DB)) {
					args = UPDATE_DESKTOP_DB_ARGS + i.first;
					p.execute(m_root + UPDATE_DESKTOP_DB, args,0);
				}
			break;

			case cards::MIME_DB:
				if (checkFileExist(m_root + UPDATE_MIME_DB)) {
					args = UPDATE_MIME_DB_ARGS + i.first;
					p.execute(m_root + UPDATE_MIME_DB, args,0);
				}
			break;

			case cards::QUERY_PIXBUF:
				if (checkFileExist(m_root + GDK_PIXBUF_QUERY_LOADER)) {
					args = GDK_PIXBUF_QUERY_LOADER_ARGS;
					p.execute(m_root + GDK_PIXBUF_QUERY_LOADER, args,0);
				}
			break;

			case cards::GIO_QUERY:
				if (checkFileExist(m_root + GIO_QUERYMODULES)) {
					args = GIO_QUERYMODULES_ARGS;
					p.execute(m_root + GIO_QUERYMODULES, args,0);
				}
			break;

			case cards::QUERY_IMOD3:
				if (checkFileExist(m_root + QUERY_IMMODULES_3)) {
					args = QUERY_IMMODULES_3_ARGS;
					p.execute(m_root + QUERY_IMMODULES_3, args,0);
				}
			break;

			case cards::QUERY_IMOD2:
				if (checkFileExist(m_root + QUERY_IMMODULES_2)) {
					args = QUERY_IMMODULES_2_ARGS;
					p.execute(m_root + QUERY_IMMODULES_2, args,0);
				}
			break;
		}
		progressInfo(cards::ACTION_ENUM_PKG_POSTINSTALL_END);
	}
}
bool pkgdbh::checkRuleAppliesToFile
	(const rule_t& rule, const std::string& file)
{
	regex_t preg;
	bool ret;

	if (regcomp(&preg, rule.pattern.c_str(), REG_EXTENDED | REG_NOSUB)) {
		m_actualError = cards::ERROR_ENUM_CANNOT_COMPILE_REGULAR_EXPRESSION;
		treatErrors(rule.pattern);
	}
	ret = !regexec(&preg, file.c_str(), 0, 0, 0);
	regfree(&preg);

	return ret;
}
void pkgdbh::getFootprintPackage(std::string& filename)
{
	unsigned int i;
	struct archive* archive;
	struct archive_entry* entry;

	std::map<std::string, mode_t> hardlink_target_modes;

	// We first do a run over the archive and remember the modes
	// of regular files.
	// In the second run, we print the footprint - using the stored
	// modes for hardlinks.
	//
	// FIXME the code duplication here is butt ugly
	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
			{
				m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
				treatErrors(filename);
			}
      //         throw RunTimeErrorWithErrno("could not open " + filename, archive_errno(archive));

	for (i = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++i) {

		mode_t mode = archive_entry_mode(entry);

		if (!archive_entry_hardlink(entry)) {
			const char *s = archive_entry_pathname(entry);

			hardlink_target_modes[s] = mode;
		}

		if (S_ISREG(mode) && archive_read_data_skip(archive))
		{
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
			treatErrors(filename);
		//	throw RunTimeErrorWithErrno("could not read " + filename, archive_errno(archive));
		}
	}
	FREE_ARCHIVE(archive);

	// Too bad, there doesn't seem to be a way to reuse our archive
	// instance
	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
			{
				m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
				treatErrors(filename);
        // throw RunTimeErrorWithErrno("could not open " + filename, archive_errno(archive));
			}
	for (i = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++i) {
		mode_t mode = archive_entry_mode(entry);

		// Access permissions
		if (S_ISLNK(mode)) {
			// Access permissions on symlinks differ among filesystems, e.g. XFS and ext2 have different.
			// To avoid getting different footprints we always use "lrwxrwxrwx".
			std::cout << "lrwxrwxrwx";
		} else {
			const char *h = archive_entry_hardlink(entry);

			if (h)
				std::cout << mtos(hardlink_target_modes[h]);
			else
				std::cout << mtos(mode);
		}

		std::cout << '\t';

		// User
		uid_t uid = archive_entry_uid(entry);
		struct passwd* pw = getpwuid(uid);
		if (pw)
			std::cout << pw->pw_name;
		else
			std::cout << uid;

		std::cout << '/';

		// Group
		gid_t gid = archive_entry_gid(entry);
		struct group* gr = getgrgid(gid);
		if (gr)
			std::cout << gr->gr_name;
		else
			std::cout << gid;

		// Filename
		std::cout << '\t' << archive_entry_pathname(entry);

		// Special cases
		if (S_ISLNK(mode)) {
			// Symlink
			std::cout << " -> " << archive_entry_symlink(entry);
		} else if (S_ISCHR(mode) ||
		           S_ISBLK(mode)) {
			// Device
			std::cout << " (" << archive_entry_rdevmajor(entry)
			     << ", " << archive_entry_rdevminor(entry)
			     << ")";
		} else if (S_ISREG(mode) &&
		           archive_entry_size(entry) == 0) {
			// Empty regular file
			std::cout << " (EMPTY)";
		}

		std::cout << '\n';

		if (S_ISREG(mode) && archive_read_data_skip(archive))
		{
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
			treatErrors(filename);
		//	throw RunTimeErrorWithErrno("could not read " + filename, archive_errno(archive));
		}
	}

	if (i == 0) {
		if (archive_errno(archive) == 0)
		{
			m_actualError = cards::ERROR_ENUM_EMPTY_PACKAGE;
			treatErrors(filename);
		}
		else
		{
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
			treatErrors(filename);
		}
	}
	FREE_ARCHIVE(archive);
}

void pkgdbh::print_version() const
{
	std::cout << m_utilName << " (cards) " << PACKAGE_VERSION << std::endl;
}

unsigned int pkgdbh::getFilesNumber()
{
    return m_filesNumber;
}

unsigned int pkgdbh::getInstalledFilesNumber()
{
    return m_installedFilesNumber;
}
std::set<std::string> pkgdbh::getFilesList()
{
	return m_filesList;
}
