/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "compile_dependencies_utils.h"

dep_list::dep_list() {
}
dep_list::~dep_list() {
}
void dep_list::reserve(unsigned int capacity) {
	if (m_capacity >= capacity)
		return;
	m_capacity = capacity;
	m_indexlevel = (IndexLevel*)realloc(m_indexlevel,
		sizeof(m_indexlevel) * m_capacity);
	if(m_indexlevel == nullptr) {
		printf("Cannot realloc m_indexlevel: %u",&m_indexlevel);
		return;
	}
}
void dep_list::push_back(IndexLevel& indexlevel) {
	if (m_capacity == m_size)
		reserve(m_size*2);
	m_indexlevel[m_size] = indexlevel;
	++m_size;
}
IndexLevel dep_list::indexlevel(unsigned int i) {
	return m_indexlevel[i];
}
unsigned int dep_list::size() {
	return m_size;
}
unsigned int dep_list::capacity() {
	return m_capacity;
}

pkg_info::pkg_info(unsigned int nameindex)
	: m_nameindex(nameindex) {
}
pkg_info::pkg_info
(unsigned int nameindex, int level)
	: m_nameindex(nameindex),
	  m_level(level) {
}
pkg_info::pkg_info
(unsigned int nameindex, int level, int decount)
       : m_nameindex(nameindex),
	 m_level(level),
         m_decount(decount) {
}
pkg_info::pkg_info
(unsigned int nameindex, int level, int decount,
 unsigned int decrement)
       : m_nameindex(nameindex),
	 m_level(level),
         m_decount(decount),
         m_decrement(decrement) {
}

pkg_info::~pkg_info() {
}
void pkg_info::nameindex(unsigned int& nameindex) {
	m_nameindex = nameindex;
}
void pkg_info::level(int& level) {
	m_level = level;
}
void pkg_info::decount(int& decount) {
	m_decount = decount;
}
void pkg_info::decrement(unsigned int& decrement) {
	m_decrement = decrement;
}
void pkg_info::dependencies(dep_list& dependencies) {
	m_dependencies = dependencies;
}

pkg_list::pkg_list() {
}
pkg_list::~pkg_list() {
}
void pkg_list::reserve(unsigned int capacity) {
	if (m_capacity >= capacity)
		return;
	m_capacity = capacity;
	m_packages = (pkg_info*)realloc(m_packages,
		sizeof(m_packages) * m_capacity);
	if(m_packages == nullptr) {
		printf("Cannot realloc m_packages: %u",&m_packages);
		return;
	}
}
void pkg_list::push_back(pkg_info& package) {
	if (m_capacity == m_size)
		reserve(m_size*2);
	m_packages[m_size] = package;
	++m_size;
}
pkg_info pkg_list::package(unsigned int i) {
	return m_packages[i];
}
unsigned int pkg_list::size() {
	return m_size;
}
unsigned int pkg_list::capacity() {
	return m_capacity;
}

depList* initDepsList(void)
{
	depList* list;
	list = (depList*)Malloc(sizeof *list );
	list->depsIndex = (unsigned int*)Malloc(sizeof (*list->depsIndex ));
	list->level     = (int*)Malloc(sizeof (*list->level));
	list->count     = 0;
	list->decount   = 0;
	list->decrement = 0;
	return list;
}

/* Add a reference to PackagesList index and the level of deps to the Dependencies List */
void addDepToDepList(depList *list, unsigned int nameIndex, int level)
{

	list->depsIndex = (unsigned int*)realloc(list->depsIndex,
		sizeof (*list->depsIndex ) * (list->count+1));
	if (list->depsIndex == nullptr) {
		fprintf(stderr,"Failed to realloc %d\n",&list->depsIndex);
		exit(EXIT_FAILURE);
	}
	list->depsIndex[list->count] = nameIndex;

	list->level = (int*)realloc(list->level,
		sizeof (*list->level ) * (list->count+1));
	if (list->level == nullptr) {
		fprintf(stderr,"Failed to realloc %d\n",&list->level);
		exit(EXIT_FAILURE);
	}

	list->level[list->count] = level;
	++list->count;
	list->decount = list->count;
}
void freeDepList(depList *list)
{
	return;
	if (list != nullptr) {
		for (unsigned int i = 0 ; i < list->count;i++) {
			if  (list->level != nullptr) {
				free(list->level);
				list->level = nullptr;
			}
			if (list->depsIndex != nullptr) {
				free(list->depsIndex);
				list->depsIndex = nullptr;
			}
		}
		free(list);
		list = nullptr;
	}
}
pkgInfo* initPkgInfo(void)
{
	pkgInfo *package = nullptr;
	package = (pkgInfo*)Malloc(sizeof *package);
	package->nameIndex = 0;
	package->level     = 0;
	package->dependences = initDepsList();

	return package;
}

pkgInfo*  addInfoToPkgInfo(unsigned int nameIndex)
{
	pkgInfo* package;
	package = initPkgInfo();
	package->nameIndex = nameIndex;
	return package;
}

void freePkgInfo(pkgInfo* package)
{
	if (package != nullptr) {
		if (package->dependences != nullptr) {
			freeDepList(package->dependences);
			package->dependences = nullptr;
			package = nullptr;
		}
	}
}

pkgList* initPkgList(void)
{
	pkgList* list = nullptr;
	list = (pkgList*)Malloc(sizeof *list);
	list->pkgs = (pkgInfo**)Malloc(sizeof *list->pkgs);
	list->count = 0;
	return list;
}

void addPkgToPkgList(pkgList *list, pkgInfo *package)
{
	list->pkgs = (pkgInfo**)realloc(list->pkgs,
		sizeof *list->pkgs * (list->count+1));

	if (list->pkgs == nullptr) {
		fprintf(stderr,"Failed to realloc %d\n",&list->pkgs);
		exit(EXIT_FAILURE);
	}

	list->pkgs[list->count] = package;
	++list->count;
}

void freePkgList(pkgList* packagesList)
{
	if (packagesList != nullptr) {
		for (unsigned int i = 0 ; i < packagesList->count;i++) {
			if (packagesList->pkgs[i] != nullptr) {
				freePkgInfo(packagesList->pkgs[i]);
				packagesList->pkgs[i] = nullptr;
			}
		}
		if (packagesList->pkgs != nullptr) {
			free( packagesList->pkgs);
			packagesList->pkgs = nullptr;
		}
		if (packagesList != nullptr) {
			free(packagesList);
			packagesList = nullptr;
		}
	}
}

/* Get the tree dependencies of the dependencies recursively */
int deps_tree(pkgList* packagesList,
	depList* dependenciesList,
	unsigned int nameIndex,
	unsigned int level)
{
	level++;
	if ( packagesList->pkgs[nameIndex]->dependences->count == 0)
		return 0;

	depList *localDependenciesList = initDepsList();
	for ( unsigned int dInd=0; dInd < packagesList->pkgs[nameIndex]->dependences->count; dInd++)
		addDepToDepList(localDependenciesList,packagesList->pkgs[nameIndex]->dependences->depsIndex[dInd],0);

	for (unsigned int dInd=0;  dInd < localDependenciesList->count; dInd++ ) {
		int found = 0;
		for ( unsigned int fdInd=0; fdInd < dependenciesList->count;fdInd++) {
			if ( dependenciesList->depsIndex[fdInd] == localDependenciesList->depsIndex[dInd] )
				found = 1;
		}
		if ( found == 0) {
			addDepToDepList(dependenciesList,localDependenciesList->depsIndex[dInd],level);
			if ( int retVal = deps_tree(packagesList,dependenciesList,localDependenciesList->depsIndex[dInd],level) !=0)
				return retVal;
		}
	}
	return 0;
}

/* Get the direct dependencies of all the package found and for each direct dependencies we check the deps recursively */
int deps_direct(itemList* filesList,
	pkgList* packagesList,
	depList* dependenciesList,
	unsigned int level)
{
	for (unsigned int nInd=0; nInd < filesList->count;nInd++) {
		if (packagesList->pkgs[nInd]->dependences->count > 0) {
			for(unsigned int dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++) {
				addDepToDepList(dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],level);
				if (level > 0)
					deps_tree(packagesList,dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],level);
			}
		} else {
			return 0;
		}
	}
	return 0;

}

/*
	Get the direct dependencies of the packageName and for each direct dependencies
	we check the deps recursively
*/
int deps_direct(itemList* filesList,
	pkgList* packagesList,
	depList* dependenciesList,
	const char* packageName,
	unsigned int level)
{
	for (unsigned int nInd=0; nInd < filesList->count;nInd++) {
		if ( strcmp(filesList->items[nInd],packageName) == 0 ) {
			if (packagesList->pkgs[nInd]->dependences->count > 0) {
				for(unsigned int dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++) {
					addDepToDepList(dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],level);
					if (level > 0)
						deps_tree(packagesList,dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],level);
				}
			} else
				printf("%s has no deps\n",packageName);
			return 0;
		}
	}
	printf("%s not found\n", packageName);
	return 0;
}

/* Generate the all list of package sorted by level where level = 0 No deps ,
	level = 1 somes deps from level 0 etc
*/
void generate_level(pkgList* packagesList,
	unsigned int& level)
{
	bool found = false;

	/* Pour tous les paquets existants */
	for (unsigned int nameIndex = 0; nameIndex < packagesList->count; nameIndex++) {
		/* Si le paquet nameIndex n'a plus de dépendance son décompte est zero */
		if ( packagesList->pkgs[nameIndex]->dependences->decount == 0 ) {
			found = true;
			/* Ce paquet ne doit plus faire parti au prochain tour. on met donc son decompte à -1 */
			packagesList->pkgs[nameIndex]->dependences->decount = -1 ;
			/* Le level du paquet nameIndex est mis a la valeur de "level" */
			packagesList->pkgs[nameIndex]->level = level;
			/* Pour tous les paquets existants, on cherche dans la liste des dépendances si elle est + grande que 1 */
			for (unsigned int nameIndexHaveThisDep = 0; nameIndexHaveThisDep < packagesList->count; nameIndexHaveThisDep++) {
				/* Et si le paquet [nameIndexHaveThisDep] contient des dépendances */
				if (packagesList->pkgs[nameIndexHaveThisDep]->dependences->decount > 0 ) {
					/* Pour toute les dépendances du paquet [nameIndexHaveThisDep] */
					for (unsigned int depIndex = 0; depIndex < packagesList->pkgs[nameIndexHaveThisDep]->dependences->count;depIndex++) {
						/* Si le Paquet [nameIndex] est présent dans la liste des deps */
						if ( packagesList->pkgs[nameIndexHaveThisDep]->dependences->depsIndex[depIndex] == nameIndex ) {
							/* On incrémente de UN le nombre de dépendances */
							packagesList->pkgs[nameIndexHaveThisDep]->dependences->decrement++;
						}
					}
				}
			}
		}
	}
	/* Pour tous les paquets trouvé */
	for (unsigned int nameIndex = 0; nameIndex < packagesList->count; nameIndex++) {
		/* Si il faut mettre a jour le nombres de deps du paquet [nameIndex] */
		if (packagesList->pkgs[nameIndex]->dependences->decrement > 0) {
			/* Le nouveau nombre de deps = l'actuel - le nombre de deps supprimés */
			packagesList->pkgs[nameIndex]->dependences->decount = packagesList->pkgs[nameIndex]->dependences->decount
				- packagesList->pkgs[nameIndex]->dependences->decrement;
			packagesList->pkgs[nameIndex]->dependences->decrement = 0;
		}
	}
	if (found) {
		level++;
		generate_level(packagesList,level);
	}
}

/* Return the full path with the name including
the name, version and release of the package
*/
char* getLongPackageName(itemList* filesList,
	const char* packageName)
{
	char* name = nullptr;
	bool found = false;
	unsigned int i = 0 ;
	for (i = 0; i < filesList->count;i++) {
		name = strdup(filesList->items[i]);
		if ( strcmp ( basename(name), packageName ) == 0 ) {
			found = true ;
			break;
		}
	}
	if (found)
		return name;

	return nullptr;
}
