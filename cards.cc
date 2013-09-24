//
//  cards
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

#include "cards.h"
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

cards::cards(const string& name)
	: utilname(name)
{
	// Ignore signals
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sa, 0);
	sigaction(SIGINT, &sa, 0);
	sigaction(SIGQUIT, &sa, 0);
	sigaction(SIGTERM, &sa, 0);
}

int cards::list_pkg (const string& path) // return the number of db files founds
{
	root = trim_filename(path + "/");
	const string pathdb =  root + PKG_DIR;
	set_of_db = file_find(pathdb);
#ifndef NDEBUG
  cerr << "Number of Packages: " << set_of_db.size() << endl;
#endif
  return set_of_db.size();
}

void cards::db_open_2()
{
	actual_action = DB_OPEN_START;
	progress();
	for (set<string>::iterator i = set_of_db.begin();i != set_of_db.end();++i) {
		actual_action = DB_OPEN_RUN;
		if ( set_of_db.size() > 100 )
			progress();		
		pkginfo_t info;
		string name(*i,0, i->find(NAME_DELIM));
		string version = *i;
		version.erase(0, version.find(NAME_DELIM) == string::npos ? string::npos : version.find(NAME_DELIM) + 1);
		info.version = version;
	
		// list of files
		const string filelist = root + PKG_DIR + "/" + *i + PKG_FILES;
		int fd = open(filelist.c_str(), O_RDONLY);
		if (fd == -1)
			throw runtime_error_with_errno("could not open " + filelist);

		stdio_filebuf<char> listbuf(fd, ios::in, getpagesize());
		istream in(&listbuf);
		if (!in)
			throw runtime_error_with_errno("could not read " + filelist);

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
				packages[name] = info;
		}
	}
#ifndef NDEBUG
  cerr << endl;
  cerr << packages.size() << " packages found in database " << endl;
#endif
	actual_action = DB_OPEN_END;
	progress();
}


void cards::db_convert()
{
	/* Convert from single db file to multi directories */

	const string packagedir = root + PKG_DIR ;
	mkdir(packagedir.c_str(),0755);
	cout << packagedir << endl;
	for (packages_t::const_iterator i = packages.begin(); i != packages.end(); ++i) {

		const string packagenamedir = root + PKG_DIR + "/" + i->first + " " + i-> second.version;
		mkdir(packagenamedir.c_str(),0755);	
		const string fileslist = packagenamedir + PKG_FILES;
		const string fileslist_new = fileslist + ".imcomplete_transaction";

		int fd_new = creat(fileslist_new.c_str(),0444);

		if (fd_new == -1)
			throw runtime_error_with_errno("could not create " + fileslist_new);

		stdio_filebuf<char> filebuf_new(fd_new, ios::out, getpagesize());

		ostream db_new(&filebuf_new);
		copy(i->second.files.begin(), i->second.files.end(), ostream_iterator<string>(db_new, "\n"));
	
		db_new.flush();
		if (!db_new)
    	throw runtime_error("could not write " + fileslist_new);

		// Synchronize file to disk
		if (fsync(fd_new) == -1)
			throw runtime_error_with_errno("could not synchronize " + fileslist_new);

		// Move new database into place
		if (rename(fileslist_new.c_str(), fileslist.c_str()) == -1)
			throw runtime_error_with_errno("could not rename " + fileslist_new + " to " + fileslist);
	}
#ifndef NDEBUG
  cerr << packages.size() << " packages written to database" << endl;
#endif
}
void cards::pkg_move_metafiles(const string& name, pkginfo_t& info)
{
	actual_action = PKG_MOVE_META_START;
	progress();
	for (set<string>::iterator i = info.files.begin(); i!=info.files.end();++i) {
		if ( strncmp(i->c_str(),PKG_INSTALL_DIR,7) == 0 )
		{
			metafiles_list.insert(metafiles_list.end(), *i);
			info.files.erase(i);
		}
	}
	const string packagedir = root + PKG_DIR ;
	const string packagenamedir = root + PKG_DIR + "/" + name + " " + info.version;
	
	mkdir(packagenamedir.c_str(),0755);
	if ( metafiles_list.size()>0 )
	{
		const string installdir = root + PKG_INSTALL_DIR;
		rename( installdir.c_str(), packagenamedir.c_str() );
	}
	actual_action = PKG_MOVE_META_END;
	progress();
}	 
void cards::db_add_pkg(const string& name, const pkginfo_t& info)
{
	packages[name] = info;
	const string packagedir = root + PKG_DIR ;
	const string packagenamedir = root + PKG_DIR + "/" + name + " " + info.version;
	const string fileslist = packagenamedir + PKG_FILES;
	const string fileslist_new = fileslist + ".imcomplete_transaction";
	int fd_new = creat(fileslist_new.c_str(),0644);
	if (fd_new == -1)
		throw runtime_error_with_errno("could not create " + fileslist_new);

	stdio_filebuf<char> filebuf_new(fd_new, ios::out, getpagesize());

	ostream db_new(&filebuf_new);
	copy(info.files.begin(), info.files.end(), ostream_iterator<string>(db_new, "\n"));

	db_new.flush();
		if (!db_new)
			{ 
				rmdir(packagenamedir.c_str());
				throw runtime_error("could not write " + fileslist_new);
			}
	// Synchronize file to disk
	if (fsync(fd_new) == -1)
	{
		rmdir(packagenamedir.c_str());
		throw runtime_error_with_errno("could not synchronize " + fileslist_new);
	}
	// Move new database into place
	if (rename(fileslist_new.c_str(), fileslist.c_str()) == -1)
	{
		rmdir(packagenamedir.c_str());
		throw runtime_error_with_errno("could not rename " + fileslist_new + " to " + fileslist);
	}
}

bool cards::db_find_pkg(const string& name)
{
	return (packages.find(name) != packages.end());
}

/* Remove meta data about the removed package */
void cards::db_rm_pkg(const string& name)
{
 	  const string packagedir = root + PKG_DIR ;
		const string version = packages[name].version;
  	const string packagenamedir = root + PKG_DIR + "/" + name + " " + version;
		metafiles_list = file_find( packagenamedir);
		if (metafiles_list.size() > 0)
			for (set<string>::iterator i = metafiles_list.begin(); i != metafiles_list.end();++i) {
				const string filename = packagenamedir +"/" + *i;
				if (file_exists(filename) && remove(filename.c_str()) == -1) {
					const char* msg = strerror(errno);
					cerr << utilname << ": could not remove " << filename << ": " << msg << endl;
				}
#ifndef NDEBUG
				cout << endl << file ;
#endif
			}
		remove(packagenamedir.c_str());
}

/* Remove the physical files after followings some rules */
void cards::rm_pkg_files(const string& name)
{
	files = packages[name].files;
	packages.erase(name);

#ifndef NDEBUG
	cerr << "Removing package phase 1 (all files in package):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files that still have references
	for (packages_t::const_iterator i = packages.begin(); i != packages.end(); ++i)
		for (set<string>::const_iterator j = i->second.files.begin(); j != i->second.files.end(); ++j)
			files.erase(*j);

#ifndef NDEBUG
	cerr << "Removing package phase 2 (files that still have references excluded):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	actual_action = RM_PKG_FILES_START;
	progress();
	// Delete the files from bottom to up to make shure we delete the contents of any folder before
	for (set<string>::const_reverse_iterator i = files.rbegin(); i != files.rend(); ++i) {
		actual_action = RM_PKG_FILES_RUN;
		progress();
		const string filename = root + *i;
		if (file_exists(filename) && remove(filename.c_str()) == -1) {
			const char* msg = strerror(errno);
			cerr << utilname << ": could not remove " << filename << ": " << msg << endl;
		}
	}
	actual_action = RM_PKG_FILES_END;
	progress();
}

void cards::db_rm_pkg(const string& name, const set<string>& keep_list)
{
	files = packages[name].files;
	packages.erase(name);

#ifndef NDEBUG
	cerr << "Removing package phase 1 (all files in package):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files found in the keep list
	for (set<string>::const_iterator i = keep_list.begin(); i != keep_list.end(); ++i)
		files.erase(*i);

#ifndef NDEBUG
	cerr << "Removing package phase 2 (files that is in the keep list excluded):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Don't delete files that still have references
	for (packages_t::const_iterator i = packages.begin(); i != packages.end(); ++i)
		for (set<string>::const_iterator j = i->second.files.begin(); j != i->second.files.end(); ++j)
			files.erase(*j);

#ifndef NDEBUG
	cerr << "Removing package phase 3 (files that still have references excluded):" << endl;
	copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
	cerr << endl;
#endif

	// Delete the files
	actual_action = RM_PKG_FILES_START;
	progress();
	for (set<string>::const_reverse_iterator i = files.rbegin(); i != files.rend(); ++i) {
		actual_action = RM_PKG_FILES_RUN;
		progress();
		const string filename = root + *i;
		if (file_exists(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			cerr << utilname << ": could not remove " << filename << ": " << msg << endl;
		}
	}
	actual_action = RM_PKG_FILES_END;
	progress();
}

void cards::db_rm_files(set<string> files, const set<string>& keep_list)
{
	// Remove all references
	for (packages_t::iterator i = packages.begin(); i != packages.end(); ++i)
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
		if (file_exists(filename) && remove(filename.c_str()) == -1) {
			if (errno == ENOTEMPTY)
				continue;
			const char* msg = strerror(errno);
			cerr << utilname << ": could not remove " << filename << ": " << msg << endl;
		}
	}
}

set<string> cards::db_find_conflicts(const string& name, const pkginfo_t& info)
{
	set<string> files;
   
	// Find conflicting files in database
	for (packages_t::const_iterator i = packages.begin(); i != packages.end(); ++i) {
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
		if (file_exists(filename) && files.find(*i) == files.end())
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
	if (packages.find(name) != packages.end()) {
		for (set<string>::const_iterator i = packages[name].files.begin(); i != packages[name].files.end(); ++i)
			files.erase(*i);

#ifndef NDEBUG
		cerr << "Conflicts phase 4 (files already owned by this package excluded):" << endl;
		copy(files.begin(), files.end(), ostream_iterator<string>(cerr, "\n"));
		cerr << endl;
#endif
	}

	return files;
}

pair<string, cards::pkginfo_t> cards::pkg_open(const string& filename)
{
	pair<string, pkginfo_t> result;
	struct archive* archive;
	struct archive_entry* entry;

	// Extract name and version from filename
	string basename(filename, filename.rfind('/') + 1);
	string name(basename, 0, basename.find(VERSION_DELIM));
	string version(basename, 0, basename.rfind(PKG_EXT));
	version.erase(0, version.find(VERSION_DELIM) == string::npos ? string::npos : version.find(VERSION_DELIM) + 1);
   
	if (name.empty() || version.empty())
		throw runtime_error("could not determine name and/or version of " + basename + ": Invalid package name");

	result.first = name;
	result.second.version = version;


	archive = archive_read_new();
	INIT_ARCHIVE(archive);

	if (archive_read_open_filename(archive,
	    filename.c_str(),
	    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK)
		throw runtime_error_with_errno("could not open " + filename, archive_errno(archive));

	actual_action = PKG_OPEN_START;
	progress();
	for (number_of_files = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++number_of_files) {
			actual_action = PKG_OPEN_RUN;
			progress();

				result.second.files.insert(result.second.files.end(),
					archive_entry_pathname(entry));

		mode_t mode = archive_entry_mode(entry);

		if (S_ISREG(mode) &&
		    archive_read_data_skip(archive) != ARCHIVE_OK)
			throw runtime_error_with_errno("could not read " + filename, archive_errno(archive));
	}

	if (number_of_files == 0) {
		if (archive_errno(archive) == 0)
			throw runtime_error("empty package");
		else
			throw runtime_error("could not read " + filename);
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
	archive_read_finish(archive);
#endif
	// Retrieve number of files from the archive is complete
	actual_action = PKG_OPEN_END;
	progress();
	// printf("%u",i);
	return result;
}

void cards::pkg_install(const string& filename, const set<string>& keep_list, const set<string>& non_install_list)
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
		throw runtime_error_with_errno("could not open " + filename, archive_errno(archive));

	chdir(root.c_str());
	absroot = getcwd(buf, sizeof(buf));
	actual_action = PKG_INSTALL_START;
	progress();
	for (number_installed_files = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++number_installed_files) {
		
		actual_action = PKG_INSTALL_RUN;
		progress();
		string archive_filename = archive_entry_pathname(entry);
		string reject_dir = trim_filename(absroot + string("/") + string(PKG_REJECTED));
		string original_filename = trim_filename(absroot + string("/") + archive_filename);
		string real_filename = original_filename;

		// Check if file is filtered out via INSTALL
		if (non_install_list.find(archive_filename) != non_install_list.end()) {
			mode_t mode;

			cout << utilname << ": ignoring " << archive_filename << endl;

			mode = archive_entry_mode(entry);

			if (S_ISREG(mode))
				archive_read_data_skip(archive);

			continue;
		}

		// Check if file should be rejected
		if (file_exists(real_filename) && keep_list.find(archive_filename) != keep_list.end())
			real_filename = trim_filename(reject_dir + string("/") + archive_filename);

		archive_entry_set_pathname(entry, const_cast<char*>
		                           (real_filename.c_str()));

		// Extract file
		unsigned int flags = ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_UNLINK;
		if (archive_read_extract(archive, entry, flags) != ARCHIVE_OK) {
			// If a file fails to install we just print an error message and
			// continue trying to install the rest of the package.
			const char* msg = archive_error_string(archive);
			cerr << utilname << ": could not install " + archive_filename << ": " << msg << endl;
			continue;
		
		}

		// Check rejected file
		if (real_filename != original_filename) {
			bool remove_file = false;
			mode_t mode = archive_entry_mode(entry);

			// Directory
			if (S_ISDIR(mode))
				remove_file = permissions_equal(real_filename, original_filename);
			// Other files
			else
				remove_file = permissions_equal(real_filename, original_filename) &&
					(file_empty(real_filename) || file_equal(real_filename, original_filename));

			// Remove rejected file or signal about its existence
			if (remove_file)
				file_remove(reject_dir, real_filename);
			else
				cout << utilname << ": rejecting " << archive_filename << ", keeping existing version" << endl;
		}
	}
	actual_action = PKG_INSTALL_END;
	progress();
	if (number_installed_files == 0) {
		if (archive_errno(archive) == 0)
			throw runtime_error("empty package");
		else
			throw runtime_error("could not read " + filename);
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
	archive_read_finish(archive);
#endif
}

void cards::ldconfig() const
{
	// Only execute ldconfig if /etc/ld.so.conf exists
	if (file_exists(root + LDCONFIG_CONF)) {
		pid_t pid = fork();

		if (pid == -1)
			throw runtime_error_with_errno("fork() failed");

		if (pid == 0) {
			execl(LDCONFIG, LDCONFIG, "-r", root.c_str(), (char *) 0);
			const char* msg = strerror(errno);
			cerr << utilname << ": could not execute " << LDCONFIG << ": " << msg << endl;
			exit(EXIT_FAILURE);
		} else {
			if (waitpid(pid, 0, 0) == -1)
				throw runtime_error_with_errno("waitpid() failed");
		}
	}
}

void cards::pkg_footprint(string& filename) const
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
                throw runtime_error_with_errno("could not open " + filename, archive_errno(archive));

	for (i = 0; archive_read_next_header(archive, &entry) ==
	     ARCHIVE_OK; ++i) {

		mode_t mode = archive_entry_mode(entry);

		if (!archive_entry_hardlink(entry)) {
			const char *s = archive_entry_pathname(entry);

			hardlink_target_modes[s] = mode;
		}

		if (S_ISREG(mode) && archive_read_data_skip(archive))
			throw runtime_error_with_errno("could not read " + filename, archive_errno(archive));
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
                throw runtime_error_with_errno("could not open " + filename, archive_errno(archive));

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
			throw runtime_error_with_errno("could not read " + filename, archive_errno(archive));
	}
   
	if (i == 0) {
		if (archive_errno(archive) == 0)
			throw runtime_error("empty package");
		else
			throw runtime_error("could not read " + filename);
	}
#if ARCHIVE_VERSION_NUMBER >= 3000000
	archive_read_free(archive);
#else
	archive_read_finish(archive);
#endif

}

void cards::print_version() const
{
	cout << utilname << " (cards) " << VERSION << endl;
}

db_lock::db_lock(const string& root, bool exclusive)
	: dir(0)
{
	const string dirname = trim_filename(root + string("/") + PKG_DIR_OLD);

	if (!(dir = opendir(dirname.c_str())))
		throw runtime_error_with_errno("could not read directory " + dirname);

	if (flock(dirfd(dir), (exclusive ? LOCK_EX : LOCK_SH) | LOCK_NB) == -1) {
		if (errno == EWOULDBLOCK)
			throw runtime_error("package database is currently locked by another process");
		else
			throw runtime_error_with_errno("could not lock directory " + dirname);
	}
}

db_lock::~db_lock()
{
	if (dir) {
		flock(dirfd(dir), LOCK_UN);
		closedir(dir);
	}
}

void cards::db_commit()
{
  const string dbfilename = root + PKG_DB;
  const string dbfilename_new = dbfilename + ".incomplete_transaction";
  const string dbfilename_bak = dbfilename + ".backup";

  // Remove failed transaction (if it exists)
  if (unlink(dbfilename_new.c_str()) == -1 && errno != ENOENT)
    throw runtime_error_with_errno("could not remove " + dbfilename_new);

  // Write new database
  int fd_new = creat(dbfilename_new.c_str(), 0444);
  if (fd_new == -1)
    throw runtime_error_with_errno("could not create " + dbfilename_new);

  stdio_filebuf<char> filebuf_new(fd_new, ios::out, getpagesize());

  ostream db_new(&filebuf_new);
  for (packages_t::const_iterator i = packages.begin(); i != packages.end(); ++i) {
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
    throw runtime_error("could not write " + dbfilename_new);

  // Synchronize file to disk
  if (fsync(fd_new) == -1)
    throw runtime_error_with_errno("could not synchronize " + dbfilename_new);

  // Relink database backup
  if (unlink(dbfilename_bak.c_str()) == -1 && errno != ENOENT)
    throw runtime_error_with_errno("could not remove " + dbfilename_bak);
  if (link(dbfilename.c_str(), dbfilename_bak.c_str()) == -1)
    throw runtime_error_with_errno("could not create " + dbfilename_bak);

  // Move new database into place
  if (rename(dbfilename_new.c_str(), dbfilename.c_str()) == -1)
    throw runtime_error_with_errno("could not rename " + dbfilename_new + " to " + dbfilename);

#ifndef NDEBUG
  cerr << packages.size() << " packages written to database" << endl;
#endif
}

void cards::db_open(const string& path)
{
  // Read database from single file
  // Only need to convert from pkgutils to cards

  root = trim_filename(path + "/");
  const string filename = root + PKG_DB;

  int fd = open(filename.c_str(), O_RDONLY);
  if (fd == -1)
    throw runtime_error_with_errno("could not open " + filename);

  stdio_filebuf<char> filebuf(fd, ios::in, getpagesize());
  istream in(&filebuf);
  if (!in)
    throw runtime_error_with_errno("could not read " + filename);

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
        packages[name] = info;
  }

#ifndef NDEBUG
  cerr << endl;
  cerr << packages.size() << " packages found in database " << endl;
#endif
}
/*******************     End of Members *********************/

/*******************   Various fonctions ********************/

void assert_argument(char** argv, int argc, int index)
{
	if (argc - 1 < index + 1)
		throw runtime_error("option " + string(argv[index]) + " requires an argument");
}

string itos(unsigned int value)
{
	static char buf[20];
	sprintf(buf, "%u", value);
	return buf;
}

string mtos(mode_t mode)
{
	string s;

	// File type
	switch (mode & S_IFMT) {
        case S_IFREG:  s += '-'; break; // Regular
        case S_IFDIR:  s += 'd'; break; // Directory
        case S_IFLNK:  s += 'l'; break; // Symbolic link
        case S_IFCHR:  s += 'c'; break; // Character special
        case S_IFBLK:  s += 'b'; break; // Block special
        case S_IFSOCK: s += 's'; break; // Socket
        case S_IFIFO:  s += 'p'; break; // Fifo
        default:       s += '?'; break; // Unknown
        }

	// User permissions
        s += (mode & S_IRUSR) ? 'r' : '-';
        s += (mode & S_IWUSR) ? 'w' : '-';
        switch (mode & (S_IXUSR | S_ISUID)) {
        case S_IXUSR:           s += 'x'; break;
        case S_ISUID:           s += 'S'; break;
        case S_IXUSR | S_ISUID: s += 's'; break;
        default:                s += '-'; break;
        }

        // Group permissions
	s += (mode & S_IRGRP) ? 'r' : '-';
        s += (mode & S_IWGRP) ? 'w' : '-';
        switch (mode & (S_IXGRP | S_ISGID)) {
        case S_IXGRP:           s += 'x'; break;
        case S_ISGID:           s += 'S'; break;
	case S_IXGRP | S_ISGID: s += 's'; break;
        default:                s += '-'; break;
        }

        // Other permissions
        s += (mode & S_IROTH) ? 'r' : '-';
        s += (mode & S_IWOTH) ? 'w' : '-';
        switch (mode & (S_IXOTH | S_ISVTX)) {
        case S_IXOTH:           s += 'x'; break;
        case S_ISVTX:           s += 'T'; break;
        case S_IXOTH | S_ISVTX: s += 't'; break;
        default:                s += '-'; break;
        }

	return s;
}

string trim_filename(const string& filename)
{
	string search("//");
	string result = filename;

	for (string::size_type pos = result.find(search); pos != string::npos; pos = result.find(search))
		result.replace(pos, search.size(), "/");

	return result;
}

bool file_exists(const string& filename)
{
	struct stat buf;
	return !lstat(filename.c_str(), &buf);
}

bool file_empty(const string& filename)
{
	struct stat buf;

	if (lstat(filename.c_str(), &buf) == -1)
		return false;
	
	return (S_ISREG(buf.st_mode) && buf.st_size == 0);
}

bool file_equal(const string& file1, const string& file2)
{
	struct stat buf1, buf2;

	if (lstat(file1.c_str(), &buf1) == -1)
		return false;

	if (lstat(file2.c_str(), &buf2) == -1)
		return false;

	// Regular files
	if (S_ISREG(buf1.st_mode) && S_ISREG(buf2.st_mode)) {
		ifstream f1(file1.c_str());
		ifstream f2(file2.c_str());
	
		if (!f1 || !f2)
			return false;

		while (!f1.eof()) {
			char buffer1[4096];
			char buffer2[4096];
			f1.read(buffer1, 4096);
			f2.read(buffer2, 4096);
			if (f1.gcount() != f2.gcount() ||
			    memcmp(buffer1, buffer2, f1.gcount()) ||
			    f1.eof() != f2.eof())
				return false;
		}

		return true;
	}
	// Symlinks
	else if (S_ISLNK(buf1.st_mode) && S_ISLNK(buf2.st_mode)) {
		char symlink1[MAXPATHLEN];
		char symlink2[MAXPATHLEN];

		memset(symlink1, 0, MAXPATHLEN);
		memset(symlink2, 0, MAXPATHLEN);

		if (readlink(file1.c_str(), symlink1, MAXPATHLEN - 1) == -1)
			return false;

		if (readlink(file2.c_str(), symlink2, MAXPATHLEN - 1) == -1)
			return false;

		return !strncmp(symlink1, symlink2, MAXPATHLEN);
	}
	// Character devices
	else if (S_ISCHR(buf1.st_mode) && S_ISCHR(buf2.st_mode)) {
		return buf1.st_dev == buf2.st_dev;
	}
	// Block devices
	else if (S_ISBLK(buf1.st_mode) && S_ISBLK(buf2.st_mode)) {
		return buf1.st_dev == buf2.st_dev;
	}

	return false;
}

bool permissions_equal(const string& file1, const string& file2)
{
	struct stat buf1;
	struct stat buf2;

	if (lstat(file1.c_str(), &buf1) == -1)
		return false;

	if (lstat(file2.c_str(), &buf2) == -1)
		return false;
	
	return(buf1.st_mode == buf2.st_mode) &&
		(buf1.st_uid == buf2.st_uid) &&
		(buf1.st_gid == buf2.st_gid);
}

void file_remove(const string& basedir, const string& filename)
{
	if (filename != basedir && !remove(filename.c_str())) {
		char* path = strdup(filename.c_str());
		file_remove(basedir, dirname(path));
		free(path);
	}
}
set<string> file_find(const string& path)
{
	set<string> files_list;
	DIR *d;
	struct dirent *dir;
	d = opendir(path.c_str());
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
		if ( strcmp (dir->d_name, ".") && strcmp (dir->d_name, "..") ) // ignore the directories dots
        files_list.insert(dir->d_name);
    }
    closedir(d);
  }
  return files_list;
}

void advance_cursor() {
  static int pos=0;
  char cursor[4]={'/','-','\\','|'};
  printf("%c\b", cursor[pos]);
  fflush(stdout);
  pos = (pos+1) % 4;
}
// vim:set ts=2 :
