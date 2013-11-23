//  pkgdbh.cc
//
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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
#include "string_utils.h"
#include "file_utils.h"
#include "pkgdbh.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <ext/stdio_filebuf.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/param.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <archive.h>
#include <archive_entry.h>

#if ARCHIVE_VERSION_NUMBER >= 3000000
#define INIT_ARCHIVE(ar) \
	archive_read_support_filter_gzip((ar)); \
	archive_read_support_filter_bzip2((ar)); \
	archive_read_support_filter_xz((ar)); \
	archive_read_support_format_tar((ar))
#else
#define INIT_ARCHIVE(ar) \
	archive_read_support_compression_gzip((ar)); \
	archive_read_support_compression_bzip2((ar)); \
	archive_read_support_compression_xz((ar)); \
	archive_read_support_format_tar((ar))
#endif
#define DEFAULT_BYTES_PER_BLOCK (20 * 512)

using __gnu_cxx::stdio_filebuf;

pkgdbh::pkgdbh(const string& name)
	: utilName(name)
{
	// Ignore signals
/*	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sa, 0);
	sigaction(SIGINT, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);
	sigaction(SIGTERM, &sa, 0); */
}
void pkgdbh::treatErrors(const string& s) const
{
	switch ( actualError )
	{
		case CANNOT_DOWNLOAD_FILE:
			throw runtime_error("could not download " + s);
			break;
		case CANNOT_CREATE_FILE:
			throw runTimeErrorWithErrno("could not created  " + s);
			break;
		case CANNOT_OPEN_FILE:
			throw runTimeErrorWithErrno("could not open " + s);
			break;
		case CANNOT_FIND_FILE:
			throw runTimeErrorWithErrno("could not find " + s);
			break;
		case CANNOT_READ_FILE:
			throw runtime_error("could not read " + s);
			break;
		case CANNOT_READ_DIRECTORY:
			throw runTimeErrorWithErrno("could not read directory " + s);
			break;
		case CANNOT_WRITE_FILE:
			throw runTimeErrorWithErrno("could not write file " + s);
			break;
		case CANNOT_SYNCHRONIZE:
			throw runTimeErrorWithErrno("could not synchronize " + s);
			break;
		case CANNOT_RENAME_FILE:
			throw runTimeErrorWithErrno("could not rename " + s);
			break;
		case CANNOT_DETERMINE_NAME_VERSION:
			throw runTimeErrorWithErrno("could not determine name / version " + s);
			break;
		case EMPTY_PACKAGE:
			throw runTimeErrorWithErrno("could not synchronize " + s);
			break;
		case CANNOT_FORK:
			throw runTimeErrorWithErrno("fork() failed " + s);
			break;
		case WAIT_PID_FAILED:
			throw runTimeErrorWithErrno("waitpid() failed " + s);
			break;
		case DATABASE_LOCKED:
			throw runTimeErrorWithErrno("Database  " + s + " locked by another user");
			break;
		case CANNOT_LOCK_DIRECTORY:
			throw runTimeErrorWithErrno("could lock directory " + s);
			break;
		case CANNOT_REMOVE_FILE:
			throw runTimeErrorWithErrno("could not remove file " + s);
			break;
		case CANNOT_RENAME_DIRECTORY:
			throw runTimeErrorWithErrno("could not rename directory  " + s);
			break;
		case OPTION_ONE_ARGUMENT:
			throw runTimeErrorWithErrno(s + " require one argument");
			break;
		case INVALID_OPTION:
			throw runtime_error(s + " invalid option" );
			break;
		case OPTION_MISSING:
			throw runtime_error(s + " option missing");
			break;
		case TOO_MANY_OPTIONS:
			throw runtime_error(s+ ": to many options");
			break;
		case ONLY_ROOT_CAN_CONVERT_DB:
			throw runtime_error(s + "only root can convert the database");
			break;
		case ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
			throw runtime_error(s + " only root can install / upgrade / remove packages");
			break;
		case PACKAGE_ALLREADY_INSTALL:
			throw runtime_error("package " + s + " allready installed (use -u to upgrade)");
			break;
		case PACKAGE_NOT_INSTALL:
			throw runtime_error("package " + s + " not yet installed");
			break;
		case PACKAGE_NOT_PREVIOUSLY_INSTALL:
			throw runtime_error("package " + s + " not previously installed (skip -u to install)");
			break;
		case LISTED_FILES_ALLREADY_INSTALLED:
			throw runtime_error(s + "listed file(s) allready installed (use -f to ignore and overwrite)");
			break;
		case PKGADD_CONFIG_LINE_TOO_LONG:
			throw runTimeErrorWithErrno(s + ": line too long, aborting");
			break;
		case PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS:
			throw runTimeErrorWithErrno(s + ": wrong number of arguments, aborting");
			break;
		case PKGADD_CONFIG_UNKNOWN_ACTION:
			throw runTimeErrorWithErrno(s + "': config unknown action, should be YES or NO, aborting");
			break;
		case PKGADD_CONFIG_UNKNOWN_EVENT:
			throw runTimeErrorWithErrno(s + "' unknown event, aborting");
			break;
		case CANNOT_COMPILE_REGULAR_EXPRESSION:
			throw runtime_error("error compiling regular expression '" + s + "', aborting");
			break;
		case NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE:
			throw runtime_error(s + " is neither an installed package nor a package file");
			break;
	}
}
void pkgdbh::progressInfo() const
{
  static int j = 0;
  int i;
  switch ( actualAction )
  {
		case PKG_DOWNLOAD_START:
			break;
		case PKG_DOWNLOAD_RUN:
			break;
		case PKG_DOWNLOAD_END:
			break;
		case PKG_MOVE_META_START:
			break;
		case PKG_MOVE_META_END:
			break;
    case DB_OPEN_START:
      cout << "Retrieve info about the " << pkgList.size() << " packages: ";
      break;
    case DB_OPEN_RUN:
      if ( pkgList.size()>100)
      {
        i = j / ( pkgList.size() / 100);
        printf("%3d%%\b\b\b\b",i);
      }
      j++;
      break;
    case DB_OPEN_END:
      printf("100 %%\n");
      break;
    case PKG_OPEN_START:
      cout << "Extract the archive: " ;
      break;
    case PKG_OPEN_RUN:
      rotatingCursor();
      break;
    case PKG_OPEN_END:
      printf("100 %%\n");
      break;
    case PKG_INSTALL_START:
      j = 0;
      cout << "Installing "<< filesNumber << " files: ";
      break;
    case PKG_INSTALL_RUN:
      if ( filesNumber > 100)
      {
        i = installedFilesNumber / ( filesNumber  / 100 );
        printf("%3u%%\b\b\b\b",i);
      }
      j++;
      break;
    case PKG_INSTALL_END:
      printf("100 %%\n");
      break;
		case DB_ADD_PKG_START:
			break;
		case DB_ADD_PKG_END:
			break;
		case RM_PKG_FILES_START:
			j=0;
			cout << "Removing " << FilesList.size() << " files: ";
			break;
		case RM_PKG_FILES_RUN:
			if ( FilesList.size()>100)
			{
				i = j / ( FilesList.size() / 100);
				printf("%3d%%\b\b\b\b",i);
			}
			j++;
			break;
		case RM_PKG_FILES_END:
			printf("100 %%\n");
			break;
		case LDCONFIG_START:
			break;
		case LDCONFIG_END:
			break;
  }
}
/* Append to the "DB" the number of packages founds (directory containg a file named files */
int pkgdbh::getListOfPackages (const string& path)
{
	keyValue string_splited;
	root = trim_filename(path + "/");
	const string pathdb =  root + PKG_DB_DIR;
	pkgFoldersList = findFile(pathdb);
	for (set<string>::iterator i = pkgFoldersList.begin();i != pkgFoldersList.end();++i) {
		string_splited=split_keyValue(*i,"#");
		if (string_splited.value.size()>0)
			pkgList.insert(string_splited.parameter + " " + string_splited.value);
	}

#ifndef NDEBUG
  cerr << "Number of Packages: " << pkgList.size() << endl;
#endif
  return pkgList.size();
}
/* get details infos of a package */
pair<string, pkginfo_t> pkgdbh::getInfosPackage(const string& packageName)
{
	pair<string, pkginfo_t> result;
	string name(packageName,0,packageName.find('#'));
	string version = packageName;
	result.second.version=version.erase(0, version.find('#') == string::npos ? string::npos : version.find('#') + 1);
	result.first = name;
	string package_foldername = name + "#" + version;
	result.second.description =  getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"des");
	result.second.url = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"url");
	result.second.maintainer = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"mai");
	result.second.packager = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"pac");
	result.second.run = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"run");
	result.second.size = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"size_i");
	return result;
}
/* Populate the database with all details infos */
void pkgdbh::getInstalledPackages(bool silent)
{
	if (!silent)
	{
		actualAction = DB_OPEN_START;
		progressInfo();
	}
	for (set<string>::iterator i = pkgList.begin();i != pkgList.end();++i) {
		if (!silent)
		{
			actualAction = DB_OPEN_RUN;
			if ( pkgList.size() > 100 )
				progressInfo();
		}
		pkginfo_t info;
		string name(*i,0, i->find(NAME_DELIM));
		string version = *i;
		version.erase(0, version.find(NAME_DELIM) == string::npos ? string::npos : version.find(NAME_DELIM) + 1);
		string arch = version;
		version.erase(version.find_last_of("-"),version.size());
		arch.erase(0, arch.find_last_of("-")+1);
		info.version = version;
		info.arch = arch;
		string package_foldername = name + "#" + version + "-" + arch;
		info.description = 	getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"des");
		info.url = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"url");
		info.maintainer = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"mai");
		info.packager = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"pac");
		info.run = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"run");
		info.size = getValueOfKey(root + PKG_DB_DIR + package_foldername +PKG_META,PARAM_DELIM,"size_i");
		// list of files
		const string filelist = root + PKG_DB_DIR + package_foldername + PKG_FILES;
		int fd = open(filelist.c_str(), O_RDONLY);
		if (fd == -1)
		{
			actualError = CANNOT_OPEN_FILE;
			treatErrors(filelist);
		}
		stdio_filebuf<char> listbuf(fd, ios::in, getpagesize());
		istream in(&listbuf);
		if (!in)
			throw runTimeErrorWithErrno("could not read " + filelist);

		while (!in.eof()){
			// read alls the files for alls the packages founds
			for (;;) {
				string file;
				getline(in, file);
				if (file.empty())
					break; // End of record
				info.files.insert(info.files.end(), file);
			}
			if (!info.files.empty())
				listOfInstPackages[name] = info;
		}
	}
#ifndef NDEBUG
  cerr << endl;
  cerr << listOfInstPackages.size() << " packages found in database " << endl;
#endif
	if (!silent)
	{
		actualAction = DB_OPEN_END;
		progressInfo();
	}
}
/* Populate the list depListOfPackages with the found dependencies from the list of packages name */
void pkgdbh::getDependenciesList(const set<string>& listOfPackagesName)
{
//	int depReturnVal=-1;
	pair<string, pkginfo_t> pkg;

	for (set<string>::iterator i = listOfPackagesName.begin();i != listOfPackagesName.end();++i)
	{
		if (checkPackageNameExist(*i))
		{
			/* we need to find the name#version folder */
			for (set<string>::const_iterator k = pkgFoldersList.begin(); k != pkgFoldersList.end(); ++k)
			{
				string name = *i + "#";
				string matchS = k->substr(0,name.size());
				size_t found = matchS.find(name);
				if ( found != string::npos)
				{
					pkg = getInfosPackage(*k);
					if (getDependencies(listOfInstPackages,depListOfPackages,pkg) == 0)
					{
						depListOfPackages.insert(pkg);
					}
					break;
				}
			}
		}
	}
}
int pkgdbh::getDependencies(const packages_t& list_of_availables_packages, packages_t& dep, const pair<string, pkginfo_t>& pkg)
{
	set<string> dDepsList;
	/* if the package pkg doen't have dependencies (anymore) we can return normaly */
	if ( strcmp(pkg.second.run.c_str(),"") == 0 || strcmp(pkg.second.run.c_str()," ") == 0 )
	{
		return 0;
	}
	/* if the list of dependencies Packages depsPkgList not yet exist as this function can be call recursivly, create it */
	dDepsList=parseDelimitedList(pkg.second.run,',');
	if (dDepsList.size() > 0)
	{
		for (set<string>::const_iterator i = dDepsList.begin();
				i != dDepsList.end(); ++i)
		{
			/* if exist and not yet in  the dependencies list */
			if (checkPackageNameExist(*i) && !(depListOfPackages.find(*i) != depListOfPackages.end()))
			{
				/* we need to find the name#version folder */
				for (set<string>::const_iterator k = pkgFoldersList.begin(); k != pkgFoldersList.end(); ++k)
				{
					string name = *i + "#";
					string matchS = k->substr(0,name.size());
					size_t found = matchS.find(name);
					if ( found != string::npos)
					{
						pair<string, pkginfo_t> tmpPkg = getInfosPackage(*k);
						dep.insert(tmpPkg);
						int depReturnVal = getDependencies(list_of_availables_packages, dep, tmpPkg);
						if  (depReturnVal != -1)
							break;
						break;
					}
				}
			}

		}
	} else cout << "List empty" << endl;
	return 0;
}
void pkgdbh::convertSpaceToNoSpaceDBFormat(const string& path)
{
	root = trim_filename(path + "/");
	/* Convert from directories with spaces to directories without spaces */
	const string packagedir = root + PKG_DB_DIR ;
	pkgList = findFile(packagedir);
	for (set<string>::iterator i = pkgList.begin();i != pkgList.end();++i) {

		if ( ! (i->find(NAME_DELIM) == string::npos))
		{
			string name(*i,0, i->find(NAME_DELIM));
			string version = *i;
			version.erase(0, version.find(NAME_DELIM) == string::npos ? string::npos : version.find(NAME_DELIM) + 1);
			const string packagenamedir_with_space = root + PKG_DB_DIR + name + " " + version;
			const string packagenamedir_without_space = root + PKG_DB_DIR + name + "#" + version;
			if (rename( packagenamedir_with_space.c_str(), packagenamedir_without_space.c_str() ) == -1 )
			{
				actualError = CANNOT_RENAME_FILE;
				treatErrors(packagenamedir_with_space + " to " + packagenamedir_without_space);
			}
			cout << packagenamedir_with_space << " renamed in " << packagenamedir_without_space << endl;
		}
	}
}
void pkgdbh::convertDBFormat()
{
	/* Convert from single db file to multi directories */

	const string packagedir = root + PKG_DB_DIR ;
	mkdir(packagedir.c_str(),0755);
	cout << packagedir << endl;
	for (packages_t::const_iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i) {

		const string packagenamedir = root + PKG_DB_DIR + i->first + "#" + i-> second.version;
		cout << packagenamedir << " created" << endl;
		mkdir(packagenamedir.c_str(),0755);
		const string fileslist = packagenamedir + PKG_FILES;
		const string fileslist_new = fileslist + ".imcomplete_transaction";

		int fd_new = creat(fileslist_new.c_str(),0444);

		if (fd_new == -1)
		{
			actualError = CANNOT_CREATE_FILE;
			treatErrors(fileslist_new);
		}

		stdio_filebuf<char> filebuf_new(fd_new, ios::out, getpagesize());

		ostream db_new(&filebuf_new);
		copy(i->second.files.begin(), i->second.files.end(), ostream_iterator<string>(db_new, "\n"));

		db_new.flush();
		if (!db_new)
		{
			actualError = CANNOT_WRITE_FILE;
			treatErrors(fileslist_new);
		}
		// Synchronize file to disk
		if (fsync(fd_new) == -1)
		{
			actualError = CANNOT_SYNCHRONIZE;
			treatErrors(fileslist_new);
		}

		// Move new database into place
		if (rename(fileslist_new.c_str(), fileslist.c_str()) == -1)
		{
			actualError = CANNOT_RENAME_FILE;
			treatErrors(fileslist_new + " to " + fileslist);
		}
	}
#ifndef NDEBUG
  cerr << listOfInstPackages.size() << " packages written to database" << endl;
#endif
}
void pkgdbh::moveMetaFilesPackage(const string& name, pkginfo_t& info)
{
	actualAction = PKG_MOVE_META_START;
	progressInfo();
	string package_meta_dir = ".";
	for (set<string>::const_iterator i = info.files.begin(); i != info.files.end(); ++i)
	{
		if ( strncmp(i->c_str(),package_meta_dir.c_str(),package_meta_dir.size()) == 0 )
		{
#ifndef NDEBUG
			cout << "*I: " << *i << endl;
#endif
			metaFilesList.insert(metaFilesList.end(), *i );
			info.files.erase(i);
		}
	}
	const string packagedir = root + PKG_DB_DIR ;
	const string packagenamedir = root + PKG_DB_DIR + name + "#" + info.version + "-" + info.arch;

	mkdir(packagenamedir.c_str(),0755);
	for (set<string>::const_iterator i = metaFilesList.begin(); i!=  metaFilesList.end(); ++i)
	{
		string file = packagenamedir + "/" + *i;
		if (rename(i->c_str(), file.c_str()) == -1)
		{
			actualError = CANNOT_RENAME_FILE;
			treatErrors( *i + " to " + file);
		}
	}
	actualAction = PKG_MOVE_META_END;
	progressInfo();
}
void pkgdbh::addPackageFilesRefsToDB(const string& name, const pkginfo_t& info)
{

	listOfInstPackages[name] = info;
	const string packagedir = root + PKG_DB_DIR ;
	const string packagenamedir = root + PKG_DB_DIR + name + "#" + info.version + "-" + info.arch;
	mkdir(packagenamedir.c_str(),0755);
	const string fileslist = packagenamedir + PKG_FILES;
	const string fileslist_new = fileslist + ".imcomplete_transaction";
	int fd_new = creat(fileslist_new.c_str(),0644);
	if (fd_new == -1)
	{
		actualError = CANNOT_CREATE_FILE;
		treatErrors(fileslist_new);
	}

	stdio_filebuf<char> filebuf_new(fd_new, ios::out, getpagesize());

	ostream db_new(&filebuf_new);
	copy(info.files.begin(), info.files.end(), ostream_iterator<string>(db_new, "\n"));

	db_new.flush();
	if (!db_new)
	{
		rmdir(packagenamedir.c_str());
		actualError = CANNOT_WRITE_FILE;
		treatErrors(fileslist_new);
	}
	// Synchronize file to disk
	if (fsync(fd_new) == -1)
	{
		rmdir(packagenamedir.c_str());
		actualError = CANNOT_SYNCHRONIZE;
		treatErrors(fileslist_new);
	}
	// Move new database into place
	if (rename(fileslist_new.c_str(), fileslist.c_str()) == -1)
	{
		rmdir(packagenamedir.c_str());
		actualError = CANNOT_RENAME_FILE;
		treatErrors(fileslist_new + " to " + fileslist);
	}
}

bool pkgdbh::checkPackageNameExist(const string& name)
{
	return (listOfInstPackages.find(name) != listOfInstPackages.end());
}

/* Remove meta data about the removed package */
void pkgdbh::removePackageFilesRefsFromDB(const string& name)
{
	const string packagedir = root + PKG_DB_DIR ;
	const string version = listOfInstPackages[name].version;
	const string arch = listOfInstPackages[name].arch;
  	const string packagenamedir = root + PKG_DB_DIR + name + "#" + version + "-" + arch;

	metaFilesList = findFile( packagenamedir);
	if (metaFilesList.size() > 0)
		for (set<string>::iterator i = metaFilesList.begin(); i != metaFilesList.end();++i) {
			const string filename = packagenamedir + "/" + *i;
			if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
				const char* msg = strerror(errno);
					cerr << utilName << ": could not remove " << filename << ": " << msg << endl;
				}
#ifndef NDEBUG
				cout  << "File: " << filename << " is removed"<< endl;
#endif
			}
		if( remove(packagenamedir.c_str()) == -1)
		{
			const char* msg = strerror(errno);
			cerr << utilName << ": could not remove " << packagenamedir << ": " << msg << endl;
		}
#ifndef NDEBUG
	cout  << "Directory: " << packagenamedir << " is removed"<< endl;
#endif
	metaFilesList.clear();
}

/* Remove the physical files after followings some rules */
void pkgdbh::removePackageFiles(const string& name)
{
	FilesList = listOfInstPackages[name].files;
	listOfInstPackages.erase(name);

#ifndef NDEBUG
	cerr << "Removing package phase 1 (all files in package):" << endl;
	copy(FilesList.begin(), FilesList.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files that still have references
	for (packages_t::const_iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i)
		for (set<string>::const_iterator j = i->second.files.begin(); j != i->second.files.end(); ++j)
			FilesList.erase(*j);

#ifndef NDEBUG
	cerr << "Removing package phase 2 (files that still have references excluded):" << endl;
	copy(FilesList.begin(), FilesList.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	actualAction = RM_PKG_FILES_START;
	progressInfo();
	// Delete the files from bottom to up to make shure we delete the contents of any folder before
	for (set<string>::const_reverse_iterator i = FilesList.rbegin(); i != FilesList.rend(); ++i) {
		actualAction = RM_PKG_FILES_RUN;
		progressInfo();
		const string filename = root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			const char* msg = strerror(errno);
			cerr << utilName << ": could not remove " << filename << ": " << msg << endl;
		}
	}
	actualAction = RM_PKG_FILES_END;
	progressInfo();
}

void pkgdbh::removePackageFiles(const string& name, const set<string>& keep_list)
{
	FilesList = listOfInstPackages[name].files;
	listOfInstPackages.erase(name);

#ifndef NDEBUG
	cerr << "Removing package phase 1 (all files in package):" << endl;
	copy(FilesList.begin(), FilesList.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files found in the keep list
	for (set<string>::const_iterator i = keep_list.begin(); i != keep_list.end(); ++i)
		FilesList.erase(*i);

#ifndef NDEBUG
	cerr << "Removing package phase 2 (files that is in the keep list excluded):" << endl;
	copy(FilesList.begin(), FilesList.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files that still have references
	for (packages_t::const_iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i)
		for (set<string>::const_iterator j = i->second.files.begin(); j != i->second.files.end(); ++j)
			FilesList.erase(*j);

#ifndef NDEBUG
	cerr << "Removing package phase 3 (files that still have references excluded):" << endl;
	copy(FilesList.begin(), FilesList.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Delete the files
	actualAction = RM_PKG_FILES_START;
	progressInfo();
	for (set<string>::const_reverse_iterator i = FilesList.rbegin(); i != FilesList.rend(); ++i) {
		actualAction = RM_PKG_FILES_RUN;
		progressInfo();
		const string filename = root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			cerr << utilName << ": could not remove " << filename << ": " << msg << endl;
		}
	}
	actualAction = RM_PKG_FILES_END;
	progressInfo();
}

void pkgdbh::removePackageFilesRefsFromDB(set<string> files, const set<string>& keep_list)
{
	// Remove all references
	for (packages_t::iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i)
		for (set<string>::const_iterator j = files.begin(); j != files.end(); ++j)
			i->second.files.erase(*j);

#ifndef NDEBUG
	cerr << "Removing files:" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files found in the keep list
	for (set<string>::const_iterator i = keep_list.begin(); i != keep_list.end(); ++i)
		files.erase(*i);

	// Delete the files
	for (set<string>::const_reverse_iterator i = files.rbegin(); i != files.rend(); ++i) {
		const string filename = root + *i;
		if (checkFileExist(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			cerr << utilName << ": could not remove " << filename << ": " << msg << endl;
		}
	}
}

set<string> pkgdbh::getConflictsFilesList(const string& name, const pkginfo_t& info)
{
	set<string> files;

	// Find conflicting files in database
	for (packages_t::const_iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i) {
		if (i->first != name) {
			set_intersection(info.files.begin(), info.files.end(),
					 i->second.files.begin(), i->second.files.end(),
					 inserter(files, files.end()));
		}
	}

#ifndef NDEBUG
	cerr << "Conflicts phase 1 (conflicts in database):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Find conflicting files in filesystem
	for (set<string>::iterator i = info.files.begin(); i != info.files.end(); ++i) {
		const string filename = root + *i;
		if (checkFileExist(filename) && files.find(*i) == files.end())
			files.insert(files.end(), *i);
	}

#ifndef NDEBUG
	cerr << "Conflicts phase 2 (conflicts in filesystem added):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Exclude directories
	set<string> tmp = files;
	for (set<string>::const_iterator i = tmp.begin(); i != tmp.end(); ++i) {
		if ((*i)[i->length() - 1] == '/')
			files.erase(*i);
	}

#ifndef NDEBUG
	cerr << "Conflicts phase 3 (directories excluded):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// If this is an upgrade, remove files already owned by this package
	if (listOfInstPackages.find(name) != listOfInstPackages.end()) {
		for (set<string>::const_iterator i = listOfInstPackages[name].files.begin(); i != listOfInstPackages[name].files.end(); ++i)
			files.erase(*i);

#ifndef NDEBUG
		cerr << "Conflicts phase 4 (files already owned by this package excluded):" << endl;
		copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
		cerr << endl;
#endif
	}

	return files;
}

pair<string, pkginfo_t> pkgdbh::openArchivePackage(const string& filename)
{
	pair<string, pkginfo_t> result;
	struct archive* archive;
	struct archive_entry* entry;

	// Extract name, version and the arch from filename
	string basename(filename, filename.rfind('/') + 1);
	string name(basename, 0, basename.find(VERSION_DELIM));
	string version(basename, 0, basename.rfind(PKG_EXT));
  version.erase(0, version.find(VERSION_DELIM) == string::npos ? string::npos : version.find(VERSION_DELIM) + 1);
	string arch = version;
  version.erase(version.find_last_of("-"),version.size());
  arch.erase(0, arch.find_last_of("-")+1);

	if (name.empty() || version.empty())
	{
		actualError = CANNOT_DETERMINE_NAME_VERSION;
		treatErrors(basename);
	}

	result.first = name;
	result.second.version = version;
	result.second.arch = arch;

	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
		{
			actualError = CANNOT_OPEN_FILE;
			treatErrors(filename);
			//throw runTimeErrorWithErrno("could not open " + filename, archive_errno(archive));
		}
	actualAction = PKG_OPEN_START;
	progressInfo();
	for (filesNumber = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++filesNumber) {
			actualAction = PKG_OPEN_RUN;
			progressInfo();

				result.second.files.insert(result.second.files.end(),
					archive_entry_pathname(entry));

		mode_t mode = archive_entry_mode(entry);

		if (S_ISREG(mode) &&
		    archive_read_data_skip(archive) != ARCHIVE_OK)
			{
				actualError = CANNOT_READ_FILE;
				treatErrors(filename);
			// throw runTimeErrorWithErrno("could not read " + filename, archive_errno(archive));
			}
	}

	if (filesNumber == 0) {
		if (archive_errno(archive) == 0)
		{
			actualError = EMPTY_PACKAGE;
			treatErrors(filename);
		}
		else
		{
			actualError = CANNOT_READ_FILE;
			treatErrors(filename);
		}
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
	archive_read_finish(archive);
#endif
	// Retrieve number of files from the archive is complete
	actualAction = PKG_OPEN_END;
	progressInfo();
	return result;
}
void pkgdbh::extractAndRunPREfromPackage(const string& filename)
{
	struct archive* archive;
	struct archive_entry* entry;
	archive = archive_read_new();
	INIT_ARCHIVE(archive);
	if (archive_read_open_filename(archive,
		filename.c_str(),
		DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
	{
		actualError = CANNOT_OPEN_FILE;
		treatErrors(filename);
	}

	chdir(root.c_str());

	for (installedFilesNumber = 0; archive_read_next_header(archive, &entry) ==
		ARCHIVE_OK; ++installedFilesNumber)
	{
		string archive_filename = archive_entry_pathname(entry);
		if ( strcmp(archive_filename.c_str(),PKG_PRE_INSTALL) == 0)
		{
			unsigned int flags = ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK;
			if (archive_read_extract(archive, entry, flags) != ARCHIVE_OK)
			{
				const char* msg = archive_error_string(archive);
				cerr << utilName << ": could not install " + archive_filename << " : " << msg << endl;
				exit(EXIT_FAILURE);
			}
			break;

		}

	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
        archive_read_free(archive);
#else
        archive_read_finish(archive);
#endif
	chdir("-");
	if (checkFileExist(root + PKG_PRE_INSTALL))
	{
#ifndef NDEBUG
  cerr << root + PKG_PRE_INSTALL + " exist " << endl;
#endif


		pid_t pid = fork();
		if (pid == -1)
		{
			actualError = CANNOT_FORK;
			treatErrors("");
		}
		string shell_path = root + "bin/sh";
		if (pid == 0)
		{
#ifndef NDEBUG
			cerr << shell_path << " " << PKG_PRE_INSTALL <<	endl;
#endif
			execl(shell_path.c_str(), shell_path.c_str(), PKG_PRE_INSTALL, (char *) 0);
			const char* msg = strerror(errno);
			cerr << utilName << ": could not execute " << PKG_PRE_INSTALL << " : " << msg << endl;
			exit(EXIT_FAILURE);
		}
		else
		{
			if (waitpid(pid, 0, 0) == -1)
			{
				actualError = WAIT_PID_FAILED;
				treatErrors("");
			}
		}
		removeFile(root,PKG_PRE_INSTALL);
	}
}
void pkgdbh::installArchivePackage(const string& filename, const set<string>& keep_list, const set<string>& non_install_list)
{
	struct archive* archive;
	struct archive_entry* entry;
	char buf[PATH_MAX];
	string absroot;

	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
		{
			actualError = CANNOT_OPEN_FILE;
			treatErrors(filename);
		}
	chdir(root.c_str());
	absroot = getcwd(buf, sizeof(buf));
	actualAction = PKG_INSTALL_START;
	progressInfo();
	for (installedFilesNumber = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++installedFilesNumber) {

		actualAction = PKG_INSTALL_RUN;
		progressInfo();
		string archive_filename = archive_entry_pathname(entry);
		string reject_dir = trim_filename(absroot + string("/") + string(PKG_REJECTED));
		string original_filename = trim_filename(absroot + string("/") + archive_filename);
		string real_filename = original_filename;

		// Check if file is filtered out via INSTALL
		if (non_install_list.find(archive_filename) != non_install_list.end()) {
			mode_t mode;

			cout << utilName << ": ignoring " << archive_filename << endl;

			mode = archive_entry_mode(entry);

			if (S_ISREG(mode))
				archive_read_data_skip(archive);

			continue;
		}

		// Check if file should be rejected
		if (checkFileExist(real_filename) && keep_list.find(archive_filename) != keep_list.end())
			real_filename = trim_filename(reject_dir + string("/") + archive_filename);

		archive_entry_set_pathname(entry, const_cast<char*>
		                           (real_filename.c_str()));

		// Extract file
		unsigned int flags = ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK;
		if (archive_read_extract(archive, entry, flags) != ARCHIVE_OK) {
			// If a file fails to install we just print an error message and
			// continue trying to install the rest of the package.
			const char* msg = archive_error_string(archive);
			cerr << utilName << ": could not install " + archive_filename << ": " << msg << endl;
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
				cout << utilName << ": rejecting " << archive_filename << ", keeping existing version" << endl;
		}
	}
	actualAction = PKG_INSTALL_END;
	progressInfo();
	if (installedFilesNumber == 0) {
		if (archive_errno(archive) == 0)
		{
			actualError = EMPTY_PACKAGE;
			treatErrors(filename);
		}
		else
		{
			actualError = CANNOT_READ_FILE;
			treatErrors(filename);
		}
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
	archive_read_finish(archive);
#endif
}

void pkgdbh::runLdConfig()
{
	// Only execute runLdConfig if /etc/ld.so.conf exists
	if (checkFileExist(root + LDCONFIG_CONF)) {
		pid_t pid = fork();

		if (pid == -1)
		{
			actualError = CANNOT_FORK;
			treatErrors("");
		}

		if (pid == 0) {
			execl(LDCONFIG, LDCONFIG, "-r", root.c_str(), (char *) 0);
			const char* msg = strerror(errno);
			cerr << utilName << ": could not execute " << LDCONFIG << ": " << msg << endl;
			exit(EXIT_FAILURE);
		} else {
			if (waitpid(pid, 0, 0) == -1)
			{
				actualError = WAIT_PID_FAILED;
				treatErrors("");
			}
		}
	}
}

void pkgdbh::getFootprintPackage(string& filename)
{
	unsigned int i;
	struct archive* archive;
	struct archive_entry* entry;

	map<string, mode_t> hardlink_target_modes;

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
				actualError = CANNOT_OPEN_FILE;
				treatErrors(filename);
			}
      //         throw runTimeErrorWithErrno("could not open " + filename, archive_errno(archive));

	for (i = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++i) {

		mode_t mode = archive_entry_mode(entry);

		if (!archive_entry_hardlink(entry)) {
			const char *s = archive_entry_pathname(entry);

			hardlink_target_modes[s] = mode;
		}

		if (S_ISREG(mode) && archive_read_data_skip(archive))
		{
			actualError = CANNOT_READ_FILE;
			treatErrors(filename);
		//	throw runTimeErrorWithErrno("could not read " + filename, archive_errno(archive));
		}
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
  archive_read_finish(archive);
#endif

	// Too bad, there doesn't seem to be a way to reuse our archive
	// instance
	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
			{
				actualError = CANNOT_OPEN_FILE;
				treatErrors(filename);
        // throw runTimeErrorWithErrno("could not open " + filename, archive_errno(archive));
			}
	for (i = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++i) {
		mode_t mode = archive_entry_mode(entry);

		// Access permissions
		if (S_ISLNK(mode)) {
			// Access permissions on symlinks differ among filesystems, e.g. XFS and ext2 have different.
			// To avoid getting different footprints we always use "lrwxrwxrwx".
			cout << "lrwxrwxrwx";
		} else {
			const char *h = archive_entry_hardlink(entry);

			if (h)
				cout << mtos(hardlink_target_modes[h]);
			else
				cout << mtos(mode);
		}

		cout << '\t';

		// User
		uid_t uid = archive_entry_uid(entry);
		struct passwd* pw = getpwuid(uid);
		if (pw)
			cout << pw->pw_name;
		else
			cout << uid;

		cout << '/';

		// Group
		gid_t gid = archive_entry_gid(entry);
		struct group* gr = getgrgid(gid);
		if (gr)
			cout << gr->gr_name;
		else
			cout << gid;

		// Filename
		cout << '\t' << archive_entry_pathname(entry);

		// Special cases
		if (S_ISLNK(mode)) {
			// Symlink
			cout << " -> " << archive_entry_symlink(entry);
		} else if (S_ISCHR(mode) ||
		           S_ISBLK(mode)) {
			// Device
			cout << " (" << archive_entry_rdevmajor(entry)
			     << ", " << archive_entry_rdevminor(entry)
			     << ")";
		} else if (S_ISREG(mode) &&
		           archive_entry_size(entry) == 0) {
			// Empty regular file
			cout << " (EMPTY)";
		}

		cout << '\n';

		if (S_ISREG(mode) && archive_read_data_skip(archive))
		{
			actualError = CANNOT_READ_FILE;
			treatErrors(filename);
		//	throw runTimeErrorWithErrno("could not read " + filename, archive_errno(archive));
		}
	}

	if (i == 0) {
		if (archive_errno(archive) == 0)
		{
			actualError = EMPTY_PACKAGE;
			treatErrors(filename);
		}
		else
		{
			actualError = CANNOT_READ_FILE;
			treatErrors(filename);
		}
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
	archive_read_finish(archive);
#endif

}

void pkgdbh::print_version() const
{
	cout << utilName << " (cards) " << VERSION << endl;
}

db_lock::db_lock(const string& root, bool exclusive)
	: dir(0)
{
	const string dirname = trim_filename(root + string("/") + PKG_DB_DIR_OLD);

	if (!(dir = opendir(dirname.c_str())))
		throw runTimeErrorWithErrno("could not read directory " + dirname);

	if (flock(dirfd(dir), (exclusive ? LOCK_EX : LOCK_SH) | LOCK_NB) == -1) {
		if (errno == EWOULDBLOCK)
			throw runtime_error("package database is currently locked by another process");
		else
			throw runTimeErrorWithErrno("could not lock directory " + dirname);
	}
}

db_lock::~db_lock()
{
	if (dir) {
		flock(dirfd(dir), LOCK_UN);
		closedir(dir);
	}
}

/*void pkgdbh::db_commit()
{
  const string dbfilename = root + PKG_DB_OLD;
  const string dbfilename_new = dbfilename + ".incomplete_transaction";
  const string dbfilename_bak = dbfilename + ".backup";

  // Remove failed transaction (if it exists)
  if (unlink(dbfilename_new.c_str()) == -1 && errno != ENOENT)
	{
		actualError = CANNOT_REMOVE_FILE;
    treatErrors(dbfilename_new);
	}
  // Write new database
  int fd_new = creat(dbfilename_new.c_str(), 0444);
  if (fd_new == -1)
	{
		actualError = CANNOT_CREATE_FILE;
		treatErrors(dbfilename_new);
	}
  stdio_filebuf<char> filebuf_new(fd_new, ios::out, getpagesize());

  ostream db_new(&filebuf_new);
  for (packages_t::const_iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i) {
    if (!i->second.files.empty()) {
      db_new << i->first << "\n";
      db_new << i->second.version << "\n";
      copy(i->second.files.begin(), i->second.files.end(), ostream_iterator<string>(db_new, "\n"));
      db_new << "\n";
    }
  }

  db_new.flush();

  // Make sure the new database was successfully written
  if (!db_new)
	{
		actualError = CANNOT_WRITE_FILE;
		treatErrors(dbfilename_new);
  }
	// Synchronize file to disk
  if (fsync(fd_new) == -1)
	{
		actualError = CANNOT_SYNCHRONIZE;
		treatErrors(dbfilename_new);
	}
  // Relink database backup
  if (unlink(dbfilename_bak.c_str()) == -1 && errno != ENOENT)
	{
		actualError = CANNOT_REMOVE_FILE;
		treatErrors(dbfilename_bak);
	}
  if (link(dbfilename.c_str(), dbfilename_bak.c_str()) == -1)
	{
		actualError = CANNOT_CREATE_FILE;
		treatErrors(dbfilename_bak);
	}

  // Move new database into place
  if (rename(dbfilename_new.c_str(), dbfilename.c_str()) == -1)
	{
		actualError = CANNOT_RENAME_FILE;
		treatErrors(dbfilename_new + " to " + dbfilename);
	}

#ifndef NDEBUG
  cerr << listOfInstPackages.size() << " packages written to database" << endl;
#endif
}
*/
void pkgdbh::db_open(const string& path)
{
  // Read database from single file
  // Only need to convert from pkgutils to cards

  root = trim_filename(path + "/");
  const string filename = root + PKG_DB_OLD;

  int fd = open(filename.c_str(), O_RDONLY);
  if (fd == -1)
	{
		actualError = CANNOT_OPEN_FILE;
    treatErrors(filename);
	}
  stdio_filebuf<char> filebuf(fd, ios::in, getpagesize());
  istream in(&filebuf);
  if (!in)
	{
		actualError = CANNOT_READ_FILE;
		treatErrors(filename);
	}
  while (!in.eof()) {
    // Read record
    string name;
    pkginfo_t info;
    getline(in, name);
    getline(in, info.version);
    for (;;) {
      string file;
      getline(in, file);

      if (file.empty())
        break; // End of record

      info.files.insert(info.files.end(), file);
    }
   if (!info.files.empty())
        listOfInstPackages[name] = info;
  }

#ifndef NDEBUG
  cerr << endl;
  cerr << listOfInstPackages.size() << " packages found in database " << endl;
#endif
}
/*******************     End of Members *********************/

/*******************   Various fonctions ********************/

void assertArgument(char** argv, int argc, int index)
{
	if (argc - 1 < index + 1)
		throw runtime_error("option " + string(argv[index]) + " requires an argument");
}
void rotatingCursor() {
  static int pos=0;
  char cursor[4]={'/','-','\\','|'};
  printf("%c\b", cursor[pos]);
  fflush(stdout);
  pos = (pos+1) % 4;
}
// vim:set ts=2 :
