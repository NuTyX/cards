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
const int CardsSync::DEFAULT_TIMEOUT = 60;

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
int CardsSync::run()
{
	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	struct stat info;
	if (stat(m_baseDirectory.c_str(), &info)) {
		if (createRecursiveDirs(m_baseDirectory)) {
			m_actualError = CANNOT_CREATE_DIRECTORY;
			treatErrors(m_baseDirectory);
		}
	}
	
	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
	for (vector<string>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
		string val = *i ;
		string prtDir, url ;
		string::size_type pos = val.find('|');
		if (pos != string::npos) {
			prtDir = stripWhiteSpace(val.substr(0,pos));
			url = stripWhiteSpace(val.substr(pos+1));
		} else {
			continue;
		}
		string category = basename(const_cast<char*>(prtDir.c_str()));
		cout << "Synchronising " << category << endl;
		string remoteUrl = url  ;
		string categoryMD5sumFile = prtDir + "/" + m_repoFile ;

		// Get the MD5SUM file of the category
		FileDownload MD5Sum(remoteUrl + "/" + m_repoFile,
			prtDir,
			m_repoFile, false);
		if ( MD5Sum.downloadFile () != 0) {
			m_actualError = CANNOT_DOWNLOAD_FILE;
    	treatErrors(remoteUrl + "/" + m_repoFile);
		}
		set<string> localPackagesList,remotePackagesList,remoteFilesList;

		if ( parseFile(remotePackagesList,categoryMD5sumFile.c_str()) != 0) {
			m_actualError = CANNOT_READ_FILE;
			treatErrors(categoryMD5sumFile);
		}
		// We need to remove the MD5SUM file
		remove(categoryMD5sumFile.c_str());
		// Let see what we have so for locally
		if ( findFile( localPackagesList, prtDir) != 0 ) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
		// If they are some ports availables
		if ( remotePackagesList.size() > 0 ) {
			// If no directories found
			if (localPackagesList.size() == 0 ) {
				for (set<string>::const_iterator i = remotePackagesList.begin(); i != remotePackagesList.end(); i++) {
					string input = *i;
					if ( input.size() < 33) {
						continue;
					}
					string dir = input.substr(33);
					cout << "d: "  << prtDir << "/" << dir << endl;
					string path = prtDir + "/" + dir;
					createRecursiveDirs(prtDir + "/" + dir);
					}
			} else {
				// some directories found, Only download/remove/change the one needs
				string localPackage,remotePackage ;
				// obsolete ?
				for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); li++) {
					bool found = false;
					for (set<string>::const_iterator ri = remotePackagesList.begin(); ri != remotePackagesList.end(); ri++) {
						localPackage = *li;
						remotePackage = *ri;
						if ( localPackage == remotePackage.substr(33)) {	
							found = true;
							break;
						}
					}
					if ( ! found ) {
						cout << "Deleting " << localPackage << endl;
						
						set<string> filesToDelete;
						if ( findFile(filesToDelete, prtDir + "/"+ localPackage) != 0 ){
							m_actualError = CANNOT_READ_FILE;
							treatErrors(prtDir + "/"+ localPackage);
						}
						for (set<string>::const_iterator f = filesToDelete.begin(); f != filesToDelete.end(); f++) {
							removeFile("/",prtDir + "/"+ localPackage + "/"+ *f);
						}
						removeFile("/",prtDir + "/"+localPackage);
					}
				}
				// New port ?
				for (set<string>::const_iterator ri = remotePackagesList.begin(); ri != remotePackagesList.end(); ri++) {
					bool found = false;
					for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); li++) {
						localPackage = *li;
						remotePackage = *ri;
						if ( remotePackage.substr(33) == localPackage ) {
							found = true;
							break;
						}
					}
					if ( ! found ) {
						cout << "d: "<< remotePackage.substr(33) << endl;
						createRecursiveDirs(prtDir + "/" + remotePackage.substr(33));
					}
				}		
			}
			if (m_argParser.isSet(CardsArgumentParser::OPT_SIGNATURE)) {
				for (set<string>::const_iterator i = remotePackagesList.begin(); i != remotePackagesList.end(); i++) {
					string input = *i;
					if ( input.size() < 33 ) {
						continue;
					}
					string dir = input.substr(33);
					string destinationFile = prtDir + "/" + dir + "/" + m_repoFile;
					/* If the MD5SUM File is present, need to check if it's uptodate
						 We need to check the MD5 sum of the local MD5SUM file against the list */
					if ( checkFileExist( destinationFile )) {
						for (set<string>::const_iterator ri = remotePackagesList.begin(); ri != remotePackagesList.end(); ri++) {
							string remote_input = *ri;
							string remote_dir = remote_input.substr(33);
							string MD5S = remote_input.substr(0,32);
							if ( remote_dir == dir ) {
								if (! checkMD5sum(destinationFile.c_str(),MD5S.c_str())) {
									cout << "f: " << destinationFile ;
									FileDownload MD5SumPort(remoteUrl + "/" + dir + "/" + m_repoFile,
										prtDir + "/" + dir,
										m_repoFile,
										input.substr(0,32),false);
									if ( MD5SumPort.downloadFile() != 0) {
										m_actualError = CANNOT_DOWNLOAD_FILE;
										treatErrors(prtDir + "/" + dir + "/" + m_repoFile);
									}
									if (MD5SumPort.checkMD5sum()) {
										cout << " OK" << endl;
									} else {
										cout << " FAIL !!!" << endl;
									}
								}
							}
						}
					} else { /* If the MD5SUM File is not present download it */
						cout << "f: " << destinationFile ;
						FileDownload MD5SumPort(remoteUrl + "/" + dir + "/" + m_repoFile,
							prtDir + "/" + dir,
							m_repoFile,
							input.substr(0,32),false);
						if ( MD5SumPort.downloadFile() != 0) {
							m_actualError = CANNOT_DOWNLOAD_FILE;
							treatErrors(prtDir + "/" + dir + "/" + m_repoFile);
						}
						if (MD5SumPort.checkMD5sum()) {
							cout << " OK" << endl;
						} else {
							cout << " FAIL !!!" << endl;
						}
					}
				}
			}
			if (m_argParser.isSet(CardsArgumentParser::OPT_DEPENDENCIES)) {
				for (set<string>::const_iterator i = remotePackagesList.begin(); i != remotePackagesList.end(); i++) {
					string input = *i;
					if ( input.size() < 33) {
						continue;
					}
					string dir  = input.substr(33);
					string MD5File = prtDir + "/" + dir + "/" + m_repoFile;
					string::size_type pos = input.find('@');
					if (pos != std::string::npos) {
						string depFile = input.substr(33,pos - 33) + ".deps";
						string destinationFile = prtDir + "/" + dir + "/" + depFile;
						/* If deps file is allready download, we check if it is Up to date */
						if ( checkFileExist(destinationFile) ) {
							set<string> filesList;
							string MD5FIle = prtDir + "/" + dir + "/" + m_repoFile;
							if ( parseFile(filesList,MD5FIle.c_str()) != 0) {
								m_actualError = CANNOT_READ_FILE;
								treatErrors(MD5FIle);
							}
							for (set<string>::const_iterator iF = filesList.begin(); iF != filesList.end(); iF++) {
								string input = *iF;
								if ( (input.size() < 10 ) || ( input[10] == ':') ) {
									continue; // The first line starting with a 10 digit epoc time in case binaries exists
								}
								string file  = input.substr(33);
								string MD5S = input.substr(0,32);
								if ( file == depFile ) {
									if (! checkMD5sum(destinationFile.c_str(),MD5S.c_str())) {
										cout << "f: " << destinationFile << endl;
										FileDownload DepsPort(remoteUrl + "/" + dir  + "/" + depFile,
											prtDir + "/" + dir,
											depFile,
											false);
										if ( DepsPort.downloadFile() != 0) {
											m_actualError = CANNOT_DOWNLOAD_FILE;
											treatErrors(destinationFile);
										}
										break;
									}
								}
							}
						} else { /* If deps file is not present download it */
							if (parseFile(remoteFilesList, MD5File.c_str()) != 0 ) {
								m_actualError = CANNOT_READ_FILE;
								treatErrors(MD5File);
							}
							bool found = false;
							for (set<string>::const_iterator i = remoteFilesList.begin(); i != remoteFilesList.end(); i++) {
								string input = *i;
								if ((input.size() < 10 ) || ( input[10] == ':')) {
									continue; // The first line is starting with a 10 digit epoc time in case binaries exists
								}
								if ( input.substr(33) == depFile ) {
									found = true;
									cout << "f: " << destinationFile << endl;
									FileDownload DepsPort(remoteUrl + "/" + dir  + "/" + depFile,
										prtDir + "/" + dir,
										depFile,
										false);
									if ( DepsPort.downloadFile() != 0) {
										m_actualError = CANNOT_DOWNLOAD_FILE;
										treatErrors(prtDir + "/" + dir + "/" + depFile);
									}
									break;
								} 
							}
							if ( found == false ) {
								depFile = input.substr(33,pos - 33) + ".run";
								for (set<string>::const_iterator i = remoteFilesList.begin(); i != remoteFilesList.end(); i++) {
									string input = *i;
									if ( (input.size() < 10 ) || ( input[10] == ':') ){
										continue; // The first line is starting with a 10 digit epoc time in case binaries exists
									}
									if ( input.substr(33) == depFile ) {
										destinationFile = prtDir + "/" + dir + "/" + depFile;
										cout << "f: " << destinationFile << endl;
										FileDownload DepsPort(remoteUrl + "/" + dir  + "/" + depFile,
											prtDir + "/" + dir,
											depFile,
											false);
										if ( DepsPort.downloadFile() != 0) {
											m_actualError = CANNOT_DOWNLOAD_FILE;
											treatErrors(prtDir + "/" + dir + "/" + depFile);
										}
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;	
}
// vim:set ts=2 :
