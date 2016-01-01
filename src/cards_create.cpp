/*
 * cards_create.cpp
 * 
 * Copyright 2016 Thierry Nuttens <tnut@nutyx.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include "cards_create.h"


Cards_create::Cards_create(const CardsArgumentParser& argParser,
	const std::string& configFileName,
	const std::vector<string>& listOfPackages)
	: Pkginst("cards create",configFileName),m_argParser(argParser)
{
	for (auto i : listOfPackages) createBinaries(configFileName, i);
}
Cards_create::Cards_create(const CardsArgumentParser& argParser,
	const std::string& configFileName,
	const std::string& packageName)
	: Pkginst("cards create",configFileName),m_argParser(argParser)
{
	createBinaries(configFileName, packageName);
}
void Cards_create::createBinaries(const string& configFileName,
	const string& packageName)
{
	Config config;
	Pkgrepo::parseConfig(configFileName, config);
	cout << "create of " << packageName << endl;
	string pkgdir = getPortDir(packageName);
	if (pkgdir == "" ) {
		m_actualError = PACKAGE_NOT_FOUND;
		treatErrors(packageName);
	}
	string timestamp;
	string commandName = "cards create: ";
	string message;
	int fdlog = -1;
	
	if ( config.logdir != "" ) {
		if ( ! createRecursiveDirs(config.logdir) ) {
			m_actualError = CANNOT_CREATE_DIRECTORY;
			treatErrors(config.logdir);
		}
		string logFile = config.logdir + "/" + packageName + ".log";
		unlink( logFile.c_str() );
		if ( fdlog == -1 ) {
			m_actualError = CANNOT_OPEN_FILE;
			treatErrors(logFile);
		}
	}
	message = commandName + pkgdir + " package(s)";
	cout << message << endl;
	
	if ( config.logdir != "" ) {
		write( fdlog, message.c_str(), message.length());
		time_t startTime;
		time(&startTime);
		timestamp = ctime(&startTime);
		timestamp = " starting build " + timestamp;
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
	}
	chdir( pkgdir.c_str() );
	
	string runscriptCommand = "sh";
	string cmd = "pkgmk";
	string args = "-d ";

	process makeprocess( cmd, args, fdlog );
	int result = 0 ;
	result = makeprocess.executeShell();
#ifndef NDEBUG
	cerr << result << endl;
#endif
	switch ( result )
	{
		case 1:
			throw runtime_error("General error");
			break;
		case 2:
			throw runtime_error("Error Invalid Pkgfile");
			break;
		case 3:
			throw runtime_error("Error Sources /build directory missing or missing read/write permission");
			break;
		case 4:
			throw runtime_error("Error during download of sources file(s)");
			break;
		case 5:
			throw runtime_error("Error during unpacking of sources file(s)");
			break;
		case 6:
			throw runtime_error("Error md5sum from sources Checking");
			break;
		case 7:
			throw runtime_error("Error footprint Checking");
			break;
		case 8:
			throw runtime_error("Error while running 'build()'");
			break;
		case 10:
			throw runtime_error("Error searching runtime dependancies");
			break;
		case 11:
			throw runtime_error(pkgdir + "/" + m_packageName +".info not found");
			break;
	}
	if (result > 0) { //TODO find out why return code is wrong
		m_actualError = CANNOT_PARSE_FILE;
		treatErrors(pkgdir+ "/Pkgfile");
	} 
	if ( config.logdir != "" ) {
		time_t endTime;
		time(&endTime);
		timestamp = ctime(&endTime);
		timestamp = commandName + "build done " + timestamp;

		write( fdlog, "\n", 1 );
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
	}

	std::set<string> listOfPackages = findPackages(pkgdir);

	// Let's install the found binaries now
	for (auto i : listOfPackages) {
		if (i.find("cards.tar")== string::npos )
			continue;
		m_packageFileName = pkgdir + "/" + i;
		ArchiveUtils packageArchive(m_packageFileName.c_str());
		string name = packageArchive.name();
		string version = packageArchive.version();
		if ( ! checkPackageNameExist(name) ) {
			message = "ADD FOR INSTALL: " + name + " " + version;
			m_packageFileName = pkgdir + "/" + i;
			run();
		} else {
			message = "WARNING: " + name + " is ALLREADY installed";
		}
		cout << message << endl;
		if ( config.logdir != "" ) {
			write( fdlog, message.c_str(), message.length());
			write( fdlog, "\n", 1 );
		}
		
	}
	if ( config.logdir != "" ) {
		time_t finishTime;
		time(&finishTime);
		timestamp = ctime(&finishTime);
		timestamp = commandName + "finish " + timestamp;
		write( fdlog, "\n", 1 );
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
		close ( fdlog );
	}	
}
set<string> Cards_create::findPackages(const string& path)
{
	set<string> packageList;
	if (findFile(packageList, path) != 0) {
		m_actualError = CANNOT_READ_DIRECTORY;
		treatErrors(path);
	}
	return packageList;
}

