//  pkgdbh.h
// 
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013-2016 by NuTyX team (http://nutyx.org)
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

#include "archive_utils.h"
#include "file_utils.h"

#include <stdexcept>
#include <csignal>
#include <cerrno>
#include <sys/types.h>

#define PKG_DB_DIR       "var/lib/pkg/DB/"
#define PKG_FILES        "/files"
#define PKG_META         "META"
#define PKG_RECEPT       "Pkgfile"
#define PKG_README       "README"
#define PKG_PRE_INSTALL  ".PRE"
#define PKG_POST_INSTALL ".POST"    

#define PKG_REJECTED     "var/lib/pkg/rejected"
#define PKGADD_CONF      "etc/pkgadd.conf"
#define PKGADD_CONF_MAXLINE	1024

#define LDCONFIG         "/sbin/ldconfig"
#define LDCONFIG_CONF    "/etc/ld.so.conf"
#define SHELL            "bin/sh"

enum action 
{ 
PKG_DOWNLOAD_START,
PKG_DOWNLOAD_RUN,
PKG_DOWNLOAD_END,
DB_OPEN_START,
DB_OPEN_RUN,
DB_OPEN_END,
PKG_PREINSTALL_START,
PKG_PREINSTALL_END,
PKG_INSTALL_START,
PKG_INSTALL_END,
PKG_INSTALL_RUN,
PKG_POSTINSTALL_START,
PKG_POSTINSTALL_END,
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
	std::string description;
	time_t build; // date of build
	std::string version;
	int release;
	std::string url;
	std::string packager;
	std::string maintainer;
	time_t install; // date of installation
	std::string arch;
	std::string size;
	std::set< std::pair<std::string,time_t> > dependencies;
	std::set<std::string> files;
};
typedef std::map<std::string, pkginfo_t> packages_t;

enum rule_event_t {
	UPGRADE,
	INSTALL
};

struct rule_t {
	rule_event_t event;
	std::string pattern;
	bool action;
};

class Pkgdbh {
public:

	explicit Pkgdbh(const std::string& name);
	virtual ~Pkgdbh() {}

	/* Following methods can be redefined in derivated class */
	virtual void parseArguments(int argc, char** argv);
	virtual void run(int argc, char** argv) {};
	virtual void run() {};
	virtual void finish() ;

	virtual void printHelp() const {};

	virtual void progressInfo() const;
	virtual void treatErrors(const std::string& s) const;


	void print_version() const;
	int getNumberOfPackages();
	bool checkPackageNameExist(const std::string& name) const;

protected:
	// Database

	int getListOfPackageNames(const std::string& path);
	std::pair<std::string, pkginfo_t> getInfosPackage(const std::string& packageName);
	void buildDatabaseWithNameVersion();
	void buildDatabaseWithDetailInfos(const bool& silent);
	void buildDatabaseWithDetailInfos(const std::string& path);


	void addPackageFilesRefsToDB(const std::string& name, const pkginfo_t& info);
	void addPackageFilesRefsToDB_2(const std::string& name, const pkginfo_t& info);
	
	bool checkPackageNameUptodate(const std::pair<std::string, pkginfo_t>& archiveName);
	bool checkPackageNameBuildDateSame(const std::pair<std::string,time_t>& dependencieNameBuild);
	bool checkPackageNameExist_2(const std::string& name);	
	/* Remove the physical files after followings some rules */
	void removePackageFiles(const std::string& name);
	
	void removePackageFiles(const std::string& name, const std::set<std::string>& keep_list);

	/* Remove meta data about the removed package */
	void removePackageFilesRefsFromDB(const std::string& name);
	void removePackageFilesRefsFromDB_2(const std::string& name, const std::set<std::string>& keep_list);
	void removePackageFilesRefsFromDB(std::set<std::string> files, const std::set<std::string>& keep_list);
	std::set<std::string> getConflictsFilesList(const std::string& name, const pkginfo_t& info);

	// Tar.gz
	std::pair<std::string, pkginfo_t> openArchivePackage(const std::string& filename);
	std::set< std::pair<std::string, time_t> > getPackageDependencies(const std::string& filename);
	void extractAndRunPREfromPackage(const std::string& filename);
	void installArchivePackage(const std::string& filename, const std::set<std::string>& keep_list, const std::set<std::string>& non_install_files);
	void moveMetaFilesPackage(const std::string& name, pkginfo_t& info); // the folder holding the meta datas is going to be create here
	void installArchivePackage_2(const std::string& filename, const std::set<std::string>& keep_list, const std::set<std::string>& non_install_files) const;


	void readRulesFile();
	void getInstallRulesList(const std::vector<rule_t>& rules, rule_event_t event, std::vector<rule_t>& found) const;
	bool checkRuleAppliesToFile(const rule_t& rule, const std::string& file);

	void getFootprintPackage(std::string& filename);
	void runLdConfig();
	std::string m_packageArchiveName;
	std::string m_packageName;
	std::string m_packageArchiveVersion;
	std::string m_packageArchiveRelease;
	std::string m_packageVersion;
	std::string m_packageRelease;
	std::string m_utilName;
	std::string m_root;
	std::string m_build;
	std::vector<rule_t> m_actionRules;

	packages_t m_listOfInstPackages;
	packages_t m_listOfDepotPackages;

	std::set<std::string> m_packageNamesList;
	std::set<std::string> m_filesList;
	std::set<std::string> m_runtimeLibrariesList;

	action m_actualAction;
	error m_actualError;
	unsigned int m_filesNumber;
	unsigned int m_installedFilesNumber;
private:
	bool m_DB_Empty;
	bool m_miniDB_Empty;
};

class Db_lock {
public:
	Db_lock(const std::string& m_root, bool exclusive);
	~Db_lock();

private:
	DIR* m_dir;
	struct sigaction m_sa;
};

// Utility functions
void assertArgument(char** argv, int argc, int index);
void rotatingCursor();
#endif /* PKGDBH_H */
// vim:set ts=2 :
