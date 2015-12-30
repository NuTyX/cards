//  pkginfo.cc
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

#include "pkginfo.h"
#include <iterator>
#include <vector>
#include <iomanip>
#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>
#include <sstream>

Pkginfo::Pkginfo()
	: Pkgdbh("pkginfo"),
	m_runtimedependencies_mode(0),
	m_footprint_mode(0),
	m_archiveinfo(0),
	m_installed_mode(0),
	m_list_mode(0),
	m_owner_mode(0),
	m_details_mode(0),
	m_librairies_mode(0),
	m_runtime_mode(0),
	m_epoc(0)
{
}
void Pkginfo::parseArguments(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			m_root = argv[i + 1];
			i++;
		} else if (option == "-i" || option == "--installed") {
			m_installed_mode += 1;
		} else if (option == "-d" || option == "--details") {
			assertArgument(argv, argc, i);
			m_details_mode +=1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-l" || option == "--list") {
			assertArgument(argv, argc, i);
			m_list_mode += 1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-R" || option == "--runtimedep") {
			assertArgument(argv, argc, i);
			m_runtime_mode += 1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-o" || option == "--owner") {
			assertArgument(argv, argc, i);
			m_owner_mode += 1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-a" || option == "--archive") {
			assertArgument(argv, argc, i);
			m_archiveinfo += 1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-f" || option == "--footprint") {
			assertArgument(argv, argc, i);
			m_footprint_mode += 1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-L" || option == "--librairies") {
			assertArgument(argv, argc, i);
			m_librairies_mode +=1;
			m_arg =  argv[i + 1];
			i++;
		} else if (option == "--runtimedepfiles") {
			assertArgument(argv, argc, i);
			m_runtimedependencies_mode +=1;
			m_arg = argv[i + 1];
			i++;
		} else if (option == "-b" || option == "--buildtime") {
			assertArgument(argv, argc, i);
			m_epoc +=1;
			m_arg = argv[i + 1];
			i++; 
		} else {
			m_actualError = INVALID_OPTION;
			treatErrors(option);
		}
		if (m_root.empty())
			m_root="/";
		else
			m_root=m_root+"/";

	}

	m_packageArchiveName = m_arg;

	if (m_runtimedependencies_mode + m_footprint_mode + m_details_mode +
	m_installed_mode + m_list_mode + m_owner_mode + m_epoc + m_archiveinfo +
	m_footprint_mode + m_librairies_mode + m_runtime_mode == 0)
	{
		m_actualError = OPTION_MISSING;
		treatErrors(m_arg);
	}
	if (m_runtimedependencies_mode + m_footprint_mode + m_installed_mode + m_archiveinfo + m_list_mode + m_owner_mode > 1)
	{
		m_actualError = TOO_MANY_OPTIONS;
		treatErrors(m_arg);
	}
}
void Pkginfo::run()
{
	if (m_archiveinfo) {
		pair<string, pkginfo_t> packageArchive = openArchivePackage(m_packageArchiveName) ;
		cout	<< packageArchive.first << " Description    : " << packageArchive.second.description << endl
			<< packageArchive.first << " URL            : " << packageArchive.second.url << endl
			<< packageArchive.first << " Maintainer(s)  : " << packageArchive.second.maintainer << endl
			<< packageArchive.first << " Packager(s)    : " << packageArchive.second.packager << endl
			<< packageArchive.first << " Version        : " << packageArchive.second.version << endl
			<< packageArchive.first << " Release        : " << packageArchive.second.release << endl
			<< packageArchive.first << " Architecture   : " << packageArchive.second.arch  << endl
			<< packageArchive.first << " Build date     : " << packageArchive.second.build << endl;
		if (packageArchive.second.dependencies.size() > 0 ) {
			cout << packageArchive.first << " Dependencies   : ";
			for ( auto i : packageArchive.second.dependencies) cout << i.first << i.second << " ";
			cout << endl;
		}
	}
	// Make footprint
	if (m_footprint_mode) {
		getFootprintPackage(m_arg);
	} else {
		// Modes that require the database to be opened
		Db_lock lock(m_root, false);
		getListOfPackageNames(m_root);
		if (m_installed_mode) {
			// List installed packages
			buildDatabaseWithNameVersion();
			for (auto i : m_listOfInstPackages) {
				cout << i.first << " " << i.second.version << "-" << i.second.release << endl;
			}
		} else if (m_list_mode) {
			// List package or file contents
			buildDatabaseWithDetailsInfos(false);
			if (checkPackageNameExist(m_arg)) {
				copy(m_listOfInstPackages[m_arg].files.begin(), m_listOfInstPackages[m_arg].files.end(), ostream_iterator<string>(cout, "\n"));
			} else if (checkFileExist(m_arg)) {
				pair<string, pkginfo_t> package = openArchivePackage(m_arg);
				copy(package.second.files.begin(), package.second.files.end(), ostream_iterator<string>(cout, "\n"));
			} else {
				m_actualError = NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE;
				treatErrors(m_arg);
			}
		} else if (m_runtimedependencies_mode) {
			/* 	Get runtimedependencies of the file found in the directory path
				get the list of installed package silently */
			buildDatabaseWithDetailsInfos(true);
			regex_t r;
			int Result;
			regcomp(&r, ".", REG_EXTENDED | REG_NOSUB);
			set<string>filenameList;
			Result = findRecursiveFile (filenameList, const_cast<char*>(m_arg.c_str()), &r, WS_DEFAULT);
			// get the list of library for all the possible files 
			set<string> librairiesList;
			for (auto i : filenameList) Result = getRuntimeLibrairiesList(librairiesList,i);
			// get the own package  for all the elf files dependencies libraries
#ifndef NDEBUG
			for (auto i : librairiesList) cerr << i <<endl;
#endif
			if ( (librairiesList.size() > 0 ) && (Result > -1) ) {
				set<string> runtimeList;
				for (set<string>::const_iterator i = librairiesList.begin();i != librairiesList.end();++i) {
					for (packages_t::const_iterator j = m_listOfInstPackages.begin(); j != m_listOfInstPackages.end();++j) {
						bool found = false;
						for (set<string>::const_iterator k = j->second.files.begin(); k != j->second.files.end(); ++k) {
							if ( k->find('/' + *i) != string::npos) {
								string dependencie = j->first + static_cast<ostringstream*>( &(ostringstream() <<  j->second.build ))->str();
								runtimeList.insert(dependencie);
								break;
								found = true;
							}
						}
						if ( found == true) {
							found = false;
							break;
						}
					}
				}
				if (runtimeList.size()>0) {
#ifndef NDEBUG
					cerr << "Number of librairies founds: " << runtimeList.size() << endl;
#endif
					unsigned int s = 1;
					for ( auto i : runtimeList ) {
						cout << i << endl;
						s++;
					}
					cout << endl;
				}
			}	
		} else if (m_librairies_mode + m_runtime_mode > 0) {
			// get the list of installed package silently
			buildDatabaseWithDetailsInfos(true);
			set<string> librairiesList;
			int Result = -1;
			if (checkPackageNameExist(m_arg)) {
				for (set<string>::const_iterator i = m_listOfInstPackages[m_arg].files.begin();
					i != m_listOfInstPackages[m_arg].files.end();
					++i){
					string filename('/' + *i);
					Result = getRuntimeLibrairiesList(librairiesList,filename);
				}
				if ( (librairiesList.size() > 0 ) && (Result > -1) ) {
					if (m_runtime_mode) {
						set<string> runtimeList;
						for (set<string>::const_iterator i = librairiesList.begin();
						i != librairiesList.end();
						++i) {
							for (packages_t::const_iterator j = m_listOfInstPackages.begin();
								j != m_listOfInstPackages.end();
								++j){
								bool found = false;
								for (set<string>::const_iterator k = j->second.files.begin();
									k != j->second.files.end();
									++k) {
									if ( k->find('/' + *i) != string::npos) {
										runtimeList.insert(j->first);
										break;
										found = true;
									}
								}
								if (found == true) {
									found = false;
									break;
								}
							}
						}
						if (runtimeList.size()>0) {
							unsigned int s = 1;
							for (set<string>::const_iterator i = runtimeList.begin();
								i!=runtimeList.end();
								++i){
								cout << *i;
								s++;
								if (s <= runtimeList.size())
									cout << ",";
							}
							cout << endl;
						}
					} else {
						for (set<string>::const_iterator i = librairiesList.begin();i != librairiesList.end();++i)
							cout << *i << endl;
					}
				}
			}	
		} else if (m_epoc) {
			// get the building time of the package return 0 if not found
			buildDatabaseWithDetailsInfos(true);
			if (checkPackageNameExist(m_arg)) {
				cout << m_listOfInstPackages[m_arg].build << endl;
			} else {
				cout << "0" << endl;
			}
		} else if (m_details_mode) {
			// get all the details of a package
			buildDatabaseWithDetailsInfos(false);
			if (checkPackageNameExist(m_arg)) {
				char * c_time_s = ctime(&m_listOfInstPackages[m_arg].build);
				cout << "Name           : " << m_arg << endl
					<< "Description    : " << m_listOfInstPackages[m_arg].description << endl
					<< "URL            : " << m_listOfInstPackages[m_arg].url << endl
					<< "Maintainer(s)  : " << m_listOfInstPackages[m_arg].maintainer << endl
					<< "Packager(s)    : " << m_listOfInstPackages[m_arg].packager << endl
					<< "Version        : " << m_listOfInstPackages[m_arg].version << endl
					<< "Release        : " << m_listOfInstPackages[m_arg].release << endl
					<< "Build date     : " << c_time_s
					<< "Size           : " << m_listOfInstPackages[m_arg].size << endl
					<< "Number of Files: " << m_listOfInstPackages[m_arg].files.size()<< endl
					<< "Arch           : " << m_listOfInstPackages[m_arg].arch << endl;
				if ( m_listOfInstPackages[m_arg].dependencies.size() > 0 ) {
					cout << "Dependencies   : ";
					for ( auto i : m_listOfInstPackages[m_arg].dependencies) cout << i.first << " ";
					cout << endl;
				}
			}
		} else if (m_owner_mode) {
			// List owner(s) of file or directory
			buildDatabaseWithDetailsInfos(false);
			regex_t preg;
			if (regcomp(&preg, m_arg.c_str(), REG_EXTENDED | REG_NOSUB)) {
				m_actualError = CANNOT_COMPILE_REGULAR_EXPRESSION;
				treatErrors(m_arg);
			}
			vector<pair<string, string> > result;
			result.push_back(pair<string, string>("Package", "File"));
			unsigned int width = result.begin()->first.length(); // Width of "Package"
#ifndef NDEBUG
			cerr << m_arg << endl;
#endif
			for (auto i : m_listOfInstPackages) {
				for (auto j : i.second.files) {
					const string file('/' + j);
					if (!regexec(&preg, file.c_str(), 0, 0, 0)) {
						result.push_back(pair<string, string>(i.first, j));
						if (i.first.length() > width) {
							width = i.first.length();
						}
					}
				}
			}

			regfree(&preg);
			if (result.size() > 1) {
				for (auto i : result ) {
					cout << left << setw(width + 2) << i.first << i.second << endl;
				}
			} else {
				cout << m_utilName << ": no owner(s) found" << endl;
			}
		}
	}
}
void Pkginfo::finish()
{
}
void Pkginfo::printHelp() const
{
	cout << USAGE << m_utilName << " [options]" << endl
	     << OPTIONS << endl
	     << "  -i, --installed             list installed packages" << endl
         << "  -d, --details               list details about the <package>" << endl
         << "  -L, --librairies            list all the runtime librairies for the <package>" << endl
	     << "  -l, --list <package|file>   list files in <package> or <file>" << endl
	     << "  -o, --owner <pattern>       list owner(s) of file(s) matching <pattern>" << endl
	     << "  -f, --footprint <file>      print footprint for <file>" << endl
         << "  -a, --archive <file>        print Name, Version, Release, BuildDate and Deps of the <file>" << endl
	     << "  -b, --buildtime <package>   return the name and the build time of the package" << endl
         << "  -R, --runtimedep <package>  return on a single line all the runtime dependencies" << endl
         << "  --runtimedepfiles <path>    return on a single line all the runtime dependencies for the files found in the <path>" << endl
	     << "  -r, --root <path>           specify alternative installation root" << endl
	     << "  -v, --version               print version and exit" << endl
	     << "  -h, --help                  print help and exit" << endl;
}
// vim:set ts=2 :
