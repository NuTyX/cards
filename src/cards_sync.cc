// cards_sync.cc
//
//  Copyright (c) 2002-2005 Johannes Winkelmann jw at tks6 dot net 
//  Copyright (c) 2014-2015 by NuTyX team (http://nutyx.org)
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <cstring>
#include <cstdlib>

#include "file_utils.h"
#include "file_download.h"
#include "pkgrepo.h"
#include "cards_sync.h"

using namespace std;

const string CardsSync::DEFAULT_REPOFILE = ".PKGREPO";

CardsSync::CardsSync (const CardsArgumentParser& argParser)
	: m_argParser(argParser)
{
	m_repoFile = DEFAULT_REPOFILE;
}
CardsSync::CardsSync ( const CardsArgumentParser& argParser,
		const string& url, const string& baseDirectory,
		const string& repoFile)
		: m_baseDirectory(baseDirectory),
		m_remoteUrl(url),
		m_argParser(argParser)
{
	if (repoFile != "") {
		m_repoFile = repoFile;
	} else {
		m_repoFile = DEFAULT_REPOFILE;
	}
}
void CardsSync::treatErrors(const string& s) const
{
	switch ( m_actualError )
	{
		case CANNOT_FIND_FILE:
		case CANNOT_PARSE_FILE:
		case CANNOT_WRITE_FILE:
		case CANNOT_SYNCHRONIZE:
		case CANNOT_COPY_FILE:
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
		case LISTED_FILES_ALLREADY_INSTALLED:
		case PKGADD_CONFIG_LINE_TOO_LONG:
		case PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS:
		case PKGADD_CONFIG_UNKNOWN_ACTION:
		case PKGADD_CONFIG_UNKNOWN_EVENT:
		case CANNOT_COMPILE_REGULAR_EXPRESSION:
		case NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE:
		case PACKAGE_NOT_FOUND:
		case PACKAGE_ALLREADY_INSTALL:
		case PACKAGE_NOT_INSTALL:
		case PACKAGE_NOT_PREVIOUSLY_INSTALL:
		case CANNOT_GENERATE_LEVEL:
		case CANNOT_CREATE_FILE:
			break;
		case CANNOT_DOWNLOAD_FILE:
			throw runtime_error("could not download " + s);
			break;
		case CANNOT_OPEN_FILE:
			throw RunTimeErrorWithErrno("could not open " + s);
			break;
		case CANNOT_READ_FILE:
			throw runtime_error("could not read " + s);
			break;
		case CANNOT_READ_DIRECTORY:
			throw RunTimeErrorWithErrno("could not read directory " + s);
			break;
		case CANNOT_CREATE_DIRECTORY:
			throw RunTimeErrorWithErrno("could not create directory " + s);
			break;
		case ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
			throw runtime_error(s + " only root can install / sync / purge / upgrade / remove packages");
			break;
	}
}
unsigned int CardsSync::getLocalPackages(const string& path)
{
	m_localPackagesList.clear();
	if ( findFile( m_localPackagesList, path) != 0 ) {
		m_actualError = CANNOT_READ_DIRECTORY;
		treatErrors(path);
	}
	return m_localPackagesList.size();
}
unsigned int CardsSync::getRemotePackages(const string& pkgrepoFile)
{
	m_remotePackagesList.clear();
	if ( parseFile(m_remotePackagesList,pkgrepoFile.c_str()) != 0) {
		m_actualError = CANNOT_READ_FILE;
		treatErrors(pkgrepoFile);
	}
	return m_remotePackagesList.size();
}
void CardsSync::run()
{
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);
	for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end(); ++i) {
		DirUrl DU = *i ;
		if (DU.Url.size() == 0 ) {
			continue;
		}
		string categoryDir, url ;
		categoryDir = DU.Dir;
		url = DU.Url;
		string category = basename(const_cast<char*>(categoryDir.c_str()));
		string categoryPKGREPOFile = categoryDir + "/" + m_repoFile ;
		FileDownload PKGRepo(url + "/" + m_repoFile,
			categoryDir,
			m_repoFile, false);
		PKGRepo.downloadFile();
	}
}
void CardsSync::purge()
{
	if (getuid()) {
			m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
			treatErrors("");
	}
	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);

	for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end(); ++i) {
		if ( i->Url.size() == 0)
			continue;
		if ( getLocalPackages(i->Dir) == 0 )
			continue;
		for (set<string>::const_iterator dir = m_localPackagesList.begin(); dir != m_localPackagesList.end(); ++dir) {
			deleteFolder(i->Dir + "/" + *dir);
		}
	}
}
void CardsSync::deleteFolder(const string& folderName)
{
	set<string> filesToDelete;
	if ( findFile(filesToDelete, folderName) != 0 ){
			m_actualError = CANNOT_READ_FILE;
			treatErrors(folderName);
	}
	for (set<string>::const_iterator f = filesToDelete.begin(); f != filesToDelete.end(); f++) {
		string fileName = folderName + "/" + *f;
		if (f->size() > 0)
			cout << "Deleting " << fileName << endl; 
		removeFile("/",fileName);
	}	
	removeFile("/",folderName);
}
// vim:set ts=2 :
