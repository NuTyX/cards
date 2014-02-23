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
#include "compile_dependencies_utils.h"
#include "cards_depends.h"
#include "config_parser.h"

using namespace std;
int cards_level()
{
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	itemList *filesList = initItemList();
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);

	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		if ( (findFile(filesList,config.prtDir[indCat].c_str())) != 0) {
			return -1;
		}
	}
	for (unsigned int nInd=0;nInd <filesList->count;nInd++) {
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}
	int niveau = generate_level (filesList,packagesList,0);

	if (niveau == 0 ) {
		printf("Problem with genrate_level: %d\n",niveau);
		return -1;
	} else {
		printf("Number of level: %d\n",niveau);
	}
	depList *dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,1) != 0 ) {
		return returnVal;
	}
	int currentNiveau = 0;
	while ( currentNiveau <= niveau) {
		printf("Level: %d\n",currentNiveau);
		for ( unsigned int dInd=0; dInd < dependenciesList->count; dInd++ ) {
			if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == currentNiveau ) {
				printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
			}
		}
		currentNiveau++;
	}
	freeItemList(filesList);
	freePkgInfo(package);
	freePkgList(packagesList);
	return 0;
}
int cards_depinst(const char* packageName)
{
	pkgList *packagesList = initPkgList();

	itemList *filesList = initItemList();

	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);

	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		if ( (findFile(filesList,config.prtDir[indCat].c_str())) != 0) {
			return -1;
		}
	}

	char * longPackageName = NULL;
	if ( (longPackageName = getLongPackageName(filesList,packageName)) == NULL) {
		cout << "The package '" << packageName << "' is not found" << endl;
		return -1;
	}

	printf("%s\n",longPackageName);
	freePkgList(packagesList);
	freeItemList(filesList);
	return 0;
}
int cards_depends(const char* packageName)
{
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	itemList *filesList = initItemList();
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		if ( (findFile(filesList,config.prtDir[indCat].c_str())) != 0) {
				return -1;
		}
	}
	char * longPackageName = NULL;
	if ( (longPackageName = getLongPackageName(filesList,packageName)) == NULL) {
		cout << "The package '" << packageName << "' is not found" << endl;
		return -1;
	}
	for (unsigned int nInd=0;nInd <filesList->count;nInd++){
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}
	int niveau = generate_level (filesList,packagesList,0);

	if (niveau == 0 ) {
		printf("Problem with genrate_level: %d\n",niveau);
		return -1;
	}
/*	printf("Number of niveaux: %d\n",niveau);

		for (unsigned int nInd = 0; nInd<packagesList->count;nInd++){
			printf("%s has %d deps:\n",filesList->items[nInd], packagesList->pkgs[nInd]->dependences->count);
			for(unsigned int dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++) {
				printf("%d) %s\n", dInd + 1,
					filesList->items[packagesList->pkgs[nInd]->dependences->depsIndex[dInd]]);
			}
		} */
		depList *dependenciesList = initDepsList();
		if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,longPackageName,1) != 0 ) {
			return returnVal;
		}
		if (dependenciesList ->count > 0) {
			int currentNiveau = 0;
			while ( currentNiveau <= niveau) {
				printf("Level: %d\n",currentNiveau);
				for ( unsigned int dInd=0; dInd < dependenciesList->count; dInd++ ) {
					if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == currentNiveau ) {
						printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
					}
				}
				currentNiveau++;
			}
			
		}
	freeItemList(filesList);
	freePkgInfo(package);
	freePkgList(packagesList);
	free(longPackageName);
	return 0;
}
int cards_deptree(const char* packageName)
{
	itemList *filesList = initItemList();

	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	depList *dependenciesList = NULL;
	dependenciesList = initDepsList();

	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);

	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		if ( (findFile(filesList,config.prtDir[indCat].c_str())) != 0) {
			return -1;
		}
	}

	char * longPackageName = NULL;
	if ( (longPackageName = getLongPackageName(filesList,packageName)) == NULL) {
		cout << "The package '" << packageName << "' is not found" << endl;
		return -1;
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
	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		string category = basename(const_cast<char*>(config.prtDir[indCat].c_str()));
		string remoteUrl = config.Url + "/" + category;
		DIR *d;
		struct dirent *dir;
		d = opendir(config.prtDir[indCat].c_str());
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				if ( strcmp (dir->d_name, ".") && strcmp (dir->d_name, "..") ) {
					localPackagesList.insert(config.prtDir[indCat] + "/" + dir->d_name);
					string dirName = dir->d_name;
					name = dirName;
					unsigned int pos =  dirName.find('_');
					if ( pos != std::string::npos) {
						name = dirName.substr(0,pos);
					}
					if (! strcmp (packageName,name.c_str())) {
						found=true;
						string depFile = config.prtDir[indCat] 
								+ "/" + dir->d_name + "/" + name + ".deps";
						if (checkFileExist(depFile)) {
								if (parseFile(depsPackagesList,depFile.c_str()) != 0 ) {
									cerr << "Cannot read " <<  depFile  << endl;
									return -1;
								}
						} else {
							FileDownload DepsPort(remoteUrl + "/" + dirName  + "/" + name + ".deps",
                    config.prtDir[indCat] + "/" + dirName  ,
                    name + ".deps",
                    false);	
							if ( DepsPort.downloadFile() != 0) {
								cerr << "Failed to download " 
              	  << depFile << endl ;
            		return -1;
							} else if (parseFile(depsPackagesList,depFile.c_str()) != 0 ) {
								cerr << "Cannot read " <<  depFile  << endl;
								return -1;
							}
						}
					}
				}
			}
		}
		closedir(d);
		if (!found) {
			cerr << "Cannot find " << packageName << endl;
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
