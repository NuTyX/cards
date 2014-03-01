// cards_install.cc
//
// Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)

#include "file_download.h"
#include "file_utils.h"
#include "pkgadd.h"

#include "compile_dependencies_utils.h"
#include "cards_depends.h"
#include "cards_install.h"
#include "config_parser.h"

int cards_install(const char* packageName)
{
  pkgList *packagesList = initPkgList();

  itemList *filesList = initItemList();
	itemList *packageFilesList = initItemList();

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
#ifndef NDEBUG
  printf("%s\n",longPackageName);
#endif

	// Let's go to the directory

	chdir(longPackageName);

	// Build the category and the url
	string packageNameVersion = basename(longPackageName);
	string category = dirname(longPackageName);
	category = basename(const_cast<char*>(category.c_str()));
	
	string url = config.Url + "/" + category + "/" + packageNameVersion + "/" ;

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
