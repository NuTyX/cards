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

int CardsSync::run()
{
	struct stat info;
	if (stat(m_baseDirectory.c_str(), &info)) {
		if (createRecursiveDirs(m_baseDirectory)) {
			cerr << "Failed to create base directory "
				<< m_baseDirectory << endl;
			return -1;
		}
	}

	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);

	for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
		string category = basename(const_cast<char*>(config.prtDir[indCat].c_str()));
		cout << "Synchronising " << category << endl;
		string remoteUrl = config.Url + "/" + category;
		string categoryMD5sumFile = config.prtDir[indCat] + "/" + m_repoFile ;

		// Get the MD5SUM file of the category
		FileDownload MD5Sum(remoteUrl + "/" + m_repoFile,
			config.prtDir[indCat],
			m_repoFile, false);
		if ( MD5Sum.downloadFile () != 0) {
			cerr << "Failed to download " + remoteUrl + "/" << m_repoFile 
				<< endl;
			return -1;
		}
		set<string> localPackagesList,remotePackagesList,remoteFilesList;

		if ( parseFile(remotePackagesList,categoryMD5sumFile.c_str()) != 0) {
			cerr << "Failed to parse " 
				<< categoryMD5sumFile << endl;
			return -1;
		}

		// We need to remove the MD5SUM file
		remove(categoryMD5sumFile.c_str());

		// Let see what we have so for locally
		if ( findFile( localPackagesList, config.prtDir[indCat]) != 0 ) {
			cerr << "Cannot read " << config.prtDir[indCat] 
				<< endl;
				return -1;
		}
		// If they are some ports availables
		if ( remotePackagesList.size() > 0 ) {
			// If no directories found
			if (localPackagesList.size() == 0 ) {
				for (set<string>::const_iterator i = remotePackagesList.begin(); i != remotePackagesList.end(); i++) {
					string input = *i;
					string dir = input.substr(33);
					cout << "d: " << dir << endl;
					createRecursiveDirs(config.prtDir[indCat] + "/" + dir);
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
						if ( findFile(filesToDelete, config.prtDir[indCat]+ "/"+ localPackage) != 0 ){
							cerr << "Cannot read " << config.prtDir[indCat]+ "/"+ localPackage 
								<< endl;
							return -1;
						}
						for (set<string>::const_iterator f = filesToDelete.begin(); f != filesToDelete.end(); f++) {
							removeFile("/",config.prtDir[indCat]+ "/"+ localPackage + "/"+ *f);
						}
						removeFile("/",config.prtDir[indCat]+ "/"+localPackage);
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
						createRecursiveDirs(config.prtDir[indCat] + "/" + remotePackage.substr(33));
					}
				}		
			}
			if (m_argParser.isSet(CardsArgumentParser::OPT_SIGNATURE)) {
				for (set<string>::const_iterator i = remotePackagesList.begin(); i != remotePackagesList.end(); i++) {
					string input = *i;
					string dir = input.substr(33);
					string destinationFile = config.prtDir[indCat] + "/" + dir + "/" + m_repoFile;
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
										config.prtDir[indCat] + "/" + dir,
										m_repoFile,
										input.substr(0,32),false);
									if ( MD5SumPort.downloadFile() != 0) {
										cerr << "Failed to download " + config.prtDir[indCat] + "/" + dir
											<< "/" + m_repoFile << endl ;
										return -1;
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
							config.prtDir[indCat] + "/" + dir,
							m_repoFile,
							input.substr(0,32),false);
						if ( MD5SumPort.downloadFile() != 0) {
							cerr << "Failed to download " + config.prtDir[indCat] + "/" + dir 
								<< "/" + m_repoFile << endl ;
							return -1;
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
					string dir  = input.substr(33);
					string MD5File = config.prtDir[indCat] + "/" + dir + "/" + m_repoFile;
					unsigned int pos = input.find('_');
					if (pos != std::string::npos) {
						string depFile = input.substr(33,pos - 33) + ".deps";
						string destinationFile = config.prtDir[indCat] + "/" + dir + "/" + depFile;
						/* If deps file is allready download, we check if it is Up to date */
						if ( checkFileExist(destinationFile) ) {
							set<string> filesList;
							string MD5FIle = config.prtDir[indCat] + "/" + dir + "/" + m_repoFile;
							if ( parseFile(filesList,MD5FIle.c_str()) != 0) {
								cerr << "Failed to parse "
									<< MD5FIle << endl;
								return -1;
							}
							for (set<string>::const_iterator iF = filesList.begin(); iF != filesList.end(); iF++) {
								string input = *iF;
								string file  = input.substr(33);
								string MD5S = input.substr(0,32);
								if ( file == depFile ) {
									if (! checkMD5sum(destinationFile.c_str(),MD5S.c_str())) {
										cout << "f: " << destinationFile << endl;
										FileDownload DepsPort(remoteUrl + "/" + dir  + "/" + depFile,
											config.prtDir[indCat] + "/" + dir,
											depFile,
											false);
										if ( DepsPort.downloadFile() != 0) {
											cerr << "Failed to download " + destinationFile << endl;
											return -1;
										}
										break;
									}
								}
							}
						} else { /* If deps file is not present download it */
							if (parseFile(remoteFilesList, MD5File.c_str()) != 0 ) {
								cerr << "Failed to parse " +  MD5File << endl;
								return -1;
							}
							for (set<string>::const_iterator indFile = remoteFilesList.begin(); indFile != remoteFilesList.end(); indFile++) {
								if ( indFile -> substr(33) == depFile ) {
									cout << "f: " << destinationFile << endl;
									FileDownload DepsPort(remoteUrl + "/" + dir  + "/" + depFile,
										config.prtDir[indCat] + "/" + dir,
										depFile,
										false);
									if ( DepsPort.downloadFile() != 0) {
										cerr << "Failed to download " + config.prtDir[indCat] + "/" + dir
											<< "/" + depFile << endl;
										return -1;
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
