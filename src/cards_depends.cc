//  cards_depends.cc
//
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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

#include <iostream>
#include <cstdlib>
#include <set>
#include <string>

#include "file_download.h"
#include "file_utils.h"
#include "pkginfo.h"
#include "compile_dependencies_utils.h"
#include "cards_depends.h"
#include "config_parser.h"

using namespace std;


vector<string> CardsDepends::getdependencies()
{
	depends();
#ifndef NDEBUG
	for (std::vector<string>::iterator i = m_dependenciesList.begin();i !=  m_dependenciesList.end();++i) {
		cerr << *i << endl;
	}
#endif
	return m_dependenciesList;
}
void CardsDepends::showdependencies()
{
	depends();
	for (std::vector<string>::iterator it = m_dependenciesList.begin();it != m_dependenciesList.end();it++) {
		cout << *it << endl;
	}
}
void CardsDepends::treatErrors(const string& s) const
{
	switch ( m_actualError )
	{
		case CANNOT_CREATE_DIRECTORY:
		case CANNOT_CREATE_FILE:
		case CANNOT_WRITE_FILE:
		case CANNOT_SYNCHRONIZE:
		case CANNOT_RENAME_FILE:
		case CANNOT_DETERMINE_NAME_BUILDNR:
		case EMPTY_PACKAGE:
		case CANNOT_FORK:
		case WAIT_PID_FAILED:
		case DATABASE_LOCKED:
		case CANNOT_LOCK_DIRECTORY:
		case CANNOT_REMOVE_FILE:
		case CANNOT_RENAME_DIRECTORY:
		case OPTION_ONE_ARGUMENT:
		case INVALID_OPTION:
		case OPTION_MISSING:
		case TOO_MANY_OPTIONS:
		case ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
		case PACKAGE_ALLREADY_INSTALL:
		case PACKAGE_NOT_INSTALL:
		case PACKAGE_NOT_PREVIOUSLY_INSTALL:
		case LISTED_FILES_ALLREADY_INSTALLED:
		case PKGADD_CONFIG_LINE_TOO_LONG:
		case PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS:
		case PKGADD_CONFIG_UNKNOWN_ACTION:
		case PKGADD_CONFIG_UNKNOWN_EVENT:
		case CANNOT_COMPILE_REGULAR_EXPRESSION:
		case NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE:
		case CANNOT_OPEN_FILE:
		case CANNOT_FIND_FILE:
		case CANNOT_PARSE_FILE:
			break;
		case CANNOT_DOWNLOAD_FILE:
			throw runtime_error("could not download " + s);
			break;
		case CANNOT_READ_FILE:
			throw runtime_error("could not read " + s);
			break;
		case CANNOT_READ_DIRECTORY:
			throw RunTimeErrorWithErrno("could not read directory " + s);
			break;
		case PACKAGE_NOT_FOUND:
			throw runtime_error("package " + s + " not found");
			break;
		case CANNOT_GENERATE_LEVEL:
			throw runtime_error("cannot generate the levels" + s);
			break;
	}
}
int CardsDepends::level()
{
	cerr << "Generate Level, please wait ... " << endl;
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	itemList *filesList = initItemList();
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
  for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
		DirUrl DU  = *i ;
		string prtDir = DU.Dir;
		if ( (findDir(filesList,prtDir.c_str())) != 0) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
#ifndef NDEBUG
		cerr << i->Dir << " " << i->Url  << endl;
#endif
	}
#ifndef NDEBUG
	cerr << filesList->count << endl;
#endif
	for (unsigned int nInd=0;nInd <filesList->count;nInd++) {
#ifndef NDEBUG
		cerr << nInd << " " << filesList->items[nInd] << endl;
#endif
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}
	int niveau = generate_level (filesList,packagesList,0);
	cerr << "Level done " << endl;
	if (niveau == 0 ) {
		m_actualError = CANNOT_GENERATE_LEVEL;
		treatErrors("0");
	} else {
#ifndef NDEBUG
		cerr << "Number of level: " << niveau << endl;
#endif
	}
	depList *dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,1) != 0 ) {
		return returnVal;
	}
	int currentNiveau = 0;
	while ( currentNiveau <= niveau) {
		for ( unsigned int nameIndex = 0; nameIndex < packagesList -> count; nameIndex++ ) {
			if ( packagesList -> pkgs[nameIndex]->niveau == currentNiveau ) {
				printf("%d: %s \n",currentNiveau,filesList->items[nameIndex]);
			}
		}
		currentNiveau++;
	}
	freeItemList(filesList);
	freePkgInfo(package);
	freePkgList(packagesList);
	return 0;
}
int CardsDepends::depends()
{
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();
	Pkginfo * packagesInfo = new Pkginfo;
	packagesInfo->getNumberOfPackages();
	itemList *filesList = initItemList();
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
  for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
    DirUrl DU = *i;
    string prtDir = DU.Dir;
		if ( (findDir(filesList,prtDir.c_str())) != 0) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
	}
	char * longPackageName = NULL;
	if ( (longPackageName = getLongPackageName(filesList,m_packageName)) == NULL) {
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(m_packageName);
	}
	for (unsigned int nInd=0;nInd <filesList->count;nInd++){
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}
	int niveau = generate_level (filesList,packagesList,0);
	if (niveau == 0 ) {
		m_actualError = CANNOT_GENERATE_LEVEL;
		treatErrors("0");
	}
	depList *dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,longPackageName,1) != 0 ) {
		return returnVal;
	}
	if (dependenciesList ->count > 0) {
		int currentNiveau = 0;
		itemList *sortPackagesList = initItemList(); // We need to get read of the duplicated found packages
		while ( currentNiveau <= niveau) {
#ifndef NDEBUG
			printf("Level: %d\n",currentNiveau);
#endif
			for ( unsigned int dInd=0; dInd < dependenciesList->count; dInd++ ) {
				if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == currentNiveau ) {
					bool found = false;
					for (unsigned int j = 0; j< sortPackagesList->count ;++j) {
						if (strcmp ( filesList->items[dependenciesList->depsIndex[dInd]] , sortPackagesList->items[j]) == 0) {
							found = true;
							break;
						}
					}
					if ( ! found ) { // if not allready found
						addItemToItemList(sortPackagesList,filesList->items[dependenciesList->depsIndex[dInd]]);
					}
				}
			}
			currentNiveau++;
		}
		for (unsigned int i = 0; i < sortPackagesList-> count;++i) {
			string packageName = basename(sortPackagesList->items[i]);
			string name(packageName,0,packageName.find('@'));
			if (m_argParser.isSet(CardsArgumentParser::OPT_ALL)) {
				printf("%s\n",sortPackagesList-> items[i]);
			} else {
				if ( ! packagesInfo->isInstalled(name.c_str())) {
					m_dependenciesList.push_back(sortPackagesList-> items[i]);
				}
			}
		}
		m_dependenciesList.push_back(longPackageName);
		freeItemList(sortPackagesList);
	}
	freeItemList(filesList);
	freePkgList(packagesList);
	freePkgInfo(package);
	free(longPackageName);
	delete packagesInfo;
	return 0;
}
int CardsDepends::deptree()
{
	itemList *filesList = initItemList();

	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	depList *dependenciesList = NULL;
	dependenciesList = initDepsList();

	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
	for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
		DirUrl DU  = *i ;
		string prtDir = DU.Dir;
		if ( (findDir(filesList,prtDir.c_str())) != 0) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
	}
	char * longPackageName = NULL;
	if ( (longPackageName = getLongPackageName(filesList,m_packageName)) == NULL) {
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(m_packageName);
	}

	for (unsigned int nInd=0;nInd <filesList->count;nInd++) {
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}

	if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,longPackageName,1) != 0 ) {
		return returnVal;
	}

	if (dependenciesList ->count > 0) {
		printf("0) %s \n",basename(longPackageName));
		for (unsigned int dInd=0; dInd < dependenciesList ->count; dInd++) {
			printf("  ");
			int j=1;
			while ( j < dependenciesList->niveau[dInd]) {
				printf("  ");
				j++;
			}
			printf("%d) %s\n",dependenciesList->niveau[dInd],basename(filesList->items[dependenciesList->depsIndex[dInd]]));
		}
	}
	
	bool found=false;
	string name = "";
	set<string> localPackagesList, depsPackagesList;

  for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
    DirUrl DU = *i ;
		if ( DU.Url == "") {
			continue;
		}
    string prtDir, Url ;
    prtDir = DU.Dir;
    Url = DU.Url;
//	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		string category = basename(const_cast<char*>(prtDir.c_str()));
		string remoteUrl = Url + "/" + category;
		DIR *d;
		struct dirent *dir;
		d = opendir(prtDir.c_str());
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				if ( strcmp (dir->d_name, ".") && strcmp (dir->d_name, "..") ) {
					localPackagesList.insert(prtDir + "/" + dir->d_name);
					string dirName = dir->d_name;
					name = dirName;
					string::size_type pos =  dirName.find('@');
					if ( pos != std::string::npos) {
						name = dirName.substr(0,pos);
					}
					if (! strcmp (m_packageName,name.c_str())) {
						found=true;
						string depFile = prtDir
								+ "/" + dir->d_name + "/" + name + ".deps";
						if (checkFileExist(depFile)) {
								if (parseFile(depsPackagesList,depFile.c_str()) != 0 ) {
									m_actualError = CANNOT_READ_FILE;
									treatErrors(depFile);
								}
						} else {
							FileDownload DepsPort(remoteUrl + "/" + dirName  + "/" + name + ".deps",
                    prtDir + "/" + dirName  ,
                    name + ".deps",
                    false);	
							DepsPort.downloadFile();
							if (parseFile(depsPackagesList,depFile.c_str()) != 0 ) {
								m_actualError = CANNOT_READ_FILE;
								treatErrors(depFile);
							}
						}
					}
				}
			}
		}
		closedir(d);
		if (!found) {
			cerr << "Cannot find " << m_packageName << endl;
			return -1;
		}
	}
	if (localPackagesList.size() == 0 ) {
		cout << "You need to 'cards sync first" << endl;
		return -1;
	}
/*	freeItemList(filesList);
	freePkgInfo(package);
	freePkgList(packagesList);
	*/
	return 0;
}
// vim:set ts=2 :
