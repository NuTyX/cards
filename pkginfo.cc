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
	int o_details_mode = 0;
	int o_dependencies_mode = 0;
	int o_librairies_mode = 0;
	int o_runtime_mode = 0;

	string o_root;
	string o_arg;

	for (int i = 1; i < argc; ++i) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option == "-i" || option == "--installed") {
			o_installed_mode += 1;
		} else if (option == "-c" || option == "--convert") {
			o_convert_mode += 1;
		} else if (option == "-D" || option == "--depends") {
			assertArgument(argv, argc, i);
			o_dependencies_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-d" || option == "--details") {
			assertArgument(argv, argc, i);
			o_details_mode +=1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-l" || option == "--list") {
			assertArgument(argv, argc, i);
			o_list_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-R" || option == "--runtime") {
			assertArgument(argv, argc, i);
			o_runtime_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-o" || option == "--owner") {
			assertArgument(argv, argc, i);
			o_owner_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-f" || option == "--footprint") {
			assertArgument(argv, argc, i);
			o_footprint_mode += 1;
			o_arg = argv[i + 1];
			i++;
		} else if (option == "-L" || option == "--librairies") {
			assertArgument(argv, argc, i);
			o_librairies_mode +=1;
			o_arg =  argv[i + 1];
			i++;
		} else {
			actualError = INVALID_OPTION;
			treatErrors(option);
		}
	}

	if (o_footprint_mode + o_details_mode + o_dependencies_mode + 
	o_installed_mode + o_list_mode + o_owner_mode + o_convert_mode + 
	o_footprint_mode + o_librairies_mode + o_runtime_mode == 0)
	{
		actualError = OPTION_MISSING;
		treatErrors(o_arg);
	}
	if (o_footprint_mode + o_installed_mode + o_list_mode + o_owner_mode > 1)
	{
		actualError = TOO_MANY_OPTIONS;
		treatErrors(o_arg);
	}
	if (o_footprint_mode) {
		//
		// Make footprint
		//
		getFootprintPackage(o_arg);
	} else if (o_convert_mode ) {
			if (getuid())
			{
				actualError = ONLY_ROOT_CAN_CONVERT_DB;
				treatErrors("");
			}

			const string new_db = PKG_DB_DIR;	
    	if (checkFileExist("/" + new_db))
			{
				convertSpaceToNoSpaceDBFormat(o_root);
			} else {
				db_lock lock(o_root, false);
				db_open(o_root);
				convertDBFormat(); }
	} else {
		//
		// Modes that require the database to be opened
		//
		{
			db_lock lock(o_root, false);
			getListOfPackages(o_root);
		}

		if (o_installed_mode) {
			//
			// List installed packages
			//
			for (set<string>::const_iterator i = pkgList.begin(); i != pkgList.end(); ++i)
				cout << *i << endl;
		} else if (o_list_mode) {
			//
			// List package or file contents
			//
			getInstalledPackages(false);
			if (checkPackageNameExist(o_arg)) {
				copy(listOfInstPackages[o_arg].files.begin(), listOfInstPackages[o_arg].files.end(), ostream_iterator<string>(cout, "\n"));
			} else if (checkFileExist(o_arg)) {
				pair<string, pkginfo_t> package = openArchivePackage(o_arg);
				copy(package.second.files.begin(), package.second.files.end(), ostream_iterator<string>(cout, "\n"));
			
			} else {
				actualError = NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE;
				treatErrors(o_arg);
			}
		} else if (o_librairies_mode + o_runtime_mode > 0) {
			getInstalledPackages(true);
			set<string> librairiesList;
			int Result;
			if (checkPackageNameExist(o_arg))
			{
				for (set<string>::const_iterator i = listOfInstPackages[o_arg].files.begin(); i != listOfInstPackages[o_arg].files.end(); ++i)
				{
					string filename('/' + *i);
					Result = getRuntimeLibrairiesList(librairiesList,filename);
				}
				if (librairiesList.size() > 0)
				{
					if (o_runtime_mode)
					{
						set<string> runtimeList;
						for (set<string>::const_iterator i = librairiesList.begin();i != librairiesList.end();++i)
						{
							for (packages_t::const_iterator j = listOfInstPackages.begin(); j != listOfInstPackages.end();++j)
							{
								bool found = false;
								for (set<string>::const_iterator k = j->second.files.begin(); k != j->second.files.end(); ++k)
								{
									if ( k->find('/' + *i) != string::npos)
									{
										runtimeList.insert(j->first);
										break;
										found = true;
									}
								}
								if (found == true)
								{
									found = false;
									break;
								}
							}
						}
						if (runtimeList.size()>0)
						{
							int s = 1;
							for (set<string>::const_iterator i = runtimeList.begin();i!=runtimeList.end();++i)
							{
								cout << *i;
								s++;
								if (s <= runtimeList.size())
									cout << ",";
							}
						}
					} else {
							for (set<string>::const_iterator i = librairiesList.begin();i != librairiesList.end();++i)
								cout << *i << endl;
					}
				}
			}	
		} else if (o_dependencies_mode) {
			getInstalledPackages(false);
			set<string> listOfPackageName;
			listOfPackageName.insert(o_arg);
			getDependenciesList(listOfPackageName);
			if (depListOfPackages.size() > 0)
			{
					cout << "Number of deps: " << depListOfPackages.size() << endl;
				for (packages_t::const_iterator i = depListOfPackages.begin(); i != depListOfPackages.end();i++)
				{
					cout << i->first << " " << i->second.version << endl;
				}
			}
		} else if (o_details_mode) {
			getInstalledPackages(false);
			if (checkPackageNameExist(o_arg)) {
				cout << "Name: " << o_arg << endl
				<< "Version: " << listOfInstPackages[o_arg].version << endl
				<< "Description: " << listOfInstPackages[o_arg].description << endl
				<< "URL: " << listOfInstPackages[o_arg].url << endl
				<< "Packager : " << listOfInstPackages[o_arg].packager << endl
				<< "Size : " << listOfInstPackages[o_arg].size << endl
				<< "Maintainer : " << listOfInstPackages[o_arg].maintainer << endl
				<< "Depends on (run): " << listOfInstPackages[o_arg].run <<"."<< endl;
				}
			} else if (o_owner_mode) {
			//
			// List owner(s) of file or directory
			//
			getInstalledPackages(false);
			regex_t preg;
			if (regcomp(&preg, o_arg.c_str(), REG_EXTENDED | REG_NOSUB))
			{
				actualError = CANNOT_COMPILE_REGULAR_EXPRESSION;
				treatErrors(o_arg);
			}
			vector<pair<string, string> > result;
			result.push_back(pair<string, string>("Package", "File"));
			unsigned int width = result.begin()->first.length(); // Width of "Package"
#ifndef NDEBUG
			cerr << o_arg << endl;
#endif
			for (packages_t::const_iterator i = listOfInstPackages.begin(); i != listOfInstPackages.end(); ++i) {
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
				cout << utilName << ": no owner(s) found" << endl;
			}
		}
	}
}
void pkginfo::printHelp() const
{
	cout << "usage: " << utilName << " [options]" << endl
	     << "options:" << endl
       << "  -c, --convert               convert the datase from pkgutils format to cards format" << endl 
	     << "  -i, --installed             list installed packages" << endl
       << "  -d, --details               list details about the <package>" << endl
       << "  -D, --depends               list of the dependencies founds in the Pkgmeta file recursively" << endl
       << "  -L, --librairies            list all the runtime librairies for the <package>" << endl
	     << "  -l, --list <package|file>   list files in <package> or <file>" << endl
	     << "  -o, --owner <pattern>       list owner(s) of file(s) matching <pattern>" << endl
	     << "  -f, --footprint <file>      print footprint for <file>" << endl
       << "  -R, --runtime <package>		 return on a single line all the runtime dependencies" << endl
	     << "  -r, --root <path>           specify alternative installation root" << endl
	     << "  -v, --version               print version and exit" << endl
	     << "  -h, --help                  print help and exit" << endl;
}
// vim:set ts=2 :
