// cards_install.cc
//
// Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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
#include "file_download.h"
#include "file_utils.h"
#include "pkgadd.h"
#include "pkginfo.h"

#include "compile_dependencies_utils.h"
#include "cards_depends.h"
#include "cards_install.h"
#include "config_parser.h"

int cards_install(const char* packageName)
{
	Pkginfo * packagesInfo = new Pkginfo ;
	if (packagesInfo->isInstalled(packageName) ) {
		delete packagesInfo;
		cout << "the package '" << packageName << "' is allready installed" << endl;
		return 0;
	}
	delete packagesInfo;
  pkgList *packagesList = initPkgList();

  itemList *filesList = initItemList();
	itemList *packageFilesList = initItemList();

  Config config;
  ConfigParser::parseConfig("/etc/cards.conf", config);

	string Url = "";
	for (map<string,string>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {

		string prtDir = i -> first;
		Url = i -> second;
//  for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
    if ( (findFile(filesList,prtDir.c_str())) != 0) {
      return -1;
    }
  }

  char * longPackageName = NULL;
  if ( (longPackageName = getLongPackageName(filesList,packageName)) == NULL) {
    cout << "The package '" << packageName << "' is not found" << endl;
    return -1;
  }
#ifndef NDEBUG
  printf("%s\n",longPackageName);
#endif

	// Let's go to the directory

	chdir(longPackageName);

	// Build the category and the url
	string packageNameVersion = basename(longPackageName);
	string category = dirname(longPackageName);
	category = basename(const_cast<char*>(category.c_str()));
	
	string url = Url + "/" + category + "/" + packageNameVersion + "/" ;

#ifndef NDEBUG
	cout << category << endl;
	cout << url << endl;
#endif

	if (! checkFileExist("MD5SUM") ) {
		FileDownload md5sumFile(url+"MD5SUM","./","MD5SUM",false);
		if (md5sumFile.downloadFile() != 0 ) {
			cerr << "MD5SUM not exist" << endl;
			return -1;
		}
	}
	if ( readFile ( packageFilesList,"MD5SUM" ) != 0 ) {
		cerr << "Error while reading MD5SUM file" << endl;
		return -1;
	}
	string packageFileName = packageNameVersion + "-x86_64.cards.tar.xz";
	for (unsigned int i = 0 ; i < packageFilesList->count;i++) {
		string input = packageFilesList->items[i];
		string file = input.substr(33);
		string md5S = input.substr(0,32);
		if ( file == packageFileName ) {
#ifndef NDEBUG
			cout << file  << endl;
#endif
			if (!checkFileExist(file)) {
				cout << "Downloading "<<  file << "..." << endl;
				FileDownload packageArchive(url+file,"./",file,md5S,true);
				if (packageArchive.downloadFile() != 0 ) {
					cerr << "Failed to download " + file << endl;
					return -1;
				}
			}
			Pkgadd * pkgToAdd = new Pkgadd;
			itemList * commandList = initItemList();
			addItemToItemList(commandList,"pkgadd");
			addItemToItemList(commandList, file.c_str());
			pkgToAdd->run(commandList->count,commandList->items);
			freeItemList(commandList);
			delete pkgToAdd;
			break;
			
		}
	}
	
  freePkgList(packagesList);
	freeItemList(packageFilesList);
  freeItemList(filesList);
  return 0;
}

// vim:set ts=2 :
