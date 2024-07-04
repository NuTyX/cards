/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "file_utils.h"

#include <iostream>
#include <cstdlib>
#include <set>
#include <string>
#include <list>
#include <map>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>

// a dependency object consist of: 2
// the index and
// the level
struct IndexLevel{

	// The index Name of the dependency
    unsigned int index;
    // The level of the Dependency
	// 0: No dependency
	// 1: It has some Dependencies from level 0 dependencies
	// 2: It has some Dependencies from level 1 dependencies
	// 3: ...
    int          level;
};

// vector of IndexLevel objects
class dep_list {
	IndexLevel* m_indexlevel;

	unsigned int m_size;
	unsigned int m_capacity;
	// resizing  capacity
	void reserve(unsigned int capacity);

public:

	dep_list();
	~dep_list();

	// vector control and info
	unsigned int size();
	unsigned int capacity();
	void push_back(IndexLevel& indexlevel);
	IndexLevel indexlevel(unsigned int i);

};

// infos around dependencies:
// level, decounting and decrementing level
class pkg_info {
	// the name index of the package
	unsigned int m_nameindex;

	// the level of the package
	int          m_level;

	// number of removed dep when searching them
	unsigned int m_decrement;

	// NO IDEA
	int          m_decount;

	// dependencies list
	// can be empty if no dependencies
	// level is then = 0
	dep_list     m_dependencies;

public:

	pkg_info(unsigned int nameindex);
	pkg_info(unsigned int nameindex,
			int level);
	pkg_info(unsigned int nameindex,
		int level,
		int decount);
	pkg_info(unsigned int nameindex,
		int level,
		int decount,
		unsigned int decrement);




	~pkg_info();

	void nameindex(unsigned int& nameindex);
	void level(int& level);
	void decount(int& decount);
	void decrement(unsigned int& decrement);

	void dependencies(dep_list& dependencies);

};

// vector of pkg_info
class pkg_list {
	pkg_info* m_packages;

	unsigned int m_size;
	unsigned int m_capacity;
	// resizing capacity
	void reserve(unsigned int capacity);

public:

	pkg_list();
	~pkg_list();

	// vector control and info
	unsigned int size();
	unsigned int capacity();
	void push_back(pkg_info& package);
	pkg_info package(unsigned int i);

};
/* depList is a dependances List */
struct depList
{
	unsigned int* depsIndex;
	int* level; /* To show a nice incrementation */
	unsigned int count;
	unsigned int decrement; /* number of removed dep when searching them */
	int decount;
};

/* pkgInfo is the nameindex of a package and the dependencies itemList */
struct pkgInfo
{
	unsigned int nameIndex;
	depList* dependences;
	int level;
};

struct pkgList
{
	pkgInfo** pkgs;
	unsigned int count;
};


/*
 *  depList: Create the list, Add dependence to the list, free the list
 */
depList* initDepsList(void);

void addDepToDepList(depList* list,
	unsigned int nameIndex,
	int level);

void freeDepList(depList *list);

/*** pkgInfo: create the pkg, Add info, free the pkgInfo  ***/
pkgInfo* initPkgInfo(void);

pkgInfo* addInfoToPkgInfo(unsigned int nameIndex);

void freePkgInfo(pkgInfo *package);

/*** pkgList: create the pkgList, add pkgs, free the pkgList ***/
pkgList* initPkgList(void);

void addPkgToPkgList(pkgList* list,
	pkgInfo* package);

void freePkgList(pkgList* packagesList);

int deps_direct (itemList* filesList,
	pkgList* packagesList,
	depList* dependenciesList,
	unsigned int level);

int deps_direct (itemList* filesList,
	pkgList* packagesList,
	depList* dependenciesList,
	const char* pkgName,
	unsigned int level);

void generate_level (pkgList* packagesList,
	unsigned int& level);

char *getLongPackageName(itemList* filesList, const char* packageName);
