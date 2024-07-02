/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <iostream>
#include <cstdlib>
#include <set>
#include <string>

#include "file_download.h"
#include "file_utils.h"
#include "compile_dependencies_utils.h"
#include "cards_depends.h"
#include "pkgrepo.h"


cards_depends::cards_depends (const CardsArgumentParser& argParser)
	: m_argParser(argParser)
{
	m_package = nullptr;
	m_longPackageName = nullptr;
	m_packageName = nullptr;
	m_packagesList = initPkgList();
	m_filesList = initItemList();
}
cards_depends::~cards_depends ()
{
	freeItemList(m_filesList);
	freePkgInfo(m_package);
	freePkgList(m_packagesList);
}
void cards_depends::checkConfig()
{
	cards::conf m_config;

	for ( auto DU : m_config.dirUrl() ) {
		if ( (findDir(m_filesList,basename(const_cast<char*>(DU.dir.c_str())))) != 0) {
			if ( (findDir(m_filesList,DU.dir.c_str())) != 0) {
				treatErrors(cards::ERROR_ENUM_CANNOT_READ_DIRECTORY,DU.dir);
			}
		}
#ifdef DEBUG
		std::cerr << DU.dir << " " << DU.url  << std::endl;
#endif
	}
}
depList * cards_depends::readDependenciesList(itemList *m_filesList, unsigned int nameIndex)
{
	if ( nameIndex > m_filesList->count  ) {
		return NULL;
	}
	depList* dependancesList = initDepsList();
	char fullPathfileName[PATH_MAX];
	char name[PATH_MAX];
	sprintf(name,"%s",basename(m_filesList->items[nameIndex]));
	itemList *nameDeps = initItemList();
	std::string missingDep = "";
	sprintf(fullPathfileName,"%s/Pkgfile",m_filesList->items[nameIndex]);
	if ( m_filesList->items[nameIndex] == NULL)
					return NULL;
	if ( ! checkFileExist(fullPathfileName) ) {
		treatErrors(cards::ERROR_ENUM_CANNOT_READ_FILE,fullPathfileName);
	}
#ifdef DEBUG
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
#ifdef DEBUG
	for (auto i: deps)
					std::cerr << i << ",";
	std::cerr << std::endl;
#endif
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
			for(j = 0; j < m_filesList->count; j++) {
				sprintf(n,"%s",basename(m_filesList->items[j]));
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
				missingDep += m_filesList->items[nameIndex];
				missingDep += " NOT FOUND ...";
				m_missingDepsList.insert(missingDep);
			}
		}
	}
	freeItemList(nameDeps);
	return dependancesList;
}

std::vector<std::string>& cards_depends::getNeededDependencies()
{
	m_packageName = m_argParser.otherArguments()[0].c_str();

	pkgdbh packageDatabase;
	packageDatabase.buildSimpleDatabase();
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
		if ( ! packageDatabase.checkPackageNameExist(name)) {
			m_neededDependenciesList.push_back(i);
		}
	}
#ifdef DEBUG
	for (auto i : m_neededDependenciesList) std::cerr << i << std::endl;
#endif
	return m_neededDependenciesList;
}
std::vector<std::string>& cards_depends::getDependencies()
{
	m_packageName=m_argParser.otherArguments()[0].c_str();
	depends();
#ifdef DEBUG
	for (auto i : m_dependenciesList) std::cerr << i << std::endl;
#endif
	return m_dependenciesList;
}
void cards_depends::showDependencies()
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

void cards_depends::treatErrors(cards::ErrorEnum action, const std::string& s) const
{
	switch ( action )
	{
		case cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY:
		case cards::ERROR_ENUM_CANNOT_CREATE_FILE:
		case cards::ERROR_ENUM_CANNOT_WRITE_FILE:
		case cards::ERROR_ENUM_CANNOT_SYNCHRONIZE:
		case cards::ERROR_ENUM_CANNOT_COPY_FILE:
		case cards::ERROR_ENUM_CANNOT_RENAME_FILE:
		case cards::ERROR_ENUM_CANNOT_DETERMINE_NAME_BUILDNR:
		case cards::ERROR_ENUM_WRONG_ARCHITECTURE:
		case cards::ERROR_ENUM_EMPTY_PACKAGE:
		case cards::ERROR_ENUM_CANNOT_FORK:
		case cards::ERROR_ENUM_WAIT_PID_FAILED:
		case cards::ERROR_ENUM_DATABASE_LOCKED:
		case cards::ERROR_ENUM_CANNOT_LOCK_DIRECTORY:
		case cards::ERROR_ENUM_CANNOT_REMOVE_FILE:
		case cards::ERROR_ENUM_CANNOT_RENAME_DIRECTORY:
		case cards::ERROR_ENUM_OPTION_ONE_ARGUMENT:
		case cards::ERROR_ENUM_INVALID_OPTION:
		case cards::ERROR_ENUM_OPTION_MISSING:
		case cards::ERROR_ENUM_TOO_MANY_OPTIONS:
		case cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
		case cards::ERROR_ENUM_PACKAGE_ALLREADY_INSTALL:
		case cards::ERROR_ENUM_PACKAGE_NOT_INSTALL:
		case cards::ERROR_ENUM_PACKAGE_NOT_PREVIOUSLY_INSTALL:
		case cards::ERROR_ENUM_LISTED_FILES_ALLREADY_INSTALLED:
		case cards::ERROR_ENUM_PKGADD_CONFIG_LINE_TOO_LONG:
		case cards::ERROR_ENUM_PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS:
		case cards::ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_ACTION:
		case cards::ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_EVENT:
		case cards::ERROR_ENUM_CANNOT_COMPILE_REGULAR_EXPRESSION:
		case cards::ERROR_ENUM_NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE:
		case cards::ERROR_ENUM_CANNOT_OPEN_FILE:
		case cards::ERROR_ENUM_CANNOT_FIND_FILE:
		case cards::ERROR_ENUM_CANNOT_PARSE_FILE:
			throw std::runtime_error("unknow error");
			break;
		case cards::ERROR_ENUM_CANNOT_DOWNLOAD_FILE:
			throw std::runtime_error("could not download " + s);
			break;
		case cards::ERROR_ENUM_CANNOT_READ_FILE:
			throw std::runtime_error("could not read " + s);
			break;
		case cards::ERROR_ENUM_CANNOT_READ_DIRECTORY:
			throw RunTimeErrorWithErrno("could not read directory " + s);
			break;
		case cards::ERROR_ENUM_PACKAGE_NOT_FOUND:
			throw std::runtime_error("package " + s + " not found");
			break;
		case cards::ERROR_ENUM_CANNOT_GENERATE_LEVEL:
			throw std::runtime_error("cannot generate the levels" + s);
			break;
	}
}
void cards_depends::level()
{
	checkConfig();

#ifdef DEBUG
	std::cerr << "number of files: " << m_filesList->count << std::endl;
#endif
	for (unsigned int nInd=0;nInd <m_filesList->count;nInd++) {
#ifdef DEBUG
		std::cout << nInd << " " << m_filesList->items[nInd] << std::endl;
#endif
		m_package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(m_packagesList,m_package);
		m_packagesList->pkgs[nInd]->dependences=readDependenciesList(m_filesList,nInd);
		if  (m_packagesList->pkgs[nInd]->dependences== NULL)
			treatErrors(cards::ERROR_ENUM_CANNOT_GENERATE_LEVEL," in level()");
	}
	unsigned int level = 0;
	static unsigned  int Niveau = level;
	generate_level (m_filesList,m_packagesList,Niveau);
	if (Niveau == 0 ) {
		treatErrors(cards::ERROR_ENUM_CANNOT_GENERATE_LEVEL," in level()");
	}
#ifdef DEBUG
		std::cerr << "Number of level: " << *pNiveau << std::endl;
#endif
	depList* dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (m_filesList,m_packagesList,dependenciesList,1) != 0 ) {
		if ( returnVal != 0)
			treatErrors(cards::ERROR_ENUM_CANNOT_GENERATE_LEVEL," in level()");
	}
	int currentLevel = 0;
	while ( currentLevel <= Niveau) {
		for ( unsigned int nameIndex = 0; nameIndex < m_packagesList -> count; nameIndex++ ) {
#ifdef DEBUG
			std::cout << "packagesList -> pkgs[nameIndex]->level: "
				<< m_packagesList -> pkgs[nameIndex]->level
				<< " "
				<< m_filesList->items[nameIndex]
				<< std::endl;
#endif
			if ( m_packagesList -> pkgs[nameIndex]->level == currentLevel ) {
				LevelName LN;
				LN.l = currentLevel;
				LN.name = m_filesList->items[nameIndex];
				m_levelList.push_back(LN);
			}
		}
		currentLevel++;
	}
#ifdef DEBUG
	std::cerr << "Level() FINISH" << std::endl;
#endif
	if ( (m_missingDepsList.size() == 0 ) || ( m_argParser.isSet(CardsArgumentParser::OPT_IGNORE))) {
		for ( auto i : m_levelList) std::cout << i.l << ": " << i.name << std::endl;
	} else {
		for ( auto i : m_missingDepsList ) std::cout << i << std::endl;
	}
}
int cards_depends::depends()
{
	checkConfig();

	if ( (m_longPackageName = getLongPackageName(m_filesList,m_packageName)) == NULL) {
		treatErrors(cards::ERROR_ENUM_PACKAGE_NOT_FOUND,m_packageName);
	}
#ifdef DEBUG
	std::cerr << m_longPackageName << " " << m_packageName << std::endl;
#endif

	// for all the Pkgfile files found
	for (unsigned int nInd=0;nInd <m_filesList->count;nInd++){
		m_package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(m_packagesList,m_package);
		m_packagesList->pkgs[nInd]->dependences=readDependenciesList(m_filesList,nInd);
	}
	unsigned int level = 0;
	static unsigned int Niveau = level;
	generate_level (m_filesList,m_packagesList,Niveau);
	if (Niveau == 0 ) {
		treatErrors(cards::ERROR_ENUM_CANNOT_GENERATE_LEVEL," in depends()");
	}
#ifdef DEBUG
	std::cerr << "Number of level: " << *pNiveau << std::endl;
#endif
	depList* dependenciesList = initDepsList();
	if ( int returnVal = deps_direct (m_filesList,m_packagesList,dependenciesList,m_longPackageName,1) != 0 ) {
		return returnVal;
	}
	if (dependenciesList ->count > 0) {
		int currentLevel = 0;
		while ( currentLevel <= Niveau) {
#ifdef DEBUG
			std::cerr << "Level: " << currentLevel << std::endl;
#endif
			for ( unsigned int dInd=0; dInd < dependenciesList->count; dInd++ ) {
#ifdef DEBUG
				std::cerr << "m_packagesList->pkgs[dependenciesList->depsIndex[dInd]]->level: "
						<< m_packagesList->pkgs[dependenciesList->depsIndex[dInd]]->level
						<< " " << m_filesList->items[dependenciesList->depsIndex[dInd]] << std::endl;
#endif
				if ( m_packagesList->pkgs[dependenciesList->depsIndex[dInd]]->level == currentLevel ) {
					bool found = false;
					for (auto i : m_dependenciesList) {
						std::string s = m_filesList->items[dependenciesList->depsIndex[dInd]];
						if ( s == i) {
							found = true;
							break;
						}
					}
					if ( ! found ) { // if not allready found
						m_dependenciesList.push_back(m_filesList->items[dependenciesList->depsIndex[dInd]]);
					}
				}
			}
			currentLevel++;
		}
	}
	m_dependenciesList.push_back(m_longPackageName);
	free(m_longPackageName);

#ifdef DEBUG
	std::cerr << "depends() FINISH" << std::endl;
#endif

	return 0;
}
int cards_depends::deptree()
{
	m_packageName = m_argParser.otherArguments()[0].c_str();
	depList* dependenciesList = NULL;
	dependenciesList = initDepsList();

	checkConfig();

	if ( (m_longPackageName = getLongPackageName(m_filesList,m_packageName)) == NULL)
		treatErrors(cards::ERROR_ENUM_PACKAGE_NOT_FOUND,m_packageName);

#ifdef DEBUG
	std::cerr << m_longPackageName << " " << m_packageName << std::endl;
#endif

	for (unsigned int nInd=0; nInd < m_filesList->count; nInd++) {
		m_package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(m_packagesList,m_package);
		m_packagesList->pkgs[nInd]->dependences=readDependenciesList(m_filesList,nInd);
	}

	if ( int returnVal = deps_direct (m_filesList,m_packagesList,dependenciesList,m_longPackageName,1) != 0 )
		return returnVal;

	if (dependenciesList ->count > 0) {
		printf("0) %s \n",basename(m_longPackageName));
		for (unsigned int dInd=0; dInd < dependenciesList->count; dInd++) {
			printf("  ");
			int j=1;
			while ( j < dependenciesList->level[dInd]) {
				printf("  ");
				j++;
			}
			printf("%d) %s\n",dependenciesList->level[dInd],basename(m_filesList->items[dependenciesList->depsIndex[dInd]]));
		}
	}

	bool found=false;
	std::string name = "";
	std::set<std::string> localPackagesList, depsPackagesList;

#ifdef DEBUG
	std::cerr << "deptree() FINISH" << std::endl;
#endif
	return 0;
}
