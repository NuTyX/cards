//  cards_depends.cxx
//
//  Copyright (c) 2013 - 2023 by NuTyX team (http://nutyx.org)
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
#include "pkgrepo.h"


CardsDepends::CardsDepends (const CardsArgumentParser& argParser)
	: Pkgdbh(""),
		m_argParser(argParser)
{
	parseArguments();
}
void CardsDepends::parseArguments()
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);
	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
}
depList * CardsDepends::readDependenciesList(itemList *filesList, unsigned int nameIndex)
{
	if ( nameIndex > filesList->count  ) {
		return NULL;
	}
	depList *dependancesList = initDepsList();
	char fullPathfileName[PATH_MAX];
	char name[PATH_MAX];
	sprintf(name,"%s",basename(filesList->items[nameIndex]));
	itemList *nameDeps = initItemList();
	std::string missingDep = "";
	sprintf(fullPathfileName,"%s/Pkgfile",filesList->items[nameIndex]);
	if ( ! checkFileExist(fullPathfileName) ) {
		m_actualError = CANNOT_READ_FILE;
		treatErrors(fullPathfileName);
	}
#ifndef NDEBUG
	std::cerr << fullPathfileName << std::endl;
#endif
	std::list<std::string> deps;
	std::string depends;
	if ( parseFile(depends, "run=(", fullPathfileName) )
		return NULL;
	if ( ! depends.empty() ) depends += " ";
	if ( parseFile(depends, "depends=(", fullPathfileName) )
		return NULL;
	if ( ! depends.empty() ) depends += " ";
	if ( parseFile(depends, "makedepends=(", fullPathfileName) )
		return NULL;
	if ( ! depends.empty() ) {
		replaceAll( depends, "'", " " );
		replaceAll( depends, "\\", " " );
		depends = stripWhiteSpace( depends );
		replaceAll( depends, " ", "," );
		replaceAll( depends, ",,", "," );
		split( depends, ',', deps, 0,true);
	}
	if ( deps.size() >0 ) {
		bool found=false;
		unsigned j = 0;
		std::string s;
		for ( auto i : deps ) {
			found=false;
			s = getValueBeforeLast( i, '.');
			// Name of Dep = Name of Package then ignore it
			if (strcmp(s.c_str(),name) == 0 )
					continue;
			char n[PATH_MAX];
			for(j = 0; j < filesList->count; j++) {
				sprintf(n,"%s",basename(filesList->items[j]));
				if (strcmp(s.c_str(),n) == 0 ) {
					addDepToDepList(dependancesList,j,0);
					found=true;
					break;
				}
			}
			if(!found) {
				missingDep = "WARNING ";
				missingDep += s;
				missingDep += " from ";
				missingDep += filesList->items[nameIndex];
				missingDep += " NOT FOUND ...";
				m_missingDepsList.insert(missingDep);
			}
		}
	}
	freeItemList(nameDeps);
	return dependancesList;
}
std::vector<LevelName>& CardsDepends::getLevel()
{
	if ( level() !=0 )
	{
		m_actualError = CANNOT_GENERATE_LEVEL;
		treatErrors(" in level()");
	}

#ifndef NDEBUG
	for ( auto i : m_levelList ) {
		std::cerr << i.name<< std::endl;
	}
#endif
	return m_levelList;
}
std::vector<std::string>& CardsDepends::getNeededDependencies()
{
	m_packageName = m_argParser.otherArguments()[0].c_str();

	buildSimpleDatabase();

	depends();

	for ( auto i : m_dependenciesList ) {
		std::string packageName = basename(const_cast<char*>(i.c_str()));
		std::string::size_type pos = packageName.find('@');
		std::string name = "";
		if ( pos != std::string::npos) {
				name= packageName.substr(0,pos);
		} else {
				name = packageName;
		}
		if ( ! checkPackageNameExist(name)) {
			m_neededDependenciesList.push_back(i);
		}
	}
#ifndef NDEBUG
	for (auto i : m_neededDependenciesList) std::cerr << i << std::endl;
#endif
	return m_neededDependenciesList;
}
std::vector<std::string>& CardsDepends::getDependencies()
{
	m_packageName=m_argParser.otherArguments()[0].c_str();
	depends();
#ifndef NDEBUG
	for (auto i : m_dependenciesList) std::cerr << i << std::endl;
#endif
	return m_dependenciesList;
}
void CardsDepends::showDependencies()
{
	m_packageName = m_argParser.otherArguments()[0].c_str();
	if (m_argParser.isSet(CardsArgumentParser::OPT_INSTALLED)) {
		depends();
		for ( auto i : m_dependenciesList ) {
			std::cout << i << std::endl;
		}
	}else {
		getNeededDependencies();
		for ( auto i : m_neededDependenciesList ) {
				std::cout << i << std::endl;
		}
	}
}
void CardsDepends::showLevel()
{
	if ( level() != 0 )
	{
		m_actualError = CANNOT_GENERATE_LEVEL;
		treatErrors(" in level()");
	}

	if ( (m_missingDepsList.size() == 0 ) || ( m_argParser.isSet(CardsArgumentParser::OPT_IGNORE))) {
		for ( auto i : m_levelList) std::cout << i.l << ": " << i.name << std::endl;
	} else {
		for ( auto i : m_missingDepsList ) std::cout << i << std::endl;
	}
}
void CardsDepends::treatErrors(const std::string& s) const
{
	switch ( m_actualError )
	{
		case CANNOT_CREATE_DIRECTORY:
		case CANNOT_CREATE_FILE:
		case CANNOT_WRITE_FILE:
		case CANNOT_SYNCHRONIZE:
		case CANNOT_COPY_FILE:
		case CANNOT_RENAME_FILE:
		case CANNOT_DETERMINE_NAME_BUILDNR:
		case WRONG_ARCHITECTURE:
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
			throw std::runtime_error("unknow error");
			break;
		case CANNOT_DOWNLOAD_FILE:
			throw std::runtime_error("could not download " + s);
			break;
		case CANNOT_READ_FILE:
			throw std::runtime_error("could not read " + s);
			break;
		case CANNOT_READ_DIRECTORY:
			throw RunTimeErrorWithErrno("could not read directory " + s);
			break;
		case PACKAGE_NOT_FOUND:
			throw std::runtime_error("package " + s + " not found");
			break;
		case CANNOT_GENERATE_LEVEL:
			throw std::runtime_error("cannot generate the levels" + s);
			break;
	}
}
int CardsDepends::level()
{
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	itemList *filesList = initItemList();
	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);
	for (std::vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
		if ( i->Url.size() > 0)
			continue;
		DirUrl DU  = *i ;
		std::string prtDir = DU.Dir;
		if ( (findDir(filesList,prtDir.c_str())) != 0) {
			m_actualError = CANNOT_READ_DIRECTORY;
			treatErrors(prtDir);
		}
#ifndef NDEBUG
		std::cerr << i->Dir << " " << i->Url  << std::endl;
#endif
	}
#ifndef NDEBUG
	std::cerr << "number of files: " << filesList->count << std::endl;
#endif
	for (unsigned int nInd=0;nInd <filesList->count;nInd++) {
#ifndef NDEBUG
		std::cerr << nInd << " " << filesList->items[nInd] << std::endl;
#endif
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
		if  (packagesList->pkgs[nInd]->dependences== NULL)
			return -1;
	}
	unsigned int niveau = 0;
	static unsigned  int *pNiveau = &niveau;
	generate_level (filesList,packagesList,pNiveau);
#ifndef NDEBUG
	std::cerr << "Number of level: " << *pNiveau << std::endl;
#endif
	if (*pNiveau == 0 ) {
		m_actualError = CANNOT_GENERATE_LEVEL;
		treatErrors(" in level()");
	} else {
#ifndef NDEBUG
		std::cerr << "Number of level: " << *pNiveau << std::endl;
#endif
	}
	depList *dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,1) != 0 ) {
		return returnVal;
	}
	int currentNiveau = 0;
	while ( currentNiveau <= *pNiveau) {
		for ( unsigned int nameIndex = 0; nameIndex < packagesList -> count; nameIndex++ ) {
#ifndef NDEBUG
			std::cerr << "packagesList -> pkgs[nameIndex]->niveau: " << packagesList -> pkgs[nameIndex]->niveau << " " << filesList->items[nameIndex] << std::endl;
#endif
			if ( packagesList -> pkgs[nameIndex]->niveau == currentNiveau ) {
				LevelName LN;
				LN.l = currentNiveau;
				LN.name = filesList->items[nameIndex];
				m_levelList.push_back(LN);
			}
		}
		currentNiveau++;
	}
	freeItemList(filesList);
// TODO findout why segmentfault
//freePkgInfo(package);
	freePkgList(packagesList);
#ifndef NDEBUG
	std::cerr << "Level() FINISH" << std::endl;
#endif
	return 0;
}
int CardsDepends::depends()
{
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();
	itemList *filesList = initItemList();
	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);
	for ( auto DU : config.dirUrl ) {
		std::string prtDir = DU.Dir;
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
#ifndef NDEBUG
	std::cerr << longPackageName << " " << m_packageName << std::endl;
#endif
	// for all the Pkgfile files found
	for (unsigned int nInd=0;nInd <filesList->count;nInd++){
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}
	unsigned int niveau = 0;
	static unsigned int *pNiveau = &niveau;
	generate_level (filesList,packagesList,pNiveau);
	if (*pNiveau == 0 ) {
		m_actualError = CANNOT_GENERATE_LEVEL;
		treatErrors(" in depends()");
	} else {
#ifndef NDEBUG
		std::cerr << "Number of level: " << *pNiveau << std::endl;
#endif
	}
	depList *dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (filesList,packagesList,dependenciesList,longPackageName,1) != 0 ) {
		return returnVal;
	}
	if (dependenciesList ->count > 0) {
		int currentNiveau = 0;
		while ( currentNiveau <= *pNiveau) {
#ifndef NDEBUG
			std::cerr << "Level: " << currentNiveau << std::endl;
#endif
			for ( unsigned int dInd=0; dInd < dependenciesList->count; dInd++ ) {
#ifndef NDEBUG
				std::cerr << "packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau: "
						<< packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau
						<< " " << filesList->items[dependenciesList->depsIndex[dInd]] << std::endl;
#endif
				if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == currentNiveau ) {
					bool found = false;
					for (std::vector<std::string>::iterator i = m_dependenciesList.begin();i != m_dependenciesList.end();++i) {
						std::string s = filesList->items[dependenciesList->depsIndex[dInd]];
						if ( s == *i) {
							found = true;
							break;
						}
					}
					if ( ! found ) { // if not allready found
						m_dependenciesList.push_back(filesList->items[dependenciesList->depsIndex[dInd]]);
					}
				}
			}
			currentNiveau++;
		}
	}
	m_dependenciesList.push_back(longPackageName);
	freeItemList(filesList);
	freePkgList(packagesList);
// TODO findout why it's segment fault
//	freePkgInfo(package);
	free(longPackageName);
#ifndef NDEBUG
	std::cerr << "depends() FINISH" << std::endl;
#endif

	return 0;
}
int CardsDepends::deptree()
{
	m_packageName = m_argParser.otherArguments()[0].c_str();

	itemList *filesList = initItemList();

	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	depList *dependenciesList = NULL;
	dependenciesList = initDepsList();

	Config config;
	Pkgrepo::parseConfig("/etc/cards.conf", config);
	for ( auto DU : config.dirUrl ) {
		std::string prtDir = DU.Dir;
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
	std::string name = "";
	std::set<std::string> localPackagesList, depsPackagesList;

	for (auto DU : config.dirUrl ) {
	if ( DU.Url == "")
		continue;

	std::string prtDir, Url ;
	prtDir = DU.Dir;
	Url = DU.Url;
		std::string category = basename(const_cast<char*>(prtDir.c_str()));
		std::string remoteUrl = Url + "/" + category;
		DIR *d;
		struct dirent *dir;
		d = opendir(prtDir.c_str());
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				if ( strcmp (dir->d_name, ".") && strcmp (dir->d_name, "..") ) {
					localPackagesList.insert(prtDir + "/" + dir->d_name);
					std::string dirName = dir->d_name;
					name = dirName;
					std::string::size_type pos =  dirName.find('@');
					if ( pos != std::string::npos) {
						name = dirName.substr(0,pos);
					}
					if (! strcmp (m_packageName,name.c_str())) {
						found=true;
						std::string depFile = prtDir
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
			std::cerr << "Cannot find " << m_packageName << std::endl;
			return -1;
		}
		if (localPackagesList.size() == 0 ) {
			std::cout << "You need to 'cards' sync first"<< std::endl;
			return -1;
		}
	}
	freeItemList(filesList);
	// TODO Findou why it's segmentfault
//	freePkgInfo(package);
	freePkgList(packagesList);
	return 0;
}
// vim:set ts=2 :
