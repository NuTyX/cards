//  compile_dependencies_utils.cc
//
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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

depList *initDepsList(void)
{
	depList *list;
	list = (depList*)Malloc(sizeof *list );
	list->depsIndex = (unsigned int*)Malloc(sizeof (*list->depsIndex ));
	list->niveau = (int*)Malloc(sizeof (*list->niveau));
	list->count = 0;
	list->decount = 0;
	list->decrement = 0;
	return list;
}

/* Add a reference to PackagesList index and the level of deps to the Dependencies List */
void addDepToDepList(depList *list, unsigned int nameIndex, int niveau)
{
	 unsigned int  *realloc_tmp;

	realloc_tmp = (unsigned int*)realloc(list->depsIndex, sizeof (*list->depsIndex ) * (list->count+1));
	if (realloc_tmp == NULL) {
		fprintf(stderr,"Failed to realloc %s\n","pkgs");
		exit(EXIT_FAILURE);
	}
	list->depsIndex = realloc_tmp;
	list->depsIndex[list->count] = nameIndex;

	int	*realloc_tmp2 = (int*)realloc(list->niveau, sizeof (*list->niveau ) * (list->count+1));
	list->niveau = realloc_tmp2;
	list->niveau[list->count] = niveau;
	++list->count;
	list->decount = list->count;
}

pkgInfo *initPkgInfo(void)
{
	pkgInfo *package;
	package = (pkgInfo*)Malloc(sizeof *package);
	package->nameIndex = 0;
	package->niveau = 0;
	package->dependences = initDepsList();

	return package;
}

pkgInfo *addInfoToPkgInfo(unsigned int nameIndex)
{
	pkgInfo *package;
	package = initPkgInfo();
	package->nameIndex = nameIndex;
	return package;
}

void freePkgInfo(pkgInfo *package)
{
	free(package);
}

pkgList *initPkgList(void)
{
	pkgList *list = NULL;
	list = (pkgList*)Malloc(sizeof *list);
	list->pkgs = (pkgInfo**)Malloc(sizeof *list->pkgs);
	list->count = 0;
	return list;
}

void addPkgToPkgList(pkgList *list, pkgInfo *package)
{
	pkgInfo **realloc_tmp;

	realloc_tmp = (pkgInfo**)realloc(list->pkgs,
		sizeof *list->pkgs * (list->count+1));

	if (realloc_tmp == NULL) {
		fprintf(stderr,"Failed to realloc %s\n","pkgs");
		exit(EXIT_FAILURE);
	}
	list->pkgs = realloc_tmp;
	list->pkgs[list->count] = package;

	++list->count;
}
void freePkgList(pkgList *packagesList)
{
	for (unsigned int i = 0 ; i < packagesList->count;i++) {
		free(packagesList->pkgs[i]);
	}
	free( packagesList->pkgs);
	free(packagesList);
}

/* Get the tree dependencies of the dependencies recursively */
int deps_tree (pkgList *packagesList, depList *dependenciesList,unsigned int nameIndex, unsigned int niveau)
{
	niveau++;
	if ( packagesList->pkgs[nameIndex]->dependences->count == 0) {
		return 0;
	}
	depList *localDependenciesList = initDepsList();
	for ( unsigned int dInd=0; dInd < packagesList->pkgs[nameIndex]->dependences->count; dInd++) {
		addDepToDepList(localDependenciesList,packagesList->pkgs[nameIndex]->dependences->depsIndex[dInd],0);
	}
	for (unsigned int dInd=0;  dInd < localDependenciesList->count; dInd++ ) {
		int found = 0;
		for ( unsigned int fdInd=0; fdInd < dependenciesList->count;fdInd++) {
			if ( dependenciesList->depsIndex[fdInd] == localDependenciesList->depsIndex[dInd] ) {
				found = 1;
			}
		}
		if ( found == 0) {
			addDepToDepList(dependenciesList,localDependenciesList->depsIndex[dInd],niveau);
			if ( int retVal = deps_tree(packagesList,dependenciesList,localDependenciesList->depsIndex[dInd],niveau) !=0) {
				return retVal;
			}
		}
	}
	return 0;
}

/* Get the direct dependencies of all the package found and for each direct dependencies we check the deps recursively */
int deps_direct (itemList *filesList, pkgList *packagesList, depList *dependenciesList, unsigned int niveau)
{
	for (unsigned int nInd=0; nInd < filesList->count;nInd++) {
		if (packagesList->pkgs[nInd]->dependences->count > 0) {
			for(unsigned int dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++) {
				addDepToDepList(dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],niveau);
				if (niveau > 0) {
					deps_tree (packagesList,dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],niveau);
				}
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
int deps_direct (itemList *filesList, pkgList *packagesList, depList *dependenciesList, const char* packageName, unsigned int niveau)
{
	for (unsigned int nInd=0; nInd < filesList->count;nInd++) {
		if ( strcmp(filesList->items[nInd],packageName) == 0 ) {
			if (packagesList->pkgs[nInd]->dependences->count > 0) {
				for(unsigned int dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++) {
					addDepToDepList(dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],niveau);
					if (niveau > 0) {
						deps_tree (packagesList,dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],niveau);
					}
				}
			} else {
				printf("%s has no deps\n",packageName);
			}
			return 0;
		}
	}
	printf("%s not found\n", packageName);
	return 0;
}

/* 
	Populate the List of dependance for each found package.
	We need to check if each dependance exist if yes add the index reference to the list
	if not printout a warning message
	We check for TWO deps files, it's need because some binaries need manual add of deps like
  xorg-server: xorg-font, may be some perl module, some icons theme what ever those are not
	found automatically
  kde: all the necessary apps
  xfce4: same 
  etc ...
*/	
depList *readDependenciesList(itemList *filesList, unsigned int nameIndex)
{
	if ( nameIndex > filesList->count  ) {
		return NULL;
	}

	depList *dependancesList = initDepsList();

	char *fullPathfileName = (char*)Malloc (sizeof(char)*255);
	char *name = (char*)Malloc(sizeof(char)*255);
	char *depfile1 = (char*)Malloc(sizeof(char)*255);	
	char *depfile2 = (char*)Malloc(sizeof(char)*255);

	sprintf(name,"%s",basename(filesList->items[nameIndex]));
	name[ strchr(name,'@') - name ]='\0';
	
	sprintf(fullPathfileName,"%s/MD5SUM",filesList->items[nameIndex]);

	itemList *nameDeps = initItemList();

	/* We check if any deps file exist */
	
	sprintf(depfile1,"%s.deps",name);
	sprintf(depfile2,"%s.run",name);
	itemList *packageFilesList = initItemList();

	bool found = false;
	if ( (readFileStripSpace(packageFilesList,fullPathfileName)) != 0 ) {
		cout << fullPathfileName << " not exist" << endl;
	} else {
		char *name = NULL;
		for (unsigned int i = 0; i < packageFilesList->count;i++) {
			if ( strchr(packageFilesList->items[i],':') != NULL) {
				name = strchr(packageFilesList->items[i],':');
				name++;
			}
			// If the file <name>.deps is found that's enough
			if (strcmp(name,depfile1) == 0 ) {
					sprintf(fullPathfileName,"%s/%s",filesList->items[nameIndex],name);
					found = true;
					break;
				}
			// If the file <name>.run is found means the .deps wasn't
			if (strcmp(name,depfile2) == 0 ) {
					sprintf(fullPathfileName,"%s/%s",filesList->items[nameIndex],name);
					found = true;
					break;
			}
		}
	}
	freeItemList(packageFilesList);
	if (found) {


		if ( (readFileStripSpace(nameDeps,fullPathfileName)) != 0 ) {
			cout << name << " not found... " << endl;
		} else {
			for (unsigned int i = 0; i < nameDeps->count;i++) {
				unsigned j = 0;
				char *name = (char*)Malloc(sizeof(char)*255);
				for(j = 0; j < filesList->count; j++) {
					sprintf(name,"%s",basename(filesList->items[j]));
					name[ strchr(name,'@') - name ]='\0';
					char * dep = strdup ( nameDeps->items[i]);
					if ( strchr(dep,'.') != NULL) {
						dep[strchr(dep,'.') - dep ]= '\0';
					}
					if ( strchr(dep,'@') != NULL) {
						dep[strchr(dep,'@') - dep ]= '-';
					}
					if (strcmp(dep,name) == 0 ) {
						addDepToDepList(dependancesList,j,0);
						break;
					}
					free(dep);
				}
				if ( j == filesList->count ) {
					printf("WARNING %s from %s NOT FOUND ...\n",
						nameDeps->items[i], filesList->items[nameIndex]);
				}
				free(name);
			}
		}
	}
	freeItemList(nameDeps);
	free(name);
	free(depfile1);
	free(depfile2);
	free(fullPathfileName);
	return dependancesList;
}

/* Generate the all list of package sorted by level where level = 0 No deps , 
	level = 1 somes deps from level 0 etc
*/
int generate_level ( itemList *filesList, pkgList *packagesList, unsigned int niveau)
{
#ifndef NDEBUG
	printf("List des %d paquets\n",packagesList->count);
	for (unsigned int nameIndex = 0; nameIndex < packagesList->count; nameIndex++) {
		printf("%s, Level: %d, Number of deps: %d, Decount: %d \n",filesList->items[nameIndex],
			packagesList->pkgs[nameIndex]->niveau,
			packagesList->pkgs[nameIndex]->dependences->count,
			packagesList->pkgs[nameIndex]->dependences->decount);
	}
#endif
	int found = 0;
#ifndef NDEBUG
	printf("Loop %d\n",niveau);
#endif
	
	/* Pour tous les paquets existants */
	for (unsigned int nameIndex = 0; nameIndex < packagesList->count; nameIndex++) {
		/* Si le paquet nameIndex n'a plus de dépendance son décompte est zero */
		if ( packagesList->pkgs[nameIndex]->dependences->decount == 0 ) {
			found = 1;
#ifndef NDEBUG
			printf(" %d) Paquet: %s\n",niveau,filesList->items[nameIndex]);
#endif
			/* Ce paquet ne doit plus faire parti au prochain tour. on met donc son decompte est -1 */
			packagesList->pkgs[nameIndex]->dependences->decount = -1 ;

			/* Le niveau du paquet nameIndex est mis a la valeur de "niveau" */
			packagesList->pkgs[nameIndex]->niveau = niveau;

			/* Pour tous les paquets existants, on cherche dans la liste des dépendances si elle est + grande que 1 */
			for (unsigned int nameIndexHaveThisDep = 0; nameIndexHaveThisDep < packagesList->count; nameIndexHaveThisDep++) {
				/* Et si le paquet [nameIndexHaveThisDep] contient des dépendances */
				if (packagesList->pkgs[nameIndexHaveThisDep]->dependences->decount > 0 ) {
					/* Pour toute les dépendances du paquet [nameIndexHaveThisDep] */
					for (unsigned int depIndex = 0; depIndex < packagesList->pkgs[nameIndexHaveThisDep]->dependences->count;depIndex++) {
						/* Si le Paquet [nameIndex] est présent dans la liste des deps */
						if ( packagesList->pkgs[nameIndexHaveThisDep]->dependences->depsIndex[depIndex] == nameIndex ) {
							/* On incrémente de UN le nombre de dépendances */
#ifndef NDEBUG
							printf("  %d) for %s. Dependencie of %s  %d > ",
								niveau, filesList->items[nameIndex],filesList->items[nameIndexHaveThisDep],
								packagesList->pkgs[nameIndexHaveThisDep]->dependences->decount);
#endif
							packagesList->pkgs[nameIndexHaveThisDep]->dependences->decrement++;
#ifndef NDEBUG
							printf(" %d\n",packagesList->pkgs[nameIndexHaveThisDep]->dependences->decrement);
#endif
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
#ifndef NDEBUG
			printf(" niveau %d: for %s, increment: %d new niveau: %d \n",
				niveau,filesList->items[nameIndex],
				packagesList->pkgs[nameIndex]->dependences->decrement,packagesList->pkgs[nameIndex]->dependences->decount);
#endif
			packagesList->pkgs[nameIndex]->dependences->decrement = 0;
		}
	}
#ifndef NDEBUG
	printf("\n");
#endif
	if ( found != 0) {
		generate_level (filesList,packagesList,niveau + 1);
	} else
		return niveau;
}

/* Return the full path with the name including 
the name, version and release of the package
*/
char *getLongPackageName(itemList *filesList, const char * packageName)
{
	char *name = NULL;
	bool found = false;
	unsigned int i = 0 ;
	for (i = 0; i < filesList->count;i++) {
		name = strdup(filesList->items[i]);
		name[strchr(name,'@') - name] = '\0';
		if ( strcmp ( basename(name), packageName ) == 0 ) {
			name[strlen(name)]='@';
			found = true ;
			break;
		}
	}
	if (found) {
		return name;
	} else {
		return NULL;
	}
}
// vim:set ts=2 :
