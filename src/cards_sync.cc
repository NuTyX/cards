// cards_sync.cc
//
//  Copyright (c) 2002-2005 Johannes Winkelmann jw at tks6 dot net 
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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
#include "config_parser.h"
#include "cards_sync.h"

using namespace std;

const string CardsSync::DEFAULT_REPOFILE = "MD5SUM";

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
			throw runtime_error(s + " only root can install / sync / upgrade / remove packages");
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
unsigned int CardsSync::getRemotePackages(const string& md5sumFile)
{
	m_remotePackagesList.clear();
	if ( parseFile(m_remotePackagesList,md5sumFile.c_str()) != 0) {
		m_actualError = CANNOT_READ_FILE;
		treatErrors(md5sumFile);
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
	ConfigParser::parseConfig("/etc/cards.conf", config);
	InfoFile downloadFile;
	string localPackageDirectory,remotePackageDirectory ;

	for (vector<string>::iterator i = config.dirUrl.begin();i != config.dirUrl.end(); ++i) {
		string val = *i ;
		string categoryDir, url ;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			categoryDir = stripWhiteSpace(val.substr(0,pos));
			url = stripWhiteSpace(val.substr(pos+1));
		} else {
			continue;
		}
		string category = basename(const_cast<char*>(categoryDir.c_str()));
		string categoryMD5sumFile = categoryDir + "/" + m_repoFile ;
		cout << "Synchronizing " << categoryDir << " from " << url << endl;
		categoryDir = categoryDir + "/";
		// Get the MD5SUM file of the category
		FileDownload MD5Sum(url + "/" + m_repoFile,
			categoryDir,
			m_repoFile, false);
		MD5Sum.downloadFile();
		// We read the category MD5SUM file
		getRemotePackages(categoryMD5sumFile);
		// and need to remove it
		remove(categoryMD5sumFile.c_str());
		if ( m_remotePackagesList.size() == 0 ) {
			cout << "no ports founds ..." << endl;
			continue;
		}
		// Check what we have so far
		getLocalPackages(categoryDir);
		for (set<string>::const_iterator li = m_localPackagesList.begin(); li != m_localPackagesList.end(); ++li) {
			bool found = false;
			for (set<string>::const_iterator ri = m_remotePackagesList.begin(); ri != m_remotePackagesList.end(); ++ri) {
				localPackageDirectory = *li;
				remotePackageDirectory = *ri;
				if ( remotePackageDirectory.size() < 34 ) {
					m_actualError = CANNOT_READ_FILE;
					treatErrors(remotePackageDirectory + "  missing info ...");
				}
				if ( remotePackageDirectory[32] != ':' ) {
					m_actualError = CANNOT_READ_FILE;
					treatErrors( remotePackageDirectory + " wrong format... ");
				}
				// Check if the local Package is still in the depot by comparing the directories
				if ( localPackageDirectory == remotePackageDirectory.substr(33)) {
					found = true ;
					break;	// found check the next local one
				}
			}
			if ( ! found ) {
				deleteFolder(categoryDir + localPackageDirectory);
			}
		}

		// We check again the local one because some are delete maybe
		getLocalPackages(categoryDir);		
		vector<InfoFile> downloadFilesList;
		// Checking for new ports
		for (set<string>::const_iterator ri = m_remotePackagesList.begin(); ri != m_remotePackagesList.end(); ++ri) {
			string remotePackageMD5SUMDirectory = *ri;
			remotePackageDirectory = remotePackageMD5SUMDirectory.substr(33) + "/";
			downloadFile.url = url + "/" + remotePackageDirectory + m_repoFile;
			downloadFile.dirname = categoryDir + remotePackageDirectory;
			downloadFile.filename = m_repoFile;
			downloadFile.md5sum = remotePackageMD5SUMDirectory.substr(0,32);
			bool MD5SUMfound = false;
			
			for (set<string>::const_iterator li = m_localPackagesList.begin(); li != m_localPackagesList.end(); ++li) {
				localPackageDirectory = *li + "/";
				if ( remotePackageDirectory == localPackageDirectory ) {
					string downloadFileName = downloadFile.dirname + m_repoFile;
					if ( checkFileExist(downloadFile.dirname + m_repoFile)) {
						// is it still uptodate
						if ( checkMD5sum(downloadFileName.c_str(),downloadFile.md5sum.c_str() ) ) {
							MD5SUMfound = true;
							break;
						}
					}
				}
			}
			if ( ! MD5SUMfound ) {
				downloadFilesList.push_back(downloadFile);
			}
		}
		if ( downloadFilesList.size() > 0 ) {
			FileDownload FD(downloadFilesList,false);
		}		
		// We check again the local one because maybe they was no one
		getLocalPackages(categoryDir);	
		downloadFilesList.clear();
		/*
		*  From here on we should have all the MD5SUM files, by default if no parameters are given
		* We just want to download the info file, let's check what's available in every folder by
		* parsing the MD5SUM file
		*/
		for (set<string>::const_iterator ri = m_localPackagesList.begin(); ri != m_localPackagesList.end(); ++ri) {
			string localPackageDirectory = *ri;
			string MD5SUMFile = categoryDir+localPackageDirectory + "/" + m_repoFile;
			string packageName = *ri;
			string::size_type pos = localPackageDirectory.find('@');
			
			if ( pos != std::string::npos) {
				packageName = localPackageDirectory.substr(0,pos);
			}
			set<string> MD5SUMFileContent;
			if ( parseFile(MD5SUMFileContent,MD5SUMFile.c_str()) != 0) {
				m_actualError = CANNOT_READ_FILE;
				treatErrors(MD5SUMFile);
			}
			for (set<string>::const_iterator i = MD5SUMFileContent.begin(); i != MD5SUMFileContent.end(); ++i) {
				string input = *i;
				/* 
					By default we dont want to know about version and extension of the binary,
					if the size of the line is small then 32 + 1 caracters, it's the firt line.
					Let's skip it
				*/
				if (input.size() < 11)
					continue;

				if (input[10] == ':')
					continue;

				downloadFile.dirname = categoryDir + localPackageDirectory +"/";
				downloadFile.md5sum = input.substr(0,32);
				if (input.substr(33) == packageName + ".info" ) {
					downloadFile.url = url + "/" + localPackageDirectory +"/"+ packageName + ".info";
					downloadFile.filename = packageName + ".info";
					string downloadFileName = downloadFile.dirname + packageName + ".info";
					if (! checkFileExist(downloadFile.dirname + downloadFile.filename)) {
						downloadFilesList.push_back(downloadFile);
					} else if ( ! checkMD5sum(downloadFileName.c_str(),downloadFile.md5sum.c_str()) ) {
						downloadFilesList.push_back(downloadFile);
					}
				}
			}
		}
		if ( (m_argParser.isSet(CardsArgumentParser::OPT_PACKAGEFILES)) || 
			(m_argParser.isSet(CardsArgumentParser::OPT_SYNCALL)) ) {
			/*
				If pkgfile is pass as option, we want to download everything
				concerning the build of a package. Download everything except
				the binaries
			*/	
			for (set<string>::const_iterator ri = m_localPackagesList.begin(); ri != m_localPackagesList.end(); ++ri) {
				string localPackageDirectory = *ri;
				string MD5SUMFile = categoryDir+localPackageDirectory + "/" + m_repoFile;
				string packageName = *ri;
				string::size_type pos = localPackageDirectory.find('@');
				if ( pos != std::string::npos) {
					packageName = localPackageDirectory.substr(0,pos);
				}
				set<string> MD5SUMFileContent;
				if ( parseFile(MD5SUMFileContent,MD5SUMFile.c_str()) != 0) {
					m_actualError = CANNOT_READ_FILE;
					treatErrors(MD5SUMFile);
				}
				for (set<string>::const_iterator i = MD5SUMFileContent.begin(); i != MD5SUMFileContent.end(); ++i) {
					string input = *i;
					if (input.size() < 11)
						continue;
					if (input[10] == ':' ) {
						continue;
					}
					downloadFile.dirname = categoryDir + localPackageDirectory +"/";
					downloadFile.md5sum = input.substr(0,32);

					string fileName = input.substr(33);
					/*
						If they is no extra separate column it's no a binary so
						if not present or wrong MD5SUM, we get it.
					*/
					string::size_type pos = fileName.find(':');
					if ( pos == std::string::npos) {
						downloadFile.url = url + "/" + localPackageDirectory + "/" + fileName;
						downloadFile.filename = fileName;
						string downloadFileName = downloadFile.dirname + fileName;
						if (! checkFileExist(downloadFile.dirname + downloadFile.filename)) {
							downloadFilesList.push_back(downloadFile);
						} else if ( ! checkMD5sum(downloadFileName.c_str(),downloadFile.md5sum.c_str()) ) {
							downloadFilesList.push_back(downloadFile);
						}
					}
				}
			}
		}
		if ( downloadFilesList.size() > 0 ) {
			FileDownload FD(downloadFilesList,false);
		}
		downloadFilesList.clear();
		if (m_argParser.isSet(CardsArgumentParser::OPT_SYNCALL)) {
			/*
				If all is pass we want to download the binaries as well
			*/
			for (set<string>::const_iterator ri = m_localPackagesList.begin(); ri != m_localPackagesList.end(); ++ri) {
				bool found = false;
				string localPackageDirectory = *ri;
				string MD5SUMFile = categoryDir+localPackageDirectory + "/" + m_repoFile;
				string packageName = *ri;
				string::size_type pos = localPackageDirectory.find('@');
				if ( pos != std::string::npos) {
					packageName = localPackageDirectory.substr(0,pos);
				}
				set<string> MD5SUMFileContent;
				if ( parseFile(MD5SUMFileContent,MD5SUMFile.c_str()) != 0) {
					m_actualError = CANNOT_READ_FILE;
					treatErrors(MD5SUMFile);
				}
				string packageExtension = "";
				string packageBuildDate = "";
				for (set<string>::const_iterator i = MD5SUMFileContent.begin();i != MD5SUMFileContent.end(); ++i) {
					string input = *i;
					if (input.size() < 11)
						continue;
					if (input[10] == ':' ) {
						packageExtension = input.substr(11,input.size());
						packageBuildDate = input.substr(0,10);
						found = true;
						continue;
					}
					if (found) {	// This port have binaries available
						downloadFile.dirname = categoryDir + localPackageDirectory +"/";
						downloadFile.md5sum = input.substr(0,32);
						string fileName = input.substr(33);
						string::size_type pos = fileName.find(':');
						if ( pos != std::string::npos) {
							string fileName = input.substr(33,pos) +
								packageBuildDate +
								input.substr(34+pos) +
								packageExtension;
							downloadFile.url = url + "/" + localPackageDirectory + "/" + fileName;
							downloadFile.filename = fileName;
							string downloadFileName = downloadFile.dirname + fileName;
							if (! checkFileExist(downloadFile.dirname + downloadFile.filename)) {
								downloadFilesList.push_back(downloadFile);
							} else if ( ! checkMD5sum(downloadFileName.c_str(),downloadFile.md5sum.c_str()) ) {
									downloadFilesList.push_back(downloadFile);
							}
						}
					}
				}
			}
			if ( downloadFilesList.size() > 0 ) {
				FileDownload FD(downloadFilesList,true);
			}
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
		cout << "Deleting " << fileName << endl; 
		removeFile("/",fileName);
	}
		cout << "Deleting " << folderName << endl;
		removeFile("/",folderName);
}
// vim:set ts=2 :
