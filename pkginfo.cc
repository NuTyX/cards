//  pkginfo.cc
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

#include "pkginfo.h"
#include <iterator>
#include <vector>
#include <iomanip>
#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <unistd.h>

void pkginfo::run(int argc, char** argv)
{
	//
	// Check command line options
	//
	int o_footprint_mode = 0;
	int o_installed_mode = 0;
	int o_list_mode = 0;
	int o_owner_mode = 0;
	int o_convert_mode = 0;
	int o_debug_mode = 0;
	string o_root;
	string o_arg;

	for (int i = 1; i < argc; ++i) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assert_argument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option == "-i" || option == "--installed") {
			o_installed_mode += 1;
		} else if (option == "-c" || option == "--convert") {
			o_convert_mode += 1;
		} else if (option == "-d" || option == "--debug") {
			o_debug_mode += 1;
		} else if (option == "-l" || option == "--list") {
			assert_argument(argv, argc, i);
			o_list_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-o" || option == "--owner") {
			assert_argument(argv, argc, i);
			o_owner_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-f" || option == "--footprint") {
			assert_argument(argv, argc, i);
			o_footprint_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else {
			actual_error = INVALID_OPTION;
			error_treatment(option);
		}
	}

	if (o_footprint_mode + o_installed_mode + o_list_mode + o_owner_mode + o_debug_mode + o_convert_mode == 0)
	{
		actual_error = OPTION_MISSING;
		error_treatment(o_arg);
	}
	if (o_footprint_mode + o_installed_mode + o_list_mode + o_owner_mode > 1)
	{
		actual_error = TOO_MANY_OPTIONS;
		error_treatment(o_arg);
	}
	if (o_footprint_mode) {
		//
		// Make footprint
		//
		pkg_footprint(o_arg);
	} else if (o_convert_mode ) {
			if (getuid())
			{
				actual_error = ONLY_ROOT_CAN_CONVERT_DB;
				error_treatment("");
			}

			const string new_db = PKG_DB_DIR;	
    	if (file_exists("/" + new_db))
			{
				db_convert_space_to_no_space(o_root);
			} else {
				db_lock lock(o_root, false);
				db_open(o_root);
				db_convert(); }
	} else {
		//
		// Modes that require the database to be opened
		//
		{
			db_lock lock(o_root, false);
			list_pkg(o_root);
		}

		if (o_installed_mode) {
			//
			// List installed packages
			//
			for (set<string>::const_iterator i = set_of_db.begin(); i != set_of_db.end(); ++i)
				cout << *i << endl;
		} else if (o_list_mode) {
			//
			// List package or file contents
			//
			db_open_2();
			if (db_find_pkg(o_arg)) {
				copy(packages[o_arg].files.begin(), packages[o_arg].files.end(), ostream_iterator<string>(cout, "\n"));
				cout << endl << "Name: " << o_arg << endl
				<< "Version: " << packages[o_arg].version << endl 
				<< "Description: " << packages[o_arg].description << endl
				<< "URL: " << packages[o_arg].url << endl
				<< "Packager : " << packages[o_arg].packager << endl
				<< "Size : " << packages[o_arg].size << endl
				<< "Maintainer : " << packages[o_arg].maintainer << endl
				<< "Depends on (run): " << packages[o_arg].depends << endl;
			} else if (file_exists(o_arg)) {
				pair<string, pkginfo_t> package = pkg_open(o_arg);
				copy(package.second.files.begin(), package.second.files.end(), ostream_iterator<string>(cout, "\n"));
			
			} else {
				actual_error = NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE;
				error_treatment(o_arg);
			}
		} else if (o_owner_mode) {
			//
			// List owner(s) of file or directory
			//
			db_open_2();
			regex_t preg;
			if (regcomp(&preg, o_arg.c_str(), REG_EXTENDED | REG_NOSUB))
			{
				actual_error = CANNOT_COMPILE_REGULAR_EXPRESSION;
				error_treatment(o_arg);
			}
			vector<pair<string, string> > result;
			result.push_back(pair<string, string>("Package", "File"));
			unsigned int width = result.begin()->first.length(); // Width of "Package"
#ifndef NDEBUG
			cerr << o_arg << endl;
#endif
			for (packages_t::const_iterator i = packages.begin(); i != packages.end(); ++i) {
				for (set<string>::const_iterator j = i->second.files.begin(); j != i->second.files.end(); ++j) {
					const string file('/' + *j);
					if (!regexec(&preg, file.c_str(), 0, 0, 0)) {
						result.push_back(pair<string, string>(i->first, *j));
						if (i->first.length() > width)
							width = i->first.length();
					}
				}
			}
			
			regfree(&preg);
			
			if (result.size() > 1) {
				for (vector<pair<string, string> >::const_iterator i = result.begin(); i != result.end(); ++i) {
					cout << left << setw(width + 2) << i->first << i->second << endl;
				}
			} else {
				cout << utilname << ": no owner(s) found" << endl;
			}
		} else {
				cout << utilname << ":debug" << endl;
		}
	}
}
void pkginfo::print_help() const
{
	cout << "usage: " << utilname << " [options]" << endl
	     << "options:" << endl
       << "  -d, --debug                 test the database" << endl
       << "  -c, --convert               convert the datase from pkgutils format to cards format" << endl 
	     << "  -i, --installed             list installed packages" << endl
	     << "  -l, --list <package|file>   list files in <package> or <file>" << endl
	     << "  -o, --owner <pattern>       list owner(s) of file(s) matching <pattern>" << endl
	     << "  -f, --footprint <file>      print footprint for <file>" << endl
	     << "  -r, --root <path>           specify alternative installation root" << endl
	     << "  -v, --version               print version and exit" << endl
	     << "  -h, --help                  print help and exit" << endl;
}
// vim:set ts=2 :
