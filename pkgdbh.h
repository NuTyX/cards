//  pkgdbh.h
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

#ifndef PKGDBH_H
#define PKGDBH_H

#include <string>
#include <set>
#include <map>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>

#define PKG_EXT          ".pkg.tar."
#define PKG_DB_DIR_OLD      "var/lib/pkg/"
#define PKG_DB_DIR          "var/lib/pkg/DB/"
#define PKG_DB_OLD           "var/lib/pkg/db"
#define PKG_FILES        "/files"
#define PKG_META         "/Pkgmeta"
#define PKG_INSTALL_DIR  "tmp/"
#define PKG_RECEPT       "Pkgfile"
#define PKG_README       "README"
#define PKG_PRE_INSTALL  "pre-install"
#define PKG_POST_INSTALL "post-install"    

#define PKG_REJECTED     "var/lib/pkg/rejected"
#define VERSION_DELIM    '#'
#define GROUP_DELIM      '#'
#define NAME_DELIM			 ' '
#define PARAM_DELIM      "="

#define LDCONFIG         "/sbin/ldconfig"
#define LDCONFIG_CONF    "/etc/ld.so.conf"

using namespace std;
enum error
{
CANNOT_DOWNLOAD_FILE,
CANNOT_CREATE_FILE,
CANNOT_OPEN_FILE,
CANNOT_FIND_FILE,
CANNOT_READ_FILE,
CANNOT_READ_DIRECTORY,
CANNOT_WRITE_FILE,
CANNOT_SYNCHRONIZE,
CANNOT_RENAME_FILE,
CANNOT_DETERMINE_NAME_VERSION,
EMPTY_PACKAGE,
CANNOT_FORK,
WAIT_PID_FAILED,
DATABASE_LOCKED,
CANNOT_LOCK_DIRECTORY,
CANNOT_REMOVE_FILE,
CANNOT_RENAME_DIRECTORY,
OPTION_ONE_ARGUMENT,
INVALID_OPTION,
OPTION_MISSING,
TOO_MANY_OPTIONS,
ONLY_ROOT_CAN_CONVERT_DB,
ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE,
PACKAGE_ALLREADY_INSTALL,
PACKAGE_NOT_INSTALL,
PACKAGE_NOT_PREVIOUSLY_INSTALL,
LISTED_FILES_ALLREADY_INSTALLED,
PKGADD_CONFIG_LINE_TOO_LONG,
PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS,
PKGADD_CONFIG_UNKNOWN_ACTION,
PKGADD_CONFIG_UNKNOWN_EVENT,
CANNOT_COMPILE_REGULAR_EXPRESSION,
NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE
};


enum action 
{ 
PKG_DOWNLOAD_START,
PKG_DOWNLOAD_RUN,
PKG_DOWNLOAD_END,
DB_OPEN_START,
DB_OPEN_RUN,
DB_OPEN_END,
PKG_OPEN_START,
PKG_OPEN_RUN,
PKG_OPEN_END,
PKG_INSTALL_START,
PKG_INSTALL_END,
PKG_INSTALL_RUN,
PKG_MOVE_META_START,
PKG_MOVE_META_END,
DB_ADD_PKG_START,
DB_ADD_PKG_END,
LDCONFIG_START,
LDCONFIG_END,
RM_PKG_FILES_START,
RM_PKG_FILES_RUN,
RM_PKG_FILES_END
};

struct pkginfo_t {
    string version;
    string arch;
    string description;
    string url;
    string packager;
    string maintainer;
    string depends;
    string run;
    string size;
    set<string> files;
  };
typedef map<string, pkginfo_t> packages_t;

class pkgdbh {
public:
	
	explicit pkgdbh(const string& name);
	virtual ~pkgdbh() {}
	virtual void run(int argc, char** argv) = 0; // Need to be redefine in derivated class
	virtual void printHelp() const = 0; // help info is depending of the derivated class
	virtual void progressInfo() const; // progressInfo info
	virtual void treatErrors(const string& s) const; 
	void print_version() const;

protected:
	// Database

	void convertDBFormat();
	int getListOfPackages(const string& path );
	pair<string, pkginfo_t> getInfosPackage(const string& packageName);
	void getInstalledPackages(bool silent);
	void getInstalledPackages(const string& path);

	/* Add the found dependencies from the list of packages name */
	void getDependenciesList(const set<string>& listOfPackagesName);
	int getDependencies(const packages_t& list_of_availables_packages, packages_t& dep, const pair<string, pkginfo_t>& pkg);

	void addPackageFilesRefsToDB(const string& name, const pkginfo_t& info);
	void addPackageFilesRefsToDB_2(const string& name, const pkginfo_t& info);
	bool checkPackageNameExist(const string& name);
	bool checkPackageNameExist_2(const string& name);	
	/* Remove the physical files after followings some rules */
	void removePackageFiles(const string& name);
	
	void removePackageFilesRefsFromDB(const string& name, const set<string>& keep_list);

	/* Remove meta data about the removed package */
	void removePackageFilesRefsFromDB(const string& name);
	void removePackageFilesRefsFromDB_2(const string& name, const set<string>& keep_list);
	void removePackageFilesRefsFromDB(set<string> files, const set<string>& keep_list);
	set<string> getConflictsFilesList(const string& name, const pkginfo_t& info);

	// old member from pkgutils
	void db_open(const string& path);
//	void db_commit();

	// Tar.gz
	pair<string, pkginfo_t> openArchivePackage(const string& filename);
	pair<string, pkginfo_t> openArchivePackage_2(const string& filename) const;
	void installArchivePackage(const string& filename, const set<string>& keep_list, const set<string>& non_install_files);
	void moveMetaFilesPackage(const string& name, pkginfo_t& info); // the folder holding the meta datas is going to be create here
	void installArchivePackage_2(const string& filename, const set<string>& keep_list, const set<string>& non_install_files) const;
	void getFootprintPackage(string& filename);
	void runLdConfig();
	void convertSpaceToNoSpaceDBFormat(const string& path);

	string utilName;
	string root;
	packages_t listOfInstPackages; /* List of Installed packages */
	set<string> pkgFoldersList;
	set<string> pkgList;
	set<string> metaFilesList;
	set<string> FilesList;
	set<string> runtimeLibrairiesList;
	packages_t depListOfPackages;

	action actualAction;
	error actualError;
	unsigned int filesNumber;
	unsigned int installedFilesNumber;
};

class db_lock {
public:
	db_lock(const string& root, bool exclusive);
	~db_lock();
private:
	DIR* dir;
};

class runTimeErrorWithErrno : public runtime_error {
public:
	explicit runTimeErrorWithErrno(const string& msg) throw()
		: runtime_error(msg + string(": ") + strerror(errno)) {}
	explicit runTimeErrorWithErrno(const string& msg, int e) throw()
		: runtime_error(msg + string(": ") + strerror(e)) {}
};

// Utility functions
void assertArgument(char** argv, int argc, int index);
void rotatingCursor();
#endif /* CARDS_H */
// vim:set ts=2 :
