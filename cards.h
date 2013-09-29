//  cards.h
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

#ifndef CARDS_H
#define CARDS_H

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
#define PKG_DIR_OLD      "var/lib/pkg"
#define PKG_DIR          "var/lib/pkg/DB"
#define PKG_DB           "var/lib/pkg/db"
#define PKG_FILES        "/files"
#define PKG_INSTALL_DIR  "install"
#define PKG_RECEPT       "/Pkgfile"
#define PKG_README       "/README"
#define PKG_PRE_INSTALL  "/pre-install"
#define PKG_POST_INSTALL "/post-install"    

#define PKG_REJECTED     "var/lib/pkg/rejected"
#define VERSION_DELIM    '#'
#define GROUP_DELIM      '#'
#define NAME_DELIM			 ' '

#define LDCONFIG         "/sbin/ldconfig"
#define LDCONFIG_CONF    "/etc/ld.so.conf"

using namespace std;

struct parameter_value {
		string parameter;
		string value;
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

class cards {
public:
	struct pkginfo_t {
		string version;
		set<string> files;
	};
	typedef map<string, pkginfo_t> packages_t;

	explicit cards(const string& name);
	virtual ~cards() {}
	virtual void run(int argc, char** argv) = 0; // Need to be redefine in derivated class
	virtual void print_help() const = 0; // help info is depending of the derivated class
	virtual void progress() const; // progress info
	void print_version() const;

protected:
	// Database

	void db_convert();
	int list_pkg(const string& path );
	void db_open_2();
	void db_open_2(const string& path);
	void db_add_pkg(const string& name, const pkginfo_t& info);
	void db_add_pkg_2(const string& name, const pkginfo_t& info);
	bool db_find_pkg(const string& name);
	bool db_find_pkg_2(const string& name);	
	/* Remove the physical files after followings some rules */
	void rm_pkg_files(const string& name);
	
	void db_rm_pkg(const string& name, const set<string>& keep_list);

	/* Remove meta data about the removed package */
	void db_rm_pkg(const string& name);
	void db_rm_pkg_2(const string& name, const set<string>& keep_list);
	void db_rm_files(set<string> files, const set<string>& keep_list);
	set<string> db_find_conflicts(const string& name, const pkginfo_t& info);

	// old member from pkgutils
	void db_open(const string& path);
	void db_commit();

	// Tar.gz
	pair<string, pkginfo_t> pkg_open(const string& filename);
	pair<string, pkginfo_t> pkg_open_2(const string& filename) const;
	void pkg_install(const string& filename, const set<string>& keep_list, const set<string>& non_install_files);
	void pkg_move_metafiles(const string& name, pkginfo_t& info); // the folder holding the meta datas is going to be create here
	void pkg_install_2(const string& filename, const set<string>& keep_list, const set<string>& non_install_files) const;
	void pkg_footprint(string& filename) const;
	void ldconfig() const;

	string utilname;
	string root;
	packages_t packages;
	set<string> set_of_db;
	set<string> metafiles_list;
	set<string>	files_list;

	action actual_action;		
	unsigned int number_of_files;
	unsigned int number_installed_files;
};

class db_lock {
public:
	db_lock(const string& root, bool exclusive);
	~db_lock();
private:
	DIR* dir;
};

class runtime_error_with_errno : public runtime_error {
public:
	explicit runtime_error_with_errno(const string& msg) throw()
		: runtime_error(msg + string(": ") + strerror(errno)) {}
	explicit runtime_error_with_errno(const string& msg, int e) throw()
		: runtime_error(msg + string(": ") + strerror(e)) {}
};

// Utility functions
parameter_value split_parameter_value(string s, string delimiter);
set<string> get_parameter_list(string file, string delimiter);
string get_configuration_value(string file, string delimiter,string parameter);
void assert_argument(char** argv, int argc, int index);
string itos(unsigned int value);
string mtos(mode_t mode);
string trim_filename(const string& filename);
bool file_exists(const string& filename);
bool file_empty(const string& filename);
bool file_equal(const string& file1, const string& file2);
bool permissions_equal(const string& file1, const string& file2);
void file_remove(const string& basedir, const string& filename);
set<string>  file_find(const string& basedir);
void advance_cursor();
#endif /* CARDS_H */
// vim:set ts=2 :
