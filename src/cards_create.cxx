/*
 * cards_create.cxx
 * 
 * Copyright 2016 - 2023 Thierry Nuttens <tnut@nutyx.org>
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


cards_create::cards_create(const CardsArgumentParser& argParser,
	const char *configFileName,
	const std::vector<std::string>& listOfPackages)
	: pkginst("cards create",configFileName),m_argParser(argParser)
{
	parseArguments();
	for (auto i : listOfPackages) createBinaries(configFileName,
		basename(const_cast<char*>(i.c_str())) );
}
cards_create::cards_create(const CardsArgumentParser& argParser,
	const char *configFileName,
	const std::string& packageName)
	: pkginst("cards create",configFileName),m_argParser(argParser)
{
	parseArguments();
	createBinaries(configFileName, packageName);
}
void cards_create::createBinaries(const char *configFileName,
	const std::string& packageName)
{
	cards::conf config(configFileName);

	std::cout << "create of " << packageName << std::endl;
	std::string pkgdir = getPortDir(packageName);
	if (pkgdir == "" ) {
		m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
		treatErrors(packageName);
	}
	std::string timestamp;
	std::string commandName = "cards create: ";
	std::string message;
	std::string packageFileName;
	int fdlog = -1;

	if ( config.logdir() != "" ) {
		if ( ! createRecursiveDirs(config.logdir()) ) {
			m_actualError = cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY;
			treatErrors(config.logdir());
		}
		std::string logFile = config.logdir() + "/" + packageName + ".log";
		unlink( logFile.c_str() );
		fdlog = open(logFile.c_str(),O_APPEND | O_WRONLY | O_CREAT, 0666 );
		if ( fdlog == -1 ) {
			m_actualError = cards::ERROR_ENUM_CANNOT_OPEN_FILE;
			treatErrors(logFile);
		}
	}
	message = commandName + pkgdir + " package(s)";
	std::cout << message << std::endl;
	
	if ( config.logdir() != "" ) {
		write( fdlog, message.c_str(), message.length());
		time_t startTime;
		time(&startTime);
		timestamp = ctime(&startTime);
		timestamp = " starting build " + timestamp;
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
	}
	chdir( pkgdir.c_str() );
	
	std::string runscriptCommand = "sh";
	std::string cmd = "pkgmk";
	std::string args = "-d ";

	process makeprocess( cmd, args, fdlog );
	int result = 0 ;
	result = makeprocess.executeShell();
#ifndef NDEBUG
	cerr << "result:" << result << std::endl;
#endif
	std::string s = RED;
	switch ( result )
	{
		case 1:
			s += "General error";
			break;
		case 2:
			s += "Invalid Pkgfile";
			break;
		case 3:
			s += "Sources / build directory missing or missing read/write permission";
			break;
		case 4:
			s += "Download of sources file(s)";
			break;
		case 5:
			s += "Unpacking of sources file(s)";
			break;
		case 6:
			s += "Checking md5sum from sources";
			break;
		case 7:
			s += "Checking footprint";
			break;
		case 8:
			s += "While running 'build()'";
			break;
		case 10:
			s += "Searching runtime dependancies";
			break;
		case 11:
			s += "Duplicate alias";
			break;
	}
	if (result > 0) {
		s += NORMAL;
		m_actualError = cards::ERROR_ENUM_CANNOT_PARSE_FILE;
		treatErrors("Pkgfile: " + s);
	} 
	if ( config.logdir() != "" ) {
		time_t endTime;
		time(&endTime);
		timestamp = ctime(&endTime);
		timestamp = commandName + "build done " + timestamp;

		write( fdlog, "\n", 1 );
		write( fdlog, timestamp.c_str(), timestamp.length());
		write( fdlog, "\n", 1 );
	}

	std::set<std::string> listOfPackages;
	if (findDir(listOfPackages, pkgdir) != 0) {
		m_actualError = cards::ERROR_ENUM_CANNOT_READ_DIRECTORY;
		treatErrors(pkgdir);
	}

	// Let's install the found binaries now
	for (auto i : listOfPackages) {
		if (i.find("cards.tar")== std::string::npos )
			continue;
		packageFileName = pkgdir + "/" + i;
		archive packageArchive(packageFileName.c_str());
		std::string name = packageArchive.name();
		std::string version = packageArchive.version();
		message = "CREATED: " + name + " " + version;
		m_upgrade=0;
		buildSimpleDatabase();
		if ( checkPackageNameExist(name) ) {
			message = name + ": is ALLREADY installed";
			m_upgrade=1;
		}
		m_packageArchiveName = pkgdir + "/" + i;
		run();
		std::cout << message << std::endl;
		if ( config.logdir() != "" ) {
			write( fdlog, message.c_str(), message.length());
			write( fdlog, "\n", 1 );
		}
		
	}
	if ( config.logdir() != "" ) {
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
void cards_create::parseArguments()
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);
	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	if (getuid()) {
		m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
}
