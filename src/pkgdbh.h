//
//  pkgdbh.h
//
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013-2017 by NuTyX team (http://nutyx.org)
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

/**
 * Pkgdbh class.
 */

#ifndef PKGDBH_H
#define PKGDBH_H

#include "archive_utils.h"
#include "file_utils.h"
#include "process.h"

#include <stdexcept>
#include <csignal>
#include <algorithm>

#include <regex.h>
#include <ext/stdio_filebuf.h>
#include <pwd.h>
#include <grp.h>

#define PKG_DB_DIR       "var/lib/pkg/DB/"
#define PKG_FILES        "/files"
#define PKG_META         "META"
#define PKG_RECEPT       "Pkgfile"
#define PKG_README       "README"
#define PKG_PRE_INSTALL  ".PRE"
#define PKG_POST_INSTALL ".POST"

#define PKG_REJECTED     "var/lib/pkg/rejected"
#define PKGADD_CONF      "var/lib/pkg/pkgadd.conf"
#define PKGADD_CONF_MAXLINE	1024

#define LDCONFIG         "sbin/ldconfig"
#define LDCONFIG_CONF    "etc/ld.so.conf"
#define LDCONFIG_CONF_ARGS "-r "
#define SHELL            "bin/sh"

// /usr/bin/install-info --info-dir="/usr/share/info" /usr/share/info/<file>.info"
#define INSTALL_INFO      "usr/bin/install-info"
#define INSTALL_INFO_ARGS "--info-dir=usr/share/info "

// /usr/bin/gtk-update-icon-cache -f -t /usr/share/icons/hicolor
#define UPDATE_ICON       "usr/bin/gtk-update-icon-cache"
#define UPDATE_ICON_ARGS  "-f -t "

// /usr/bin/glib-compile-schemas /usr/share/glib-2/schemas
#define COMPILE_SCHEMAS       "usr/bin/glib-compile-schemas"
#define COMPILE_SCHEMAS_ARGS  ""

// /usr/bin/update-desktop-database -q /usr/share/applications
#define UPDATE_DESKTOP_DB      "usr/bin/update-desktop-database"
#define UPDATE_DESKTOP_DB_ARGS "-q "

// /usr/bin/update-mime-database usr/share/mime
#define UPDATE_MIME_DB  "usr/bin/update-mime-database"
#define UPDATE_MIME_DB_ARGS  "-n "

// /usr/bin/gdk-pixbuf-query-loaders --update-cache
#define GDK_PIXBUF_QUERY_LOADER "usr/bin/gdk-pixbuf-query-loaders"
#define GDK_PIXBUF_QUERY_LOADER_ARGS "--update-cache"

// /usr/bin/gio-querymodules /usr/lib/gio/modules
#define GIO_QUERYMODULES "usr/bin/gio-querymodules"
#define GIO_QUERYMODULES_ARGS "usr/lib/gio/modules"

// /usr/bin/gtk-query-immodules-3.0 --update-cache
#define QUERY_IMMODULES_3 "usr/bin/gtk-query-immodules-3.0"
#define QUERY_IMMODULES_3_ARGS "--update-cache"

// /usr/bin/gtk-query-immodules-2.0 --update-cache
#define QUERY_IMMODULES_2 "usr/bin/gtk-query-immodules-2.0"
#define QUERY_IMMODULES_2_ARGS "--update-cache"

// /usr/bin/mkfontdir /usr/share/fonts/<dir>/
#define MKFONTDIR       "usr/bin/mkfontdir"
#define MKFONTDIR_ARGS  ""

// /usr/bin/mkfontscale /usr/share/fonts/<dir>/
#define MKFONTSCALE     "usr/bin/mkfontscale"
#define MKFONTSCALE_ARGS  ""

// /usr/bin/fc-cache /usr/share/fonts/<dir>/
#define FC_CACHE     "usr/bin/fc-cache"
#define FC_CACHE_ARGS  ""

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
	std::string base;
	std::string group;
	std::string collection;
	std::string description;
	std::string signature;
	time_t build; // date of build
	std::string version;
	int release;
	std::string url;
	std::string contributors;
	std::string packager;
	std::string maintainer;
	time_t install; // date of last installation
	std::string arch;
	int size;
	std::set< std::pair<std::string,time_t> > dependencies;
	std::set<std::string> alias;
	std::set<std::string> files;
};
typedef std::map<std::string, pkginfo_t> packages_t;
typedef std::map<std::string, std::string> alias_t;

enum rule_event_t {
	LDCONF,
	UPGRADE,
	INSTALL,
	INFO,
	ICONS,
	FONTS,
	SCHEMAS,
	DESKTOP_DB,
	MIME_DB,
	QUERY_PIXBUF,
	GIO_QUERY,
	QUERY_IMOD3,
	QUERY_IMOD2
};

struct rule_t {
	rule_event_t event;
	std::string pattern;
	bool action;
};

class Pkgdbh {
public:

	explicit Pkgdbh(const std::string& name);
	virtual ~Pkgdbh();

	/* Following methods can be redefined in derivated class */
	virtual void parseArguments(int argc, char** argv);
	virtual void run(int argc, char** argv) {};
	virtual void run() {};

	virtual void printHelp() const {};

	virtual void progressInfo() const;
	virtual void treatErrors(const std::string& s) const;


	void print_version() const;
	int getNumberOfPackages();
	std::set<std::string> getListOfPackageName();
	bool checkPackageNameExist(const std::string& name) const;
	unsigned int getFilesNumber();
	unsigned int getInstalledFilesNumber();

protected:
	// Database

	std::set<std::string> getFilesOfPackage(const std::string& packageName);
	int getListOfPackageNames(const std::string& path);
	std::pair<std::string, pkginfo_t> getInfosPackage(const std::string& packageName);
	void buildSimpleDatabase();
	void buildCompleteDatabase(const bool& silent);
	void buildDatabase(const bool& progress,
	const bool& simple,
	const bool& all,
	const bool& files,
	const std::string& packageName);


	void addPackageFilesRefsToDB(const std::string& name,
		const pkginfo_t& info);

	bool checkPackageNameUptodate(const std::pair<std::string,
		pkginfo_t>& archiveName);
	bool checkPackageNameBuildDateSame(const std::pair<std::string,
		time_t>& dependencieNameBuild);

	/*
	 * Remove the physical files after followings some rules
	 */
	void removePackageFiles(const std::string& name);
	void removePackageFiles(const std::string& name,
		const std::set<std::string>& keep_list);

	/*
	 * Remove meta data about the removed package
	 */
	void removePackageFilesRefsFromDB(const std::string& name);
	void removePackageFilesRefsFromDB(std::set<std::string> files,
		const std::set<std::string>& keep_list);
	std::set<std::string> getConflictsFilesList(const std::string& name,
		const pkginfo_t& info);

	// Tar.gz
	std::pair<std::string, pkginfo_t> openArchivePackage(const std::string& filename);
	std::set< std::pair<std::string, time_t> > getPackageDependencies(const std::string& filename);
	void extractAndRunPREfromPackage(const std::string& filename);
	void installArchivePackage(const std::string& filename,
	const std::set<std::string>& keep_list,
	const std::set<std::string>& non_install_files);

	/*
	 * The folder holding the meta datas is going to be create here
	 */
	void moveMetaFilesPackage(const std::string& name, pkginfo_t& info);


	void readRulesFile();
	void getInstallRulesList(const std::vector<rule_t>& rules,
		rule_event_t event, std::vector<rule_t>& found) const;
	bool checkRuleAppliesToFile(const rule_t& rule,
		const std::string& file);

	void getFootprintPackage(std::string& filename);

	std::string m_packageArchiveName;
	std::string m_packageName;
	std::string m_packageArchiveVersion;
	std::string m_packageArchiveRelease;
	std::string m_packageArchiveCollection;
	std::string m_packageVersion;
	std::string m_packageRelease;
	std::string m_packageCollection;
	std::string m_utilName;
	std::string m_root;
	std::string m_build;
	std::vector<rule_t> m_actionRules;
	std::set< std::pair<std::string, int> > m_postInstallList;
	alias_t	m_listOfAlias;

	packages_t m_listOfInstPackages;
	packages_t m_listOfDepotPackages;

	action m_actualAction;
	error m_actualError;

private:

	void runLastPostInstall();

	std::set<std::string> m_runtimeLibrariesList;
	std::set<std::string> m_filesList;
	std::set<std::string> m_packageNamesList;
	unsigned int m_filesNumber;
	unsigned int m_installedFilesNumber;


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
