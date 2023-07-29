//
//  pkgdbh.cxx
//
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 - 2023 by NuTyX team (http://nutyx.org)
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

#include "pkgdbh.h"

using __gnu_cxx::stdio_filebuf;

Pkgdbh::Pkgdbh(const std::string& name)
	: m_utilName(name), m_DB_Empty(true), m_miniDB_Empty(true)
{
	openlog(m_utilName.c_str(),LOG_CONS,LOG_LOCAL7);
}
Pkgdbh::Pkgdbh()
	: m_utilName("pkgdbh"), m_DB_Empty(true), m_miniDB_Empty(true)
{
	m_root="/";
}
Pkgdbh::~Pkgdbh()
{
#ifndef NDEBUG
		syslog(LOG_INFO,"Closing log...");
#endif
		runLastPostInstall();
		void closelog(void);
}
void Pkgdbh::parseArguments(int argc, char** argv)
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
void Pkgdbh::run(int argc, char** argv)
{
	parseArguments(argc,argv);
	run();
}

void Pkgdbh::treatErrors(const std::string& s) const
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
void Pkgdbh::progressInfo(cards::ActionEnum action)
{
	m_actualAction = action;
	progressInfo();
}
void Pkgdbh::progressInfo()
{
	using namespace cards;
  static int j=0;
  switch ( m_actualAction )
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

std::set<std::string> Pkgdbh::getListOfPackagesNames()
{
	if (m_listOfPackagesNames.empty())
		getListOfPackagesNames(m_root);
	return m_listOfPackagesNames;
}
std::string Pkgdbh::getSingleItem(const std::string& PackageName, const char i) const
{
	const std::string metaFile = m_root + PKG_DB_DIR + PackageName + '/' + PKG_META;
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
std::set<std::string> Pkgdbh::getSetOfItems(const std::string& PackageName, const char i) const
{
	const std::string metaFile = m_root + PKG_DB_DIR + PackageName + '/' + PKG_META;
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
std::string Pkgdbh::getDescription(const std::string& name) const
{
	return getSingleItem(name,'D');
}
std::string Pkgdbh::getVersion(const std::string& name) const
{
	return getSingleItem(name,'V');
}
std::string Pkgdbh::getCollection(const std::string& name) const
{
	return getSingleItem(name,'c');
}
std::string Pkgdbh::getSets(const std::string& name) const
{
	std::string s;
	for (auto i: getSetOfItems(name,'s'))
		s = s + " " + i;
	return s;
}
std::set<std::string> Pkgdbh::getSetOfSets(const std::string& name) const
{
	return getSetOfItems(name,'s');
}
std::set<std::string> Pkgdbh::getSetOfCategories (const std::string& name) const
{
	return getSetOfItems(name, 'T');
}
std::string Pkgdbh::getCategories(const std::string& name) const
{
	std::string s;
	for (auto i: getSetOfItems(name,'T'))
		s = s + " " + i;
	return s;
}
std::string Pkgdbh::getArch(const std::string& name) const
{
	return getSingleItem(name,'A');
}
std::string Pkgdbh::getLicense(const std::string& name) const
{
	return getSingleItem(name,'L');
}
int Pkgdbh::getSize(const std::string& name) const
{
	std::string r = getSingleItem(name,'S');
	return atoi(r.c_str());
}
int Pkgdbh::getRelease(const std::string& name) const
{
	std::string r = getSingleItem(name,'r');
	return atoi(r.c_str());
}
/* Append to the "DB" the number of packages founds
 * (directory containing a file named files
 * */
int Pkgdbh::getListOfPackagesNames (const std::string& path)
{
	if (! m_listOfPackagesNames.empty())
		return m_listOfPackagesNames.size();

	const std::string pathdb =  m_root + PKG_DB_DIR;
#ifndef NDEBUG
	std::cerr << "pathdb: " << pathdb << std::endl;
#endif
	if ( findDir(m_listOfPackagesNames, pathdb) != 0 ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
		treatErrors(pathdb);
	}
#ifndef NDEBUG
	std::cerr << "Number of Packages: "
		<< m_listOfPackagesNames.size()
		<< std::endl;
#endif
	return m_listOfPackagesNames.size();
}

/* get details info of a package */
std::pair<std::string, cards::Db>
Pkgdbh::getInfosPackage( const std::string& packageName )
{
	std::pair<std::string, cards::Db> result;

	result.first = packageName;
	return result;
}

std::set<std::string>
Pkgdbh::getSetOfFiles( const std::string& packageName )
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
Pkgdbh::buildDatabase(const bool& progress,
		const bool& simple,
		const bool& all,
		const bool& files,
		const std::string& packageName)
{
	using namespace cards;
	/*
	 * This part is done in every cases
	 */
	cleanupMetaFiles(m_root);
	if (progress) {
		progressInfo(ACTION_ENUM_DB_OPEN_START);
	}
#ifndef NDEBUG
	std::cerr << "m_root: " << m_root << std::endl;
#endif
	if (m_listOfPackagesNames.empty() )
		getListOfPackagesNames (m_root);

	for ( auto name : m_listOfPackagesNames) {
		if (progress) {
			progressInfo(ACTION_ENUM_DB_OPEN_RUN);
		}
		const std::string metaFile = m_root
		+ PKG_DB_DIR
		+ name
		+ '/'
		+ PKG_META;
		std::set<std::string> fileContent;
		parseFile(fileContent,metaFile.c_str());
		m_listOfAlias[name] = name;
		for ( auto attribute : fileContent) {
			if ( attribute[0] != 'A' )
				break;
			m_listOfAlias[attribute.substr(1)] = name;
		}
	}

	if ( !simple && !all && !files && (packageName.size() == 0) )
		return;

	if (packageName.size() > 0) {
		std::string name = m_listOfAlias[packageName];
		cards::Db info;
		info.files=getSetOfFiles(name);
		m_listOfPackages[name] = info;
	}
	if ( !simple && !all && !files ) {
		if (progress) {
			m_actualAction = ACTION_ENUM_DB_OPEN_END;
			progressInfo();
		}
		return;
	}
	if (simple) {
		cards::Db info;
		for ( auto pkgName : m_listOfPackagesNames) {
			if (progress) {
				m_actualAction = ACTION_ENUM_DB_OPEN_RUN;
				progressInfo();
			}
			const std::string metaFile = m_root
			+ PKG_DB_DIR
			+ pkgName
			+ '/'
			+ PKG_META;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
			m_listOfAlias[pkgName] = pkgName;
			unsigned short flags = 0;
			info.release(1);
			for ( auto attribute : fileContent) {
				if ( attribute[0] == 'B' ) {
					info.build(strtoul(attribute.substr(1).c_str(),NULL,0));
					flags++;
				}
				if ( attribute[0] == 'V' ) {
					info.version(attribute.substr(1));
					flags=flags + 2;
				}
				if ( attribute[0] == 'c' ) {
					info.collection(attribute.substr(1));
					flags = flags + 4;
				}
				/* As a group is not always present we cannot
				 * depend on a found one to break */
				if ( attribute[0] == 'g' ) {
					info.group(attribute.substr(1));
				}
				/* As a std::set is not always present we cannot
				 * depen on a found one to break */
				if ( attribute[0] == 's' ) {
					std::string s;
					s = s + " " + attribute.substr(1);
					info.sets(s);
				}
				if ( attribute[0] == 'r' ) {
					info.release(atoi(attribute.substr(1).c_str()));
					flags = flags + 8;
				}
				if ( flags == 15 ) {
					m_listOfPackages[pkgName] = info;
					break;
				}
			}
		}
	}
	if (progress)
	{
		m_actualAction = ACTION_ENUM_DB_OPEN_END;
		progressInfo();
	}
}

/**
 * Populate m_listOfPackagesWithDeps with:
 * - Name of the package
 * - Dependencies list of the package
 * */
void
Pkgdbh::buildSimpleDependenciesDatabase()
{
	if (m_listOfPackagesNames.empty() )
			getListOfPackagesNames (m_root);
	for ( auto i : m_listOfPackagesNames) {
		std::pair < std::string, std::set<std::string> > packageWithDeps;
		packageWithDeps.first=i;
		const std::string metaFile = m_root + PKG_DB_DIR + i + '/' + PKG_META;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
		for (auto attribute : fileContent) {
			if ( attribute[0] == 'R' ) {
				std::string s = attribute;
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
Pkgdbh::buildSimpleDatabase()
{
	if (m_miniDB_Empty) {
		if (m_listOfPackagesNames.empty() )
			getListOfPackagesNames (m_root);
		for ( auto i : m_listOfPackagesNames) {
			cards::Db info;
			const std::string metaFile = m_root + PKG_DB_DIR + i + '/' + PKG_META;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
			info.release(1);
			info.dependency(false);
			m_listOfAlias[i] = i;
			for (auto attribute : fileContent) {
				if ( attribute[0] == 'c' ) {
					std::string s = attribute;
					info.collection(s.substr(1));
				}
				if ( attribute[0] == 's' ) {
					std::string s;
					s = s + " " + attribute.substr(1);
					info.sets(s);
				}
				if ( attribute[0] == 'V' ) {
					std::string s = attribute;
					info.version(s.substr(1));
				}
				if ( attribute[0] == 'r' ) {
					std::string s = attribute;
					info.release( atoi(s.substr(1).c_str()) );
				}
				if ( attribute[0] == 'B' ) {
					std::string s = attribute;
					info.build( strtoul(s.substr(1).c_str(),NULL,0) );
				}
				if ( attribute[0] == 'g' ) {
					std::string s = attribute;
					info.group( s.substr(1) );
				}
				if ( attribute[0] == 'A' ) {
					std::string s;
					s = s + " " + attribute.substr(1);
					info.alias(s);
				}
				if ( attribute[0] == 'P' ) {
					std::string s = attribute;
					info.packager(s.substr(1));
				}
				if ( attribute[0] == 'd' ) {
					std::string s = attribute;
					if ( s == "d1" )
						info.dependency(true);
				}

			}
			m_listOfPackages[i] = info;
		}
		m_miniDB_Empty=false;
	}
#ifndef NDEBUG
	for (auto i : m_listOfAlias)
		std::cerr << "Alias: " << i.first << ", Package: " << i.second << std::endl;
#endif
}
 /**
 * Populate the database with all details infos
 */
void Pkgdbh::buildCompleteDatabase(const bool& silent)
{
	using namespace cards;
	cleanupMetaFiles(m_root);
	if (m_DB_Empty) {
		if (m_listOfPackagesNames.empty() )
			getListOfPackagesNames (m_root);

		if (!silent) {
			m_actualAction = ACTION_ENUM_DB_OPEN_START;
			progressInfo();
		}
#ifndef NDEBUG
		std::cerr << "m_root: " << m_root<< std::endl;
#endif

		for (auto i : m_listOfPackagesNames) {
			if (!silent) {
				m_actualAction = ACTION_ENUM_DB_OPEN_RUN;
				progressInfo();
			}
			cards::Db info;
			const std::string metaFileDir = m_root + PKG_DB_DIR + i;
			const std::string metaFile = metaFileDir + '/' + PKG_META;
			info.install( getEpochModifyTimeFile(metaFileDir) );
			std::set<std::pair<std::string,time_t>> dependencies;
			std::set<std::string> fileContent;
			parseFile(fileContent,metaFile.c_str());
			info.release(1);
			info.dependency(false);
			m_listOfAlias[i] = i;
			for (auto attribute : fileContent) {
				if ( attribute[0] == 'C' ) {
					std::string s = attribute;
					info.contributors( s.substr(1) );
				}
				if ( attribute[0] == 'D' ) {
					std::string s = attribute;
					info.description( s.substr(1) );
				}
				if ( attribute[0] == 'B' ) {
					std::string s = attribute;
					info.build( strtoul(s.substr(1).c_str(),NULL,0) );
				}
				if ( attribute[0] == 'U' ) {
					std::string s = attribute;
					info.url( s.substr(1) );
				}
				if ( attribute[0] == 'L' ) {
					std::string s = attribute;
					info.license( s.substr(1) );
				}
				if ( attribute[0] == 'M' ) {
					std::string s = attribute;
					info.maintainer( s.substr(1) );
				}
				if ( attribute[0] == 'P' ) {
					std::string s = attribute;
					info.packager( s.substr(1) );
				}
				if ( attribute[0] == 'V' ) {
					std::string s = attribute;
					info.version( s.substr(1) );
				}
				if ( attribute[0] == 'r' ) {
					std::string s = attribute;
					info.release( atoi(s.substr(1).c_str()) );
				}
				if ( attribute[0] == 'a' ) {
					std::string s = attribute;
					info.arch( s.substr(1) );
				}
				if ( attribute[0] == 'c' ) {
					info.collection(attribute.substr(1));
				}
				if ( attribute[0] == 's' ) {
					std::string s;
					s = s + " " + attribute.substr(1);
					info.sets(s);
				}
				if ( attribute[0] == 'g' ) {
					std::string s = attribute;
					info.group( s.substr(1) );
				}
				if ( attribute[0] == 'd' ) {
					std::string s = attribute;
					if ( s == "d1" )
						info.dependency(true);
				}
				if ( attribute[0] == 'S' ) {
					std::string s = attribute;
					info.space( atoi(s.substr(1).c_str()) );
				}
				if ( attribute[0] == 'A' ) {
					std::string s;
					s = s + " " + attribute.substr(1);
					info.alias(s);
					m_listOfAlias[attribute.substr(1)] = i;
				}
				if ( attribute[0] == 'T' ) {
					std::string s;
					s = s + " " + attribute.substr(1);
					info.categories(s);
				}
				if ( attribute[0] == 'R' ) {
					std::string s = attribute;
					std::pair<std::string,time_t > NameEpoch;
					NameEpoch.first=s.substr(1,s.size()-11);
					NameEpoch.second=strtoul((s.substr(s.size()-10)).c_str(),NULL,0);
					dependencies.insert(NameEpoch);
				}
			}
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
#ifndef NDEBUG
		std::cerr << std::endl;
		std::cerr << m_listOfPackages.size()
		<< " packages found in database " << std::endl;
#endif
		if (!silent)
		{
			m_actualAction = ACTION_ENUM_DB_OPEN_END;
			progressInfo();
		}
		m_DB_Empty=false;
	}
}
void Pkgdbh::moveMetaFilesPackage(const std::string& name, cards::Db& info)
{
	std::set<std::string> metaFilesList;
	m_actualAction = cards::ACTION_ENUM_PKG_MOVE_META_START;
	progressInfo();
	const std::string packagedir = m_root + PKG_DB_DIR ;
	const std::string packagenamedir = m_root + PKG_DB_DIR + name ;

	for (auto i: info.files)
	{
		if ( i[0] == '.' ) {
#ifndef NDEBUG
			std::cout << "i: " << i << std::endl;
#endif
			metaFilesList.insert(metaFilesList.end(), i );
		}
	}
	for ( auto i : metaFilesList) info.files.erase(i);
	removeFile ( m_root, "/.MTREE");
	metaFilesList.insert(".META");
	std::set<std::string> fileContent;
	if ( parseFile(fileContent,".META") == -1 ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_FIND_FILE;
		treatErrors(".META");
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
		if ( i == ".POST" ) {
			if (copyFile(i.c_str(), file.c_str()) == -1) {
				m_actualError = cards::ERROR_ENUM_CANNOT_COPY_FILE;
				treatErrors( file  + " to " + i);
			}
		}
	}
	if (m_dependency) {
		std::string file = packagenamedir + "/META";
		fileContent.insert("d1");
		std::ofstream out(file);
		for ( auto i: fileContent) out << i << std::endl;
		out.close();
		m_dependency=false;
	}
	m_actualAction = cards::ACTION_ENUM_PKG_MOVE_META_END;
	progressInfo();
}
void Pkgdbh::addPackageFilesRefsToDB(const std::string& name, const cards::Db& info)
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
bool Pkgdbh::checkPackageNameExist(const std::string& name) const
{
	return (m_listOfAlias.find(name) != m_listOfAlias.end());
}
bool Pkgdbh::checkDependency(const std::string& name)
{
	if  ( checkPackageNameExist(name) )
		return m_listOfPackages[name].dependency();
	return false;
}
void Pkgdbh::setDependency()
{
	m_dependency=true;
}
void Pkgdbh::resetDependency()
{
	m_dependency=false;
}

bool
Pkgdbh::checkPackageNameUptodate(std::pair<std::string, cards::Db>& archiveName)
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

bool Pkgdbh::checkPackageNameBuildDateSame(const std::pair<std::string,time_t>& dependencieNameBuild)
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
void Pkgdbh::removePackageFilesRefsFromDB(const std::string& name)
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
#ifndef NDEBUG
				std::cerr  << "File: " << filename << " is removed"<< std::endl;
#endif
			}
	}
	if( remove(packagenamedir.c_str()) == -1) {
		const char* msg = strerror(errno);
		std::cerr << m_utilName << ": could not remove " << packagenamedir << ": " << msg << std::endl;
	}
#ifndef NDEBUG
	std::cerr  << "Directory: " << packagenamedir << " is removed"<< std::endl;
#endif
}

/* Remove the physical files after followings some rules */
void Pkgdbh::removePackageFiles(const std::string& name)
{
	m_filesList = m_listOfPackages[name].files;
	m_listOfPackages.erase(name);
	m_packageName =  name ;

#ifndef NDEBUG
	std::cerr << "Removing package phase 1 (all files in package):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files that still have references
	for (auto i : m_listOfPackages)
		for (auto j : i.second.files)
			m_filesList.erase(j);

#ifndef NDEBUG
	std::cerr << "Removing package phase 2 (files that still have references excluded):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	m_actualAction = cards::ACTION_ENUM_RM_PKG_FILES_START;
	progressInfo();
	// Delete the files from bottom to up to make sure we delete the contents of any folder before
	for (std::set<std::string>::const_reverse_iterator i = m_filesList.rbegin(); i != m_filesList.rend(); ++i) {
		m_actualAction = cards::ACTION_ENUM_RM_PKG_FILES_RUN;
		progressInfo();
		const std::string filename = m_root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			const char* msg = strerror(errno);
			std::cerr << m_utilName << ": could not remove " << filename << ": " << msg << std::endl;
		}
	}
	m_actualAction = cards::ACTION_ENUM_RM_PKG_FILES_END;
	progressInfo();
}

void Pkgdbh::removePackageFiles(const std::string& name, const std::set<std::string>& keep_list)
{
	m_filesList = m_listOfPackages[name].files;
	m_listOfPackages.erase(name);
	m_packageName =  name ;
#ifndef NDEBUG
	std::cerr << "Removing package phase 1 (all files in package):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files found in the keep list
	for (auto i : keep_list) m_filesList.erase(i);

#ifndef NDEBUG
	std::cerr << "Removing package phase 2 (files that is in the keep list excluded):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Don't delete files that still have references
	for (auto i: m_listOfPackages)
		for (auto j: i.second.files)
			m_filesList.erase(j);

#ifndef NDEBUG
	std::cerr << "Removing package phase 3 (files that still have references excluded):" << std::endl;
	copy(m_filesList.begin(), m_filesList.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// Delete the files
	m_actualAction = cards::ACTION_ENUM_RM_PKG_FILES_START;
	progressInfo();
	for (std::set<std::string>::const_reverse_iterator i = m_filesList.rbegin(); i != m_filesList.rend(); ++i) {
		m_actualAction = cards::ACTION_ENUM_RM_PKG_FILES_RUN;
		progressInfo();
		const std::string filename = m_root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			std::cerr << m_utilName << ": could not remove " << filename << ": " << msg << std::endl;
		}
	}
	m_actualAction = cards::ACTION_ENUM_RM_PKG_FILES_END;
	progressInfo();
}

void Pkgdbh::removePackageFilesRefsFromDB(std::set<std::string> files, const std::set<std::string>& keep_list)
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
#ifndef NDEBUG
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
Pkgdbh::getConflictsFilesList (const std::string& name, cards::Db& info)
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

#ifndef NDEBUG
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

#ifndef NDEBUG
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

#ifndef NDEBUG
	std::cerr << "Conflicts phase 3 (directories excluded):" << std::endl;
	copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
	std::cerr << std::endl;
#endif

	// If this is an upgrade, remove files already owned by this package
	if (m_listOfPackages.find(name) != m_listOfPackages.end()) {
		for (auto i : m_listOfPackages[name].files)
			files.erase(i);

#ifndef NDEBUG
		std::cerr << "Conflicts phase 4 (files already owned by this package excluded):" << std::endl;
		copy(files.begin(), files.end(), std::ostream_iterator<std::string>(std::cerr, "\n"));
		std::cerr << std::endl;
#endif
	}

	return files;
}
std::pair<std::string, cards::Db>
Pkgdbh::openArchivePackage(const std::string& filename)
{
	std::string packageArchiveName;
	std::pair<std::string, cards::Db> result;
	ArchiveUtils packageArchive(filename.c_str());
#ifndef NDEBUG
	std::cerr << "Number of files: " << packageArchive.size() << std::endl;
#endif
	std::string basename(filename, filename.rfind('/') + 1);
	m_filesNumber = packageArchive.size();
	if (m_filesNumber == 0 ) {
		m_actualError = cards::ERROR_ENUM_EMPTY_PACKAGE;
		treatErrors(basename);
	}
	packageArchiveName = packageArchive.name();
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
	if (packageArchiveName.empty() ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_DETERMINE_NAME_BUILDNR;
		treatErrors(basename);
	}
	result.first = packageArchiveName;
	result.second.description(packageArchive.description());
	result.second.url(packageArchive.url());
	result.second.maintainer(packageArchive.maintainer());
	result.second.packager(packageArchive.packager());
	result.second.version(packageArchive.version());
	result.second.release(packageArchive.release());
	result.second.arch(packageArchive.arch());
	result.second.build(packageArchive.buildn());
	result.second.group(packageArchive.group());

	std::set<std::string> fileList =  packageArchive.setofFiles();
	for (auto i :fileList) {
		result.second.files.insert(i);
	}
	result.second.dependencies(packageArchive.listofDependenciesBuildDate());
	m_packageName = packageArchiveName;
	return result;
}
std::set< std::pair<std::string,time_t> >
Pkgdbh::getPackageDependencies (const std::string& filename)
{
	std::pair<std::string, cards::Db> packageArchive;
	std::set< std::pair<std::string,time_t> > packageNameDepsBuildTime;
	std::set< std::pair<std::string,time_t> > packageNameDepsBuildTimeTemp;
	packageArchive = openArchivePackage(filename);
#ifndef NDEBUG
	std::cerr << "----> Begin of Direct Dependencies of " << packageArchive.first << std::endl;
#endif
	if ( checkPackageNameUptodate(packageArchive ) ) {
#ifndef NDEBUG
		std::cerr << packageArchive.first << " already installed and Up To Dated" << std::endl
			<< "----> NO Direct Dependencies" << std::endl
			<< "----> End of Direct Dependencies" << std::endl;
#endif
		return packageNameDepsBuildTime;
	}
	if (! packageArchive.second.dependencies().empty() )
		m_listOfRepoPackages[packageArchive.first] = packageArchive.second;
#ifndef NDEBUG
	std::cerr << "----> End of Direct Dependencies" << std::endl;
#endif
	packageNameDepsBuildTimeTemp = packageArchive.second.dependencies();
#ifndef NDEBUG
	std::cerr << "----> Before cleanup: " << packageArchive.first << std::endl;
	for (auto it : packageNameDepsBuildTimeTemp ) std::cerr << it.first << it.second<< " ";
	std::cerr << std::endl;
	int i=1;
#endif
	for (auto it : packageNameDepsBuildTimeTemp ) {
#ifndef NDEBUG
		std::cerr << it.first << " " <<  it.second << std::endl;
		std::cerr << "packageArchiveName:" <<packageArchive.first << std::endl;
#endif
		/*
		 * If actual and already present don't add the dep
		 */
		if ( checkPackageNameBuildDateSame(it)  )
			continue;
		packageNameDepsBuildTime.insert(it);
			//packageNameDepsBuildTime.erase(it);
#ifndef NDEBUG
		std::cerr << "----> " << it.first << " deleted" << std::endl;
		std::cerr << i << std::endl;
		i++;
#endif
	}
	if (! packageNameDepsBuildTime.empty() )
		m_listOfRepoPackages[packageArchive.first].dependencies(packageNameDepsBuildTime);
#ifndef NDEBUG
	std::cerr << "----> Number of remains direct deps: "
		<< packageArchive.first
		<< ": "
		<< packageNameDepsBuildTime.size()
		<< "/"
		<< packageArchive.second.dependencies().size()
		<< std::endl;
	for ( auto it : packageNameDepsBuildTime )
		std::cerr << it.first << " " ;
	std::cerr << std::endl;
#endif
	return packageNameDepsBuildTime;
}
void Pkgdbh::extractAndRunPREfromPackage(const std::string& filename)
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
		m_actualAction = cards::ACTION_ENUM_PKG_PREINSTALL_START;
		progressInfo();
		process preinstall(SHELL,PKG_PRE_INSTALL, 0 );
		if (preinstall.executeShell()) {
			exit(EXIT_FAILURE);
		}
		removeFile(m_root,PKG_PRE_INSTALL);
		m_actualAction = cards::ACTION_ENUM_PKG_PREINSTALL_END;
		progressInfo();
	}
	chdir(buf);
}
void Pkgdbh::installArchivePackage
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
#ifndef NDEBUG
	std::cout << "absm_root: " <<  absm_root  << " and m_root: " << m_root<< std::endl;
#endif
	m_actualAction = cards::ACTION_ENUM_PKG_INSTALL_START;
	progressInfo();
	for (m_installedFilesNumber = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++m_installedFilesNumber) {

		m_actualAction = cards::ACTION_ENUM_PKG_INSTALL_RUN;
		progressInfo();
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
	m_actualAction = cards::ACTION_ENUM_PKG_INSTALL_END;
	progressInfo();
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
void Pkgdbh::readRulesFile()
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
					if (!strcmp(event, "UPGRADE"))
						rule.event = UPGRADE;
					if (!strcmp(event, "INSTALL"))
						rule.event = INSTALL;
					if (!strcmp(event, "INFO"))
						rule.event = INFO;
					if (!strcmp(event, "LDCONF"))
						rule.event = LDCONF;
					if (!strcmp(event, "FONTS"))
						rule.event = FONTS;
					if (!strcmp(event, "ICONS"))
						rule.event = ICONS;
					if (!strcmp(event, "SCHEMAS"))
						rule.event = SCHEMAS;
					if (!strcmp(event, "DESKTOP_DB"))
						rule.event = DESKTOP_DB;
					if (!strcmp(event, "MIME_DB"))
						rule.event = MIME_DB;
					if (!strcmp(event, "QUERY_PIXBUF"))
						rule.event = QUERY_PIXBUF;
					if (!strcmp(event, "GIO_QUERY"))
						rule.event = GIO_QUERY;
					if (!strcmp(event, "QUERY_IMOD3"))
						rule.event = QUERY_IMOD3;
					if (!strcmp(event, "QUERY_IMOD2"))
						rule.event = QUERY_IMOD2;


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
#ifndef NDEBUG
	std::cerr << "Configuration:" << std::endl;
	for (auto j : m_actionRules )
		std::cerr << j.event << "\t" << j.pattern << "\t" << j.action << std::endl;
	std::cerr << std::endl;
#endif
}
void Pkgdbh::runLastPostInstall()
{
	if ( m_postInstallList.size() > 0 ) {
		m_actualAction = cards::ACTION_ENUM_PKG_POSTINSTALL_START;
		progressInfo();
#ifndef NDEBUG
		for (auto i : m_postInstallList)
			std::cerr << i.second << " " << i.first << std::endl;
#endif
		process p;
		std::string args;
		for (auto i : m_postInstallList)
		switch ( i.second )
		{
			case LDCONF:
				if (checkFileExist(m_root + LDCONFIG_CONF)) {
					args = LDCONFIG_CONF_ARGS + m_root;
					p.execute(m_root + LDCONFIG, args,0);
				}
			break;

			case INFO:
				if (checkFileExist(m_root + INSTALL_INFO)) {
					args = INSTALL_INFO_ARGS + i.first;
					p.execute(m_root + INSTALL_INFO, args,0);
				}
			break;

			case ICONS:
				if (checkFileExist(m_root + UPDATE_ICON)) {
					args = UPDATE_ICON_ARGS + i.first;
					p.execute(m_root + UPDATE_ICON,args,0);
				}
			break;

			case FONTS:
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

			case SCHEMAS:
				if (checkFileExist(m_root + COMPILE_SCHEMAS)) {
					args = COMPILE_SCHEMAS_ARGS + i.first;
					p.execute(m_root + COMPILE_SCHEMAS, args,0);
				}
			break;

			case DESKTOP_DB:
				if (checkFileExist(m_root + UPDATE_DESKTOP_DB)) {
					args = UPDATE_DESKTOP_DB_ARGS + i.first;
					p.execute(m_root + UPDATE_DESKTOP_DB, args,0);
				}
			break;

			case MIME_DB:
				if (checkFileExist(m_root + UPDATE_MIME_DB)) {
					args = UPDATE_MIME_DB_ARGS + i.first;
					p.execute(m_root + UPDATE_MIME_DB, args,0);
				}
			break;

			case QUERY_PIXBUF:
				if (checkFileExist(m_root + GDK_PIXBUF_QUERY_LOADER)) {
					args = GDK_PIXBUF_QUERY_LOADER_ARGS;
					p.execute(m_root + GDK_PIXBUF_QUERY_LOADER, args,0);
				}
			break;

			case GIO_QUERY:
				if (checkFileExist(m_root + GIO_QUERYMODULES)) {
					args = GIO_QUERYMODULES_ARGS;
					p.execute(m_root + GIO_QUERYMODULES, args,0);
				}
			break;

			case QUERY_IMOD3:
				if (checkFileExist(m_root + QUERY_IMMODULES_3)) {
					args = QUERY_IMMODULES_3_ARGS;
					p.execute(m_root + QUERY_IMMODULES_3, args,0);
				}
			break;

			case QUERY_IMOD2:
				if (checkFileExist(m_root + QUERY_IMMODULES_2)) {
					args = QUERY_IMMODULES_2_ARGS;
					p.execute(m_root + QUERY_IMMODULES_2, args,0);
				}
			break;
		}
		m_actualAction = cards::ACTION_ENUM_PKG_POSTINSTALL_END;
		progressInfo();
	}
}
bool Pkgdbh::checkRuleAppliesToFile
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
void Pkgdbh::getFootprintPackage(std::string& filename)
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

void Pkgdbh::print_version() const
{
	std::cout << m_utilName << " (cards) " << VERSION << std::endl;
}

unsigned int Pkgdbh::getFilesNumber()
{
    return m_filesNumber;
}

unsigned int Pkgdbh::getInstalledFilesNumber()
{
    return m_installedFilesNumber;
}
std::set<std::string> Pkgdbh::getFilesList()
{
	return m_filesList;
}
Db_lock::Db_lock(const std::string& m_root, bool exclusive)
	: m_dir(0)
{
  // Ignore signals
  memset(&m_sa, 0, sizeof(m_sa));
  m_sa.sa_handler = SIG_IGN;
  sigaction(SIGHUP, &m_sa, NULL);
  sigaction(SIGINT, &m_sa, NULL);
  sigaction(SIGQUIT, &m_sa, NULL);
  sigaction(SIGTERM, &m_sa, NULL);

	const std::string dirname = trimFileName(m_root + std::string("/") + PKG_DB_DIR);
#ifndef NDEBUG
	std::cerr << "Lock the database " << dirname << std::endl;
#endif
	if (!(m_dir = opendir(dirname.c_str())))
		throw RunTimeErrorWithErrno("could not read directory " + dirname);

	if (flock(dirfd(m_dir), (exclusive ? LOCK_EX : LOCK_SH) | LOCK_NB) == -1) {
		if (errno == EWOULDBLOCK)
			throw std::runtime_error("package database is currently locked by another process");
		else
			throw RunTimeErrorWithErrno("could not lock directory " + dirname);
	}
}

Db_lock::~Db_lock()
{
	m_sa.sa_handler = SIG_DFL;
	signal(SIGHUP,SIG_DFL);
	signal(SIGINT,SIG_DFL);
	signal(SIGQUIT,SIG_DFL);
	signal(SIGTERM,SIG_DFL);
	if (m_dir) {
		flock(dirfd(m_dir), LOCK_UN);
		closedir(m_dir);
	}
#ifndef NDEBUG
	std::cerr << "Unlock the database " << m_dir << std::endl;
#endif
}
/*******************     End of Members *********************/

/*******************   Various fonctions ********************/

void assertArgument(char** argv, int argc, int index)
{
	if (argc - 1 < index + 1)
		throw std::runtime_error("option " + std::string(argv[index]) + " requires an argument");
}
void rotatingCursor() {
  static int pos=0;
  char cursor[4]={'/','-','\\','|'};
  fprintf(stderr,"\r [ %c ] ", cursor[pos]);
  fflush(stderr);
  pos = (pos+1) % 4;
}
// vim:set ts=2 :
