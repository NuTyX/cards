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

#include "compile_dependencies_utils.h"

void *Malloc(size_t s)
{
	void *p;
	if ( ! ( p = malloc(s) ) )
	{
		fprintf(stderr,"Failled to malloc\n");
		if ( errno )
			perror("malloc");
		exit(EXIT_FAILURE);
	}
	return p;
}
FILE *openFile(const char *fileName)
{
  FILE *fileHandler = NULL;
  if ( (fileHandler = fopen(fileName,"r")) == NULL )
  {
    fprintf(stderr,"Failed to open %s\n",fileName);
    if (errno)
    {
            perror(fileName);
    }
    return NULL;
  }
  return fileHandler;
}

itemList *initItemList(void)
{
	itemList *list = Malloc(sizeof *list);
	list->items = Malloc(sizeof (*list->items));
	list->count = 0;
	return list;
}

void addItemToItemList(itemList *list, const char *item)
{
	char **realloc_tmp;
	realloc_tmp = realloc( list->items, sizeof *list->items * (list->count + 1) );
	
	if ( realloc_tmp == NULL )
		return;
	list->items = realloc_tmp;
	list->items[ list->count ] = strdup(item);
	++list->count;
}

void freeItemList(itemList *list)
{
  unsigned int i;
  for (i=0; i < list->count;i++)
  {
    free(list->items[i]);
  }
  free(list->items);
  free(list);
}

depList *initDepsList(void)
{
	depList *list;
	list = Malloc(sizeof *list );
	list->depsIndex = Malloc(sizeof (*list->depsIndex ));
	list->niveau = Malloc(sizeof (*list->niveau));
	list->count = 0;
	list->decount = 0;
	list->decrement = 0;
	return list;
}

void addDepToDepList(depList *list, unsigned int nameIndex, unsigned int niveau)
{
	unsigned int  *realloc_tmp;

	realloc_tmp = realloc(list->depsIndex, sizeof (*list->depsIndex ) * (list->count+1));
	if (realloc_tmp == NULL)
	{
		fprintf(stderr,"Failed to realloc %s\n","pkgs");
		exit(EXIT_FAILURE);
	}
	list->depsIndex = realloc_tmp;
	list->depsIndex[list->count] = nameIndex;

	realloc_tmp = realloc(list->niveau, sizeof (*list->niveau ) * (list->count+1));
	list->niveau = realloc_tmp;
	list->niveau[list->count] = niveau;
	++list->count;
	list->decount = list->count;
}

depList *readDependenciesList(itemList *listFiles, unsigned int nameIndex)
{
	if ( nameIndex >  listFiles->count  )
		return NULL;
	
	depList *list;

	char *fullPathfileName = calloc (255,sizeof(char));
	sprintf(fullPathfileName,"%s",listFiles->items[nameIndex]);
	FILE *fileHandler = NULL;
	size_t bytesReads = 0, lineLenght = 0;

	char *readingLine = calloc (255,sizeof(char));

	if  ( (fileHandler = openFile(fullPathfileName)) != NULL )
	{
		bytesReads = getline(&readingLine,&lineLenght,fileHandler);
	}
	else
		return NULL;
	if (bytesReads > 0)
	{
		readingLine[bytesReads-1]='\0';
		list = initDepsList();
		int pos = 0, len = strlen(readingLine);

		while ( isspace(readingLine[pos]) != 0 )
			++pos;

		while (pos < len)
		{
			char *start = readingLine + pos;
			char *end = NULL, *ptr = NULL;
			int start_len = strlen(start), end_len = 0;

			if(strchr(start,',') != NULL)
			{
				if (readingLine[pos] == ',' || isspace(readingLine[pos]) != 0)
				{
					++pos;
					continue;
				}
				end = strchr(start,',');
			}
			if (end !=NULL)
				end_len = strlen(end);
			else
				end_len = 0;
			ptr = strndup(start, start_len - end_len);
			if (ptr != NULL )
			{
				unsigned int i;
				for(i = 0; i < listFiles->count; i++)
				{
					if (strcmp(ptr,listFiles->items[i]) == 0 )
					{
						addDepToDepList(list,i,0);
						break;
					}
				}
				if ( i == listFiles->count )
				{
					printf("WARNING %s from %s\n NOT FOUND ...",ptr,listFiles->items[nameIndex]);
					exit(EXIT_FAILURE);
				}
				free(ptr);
			}
			pos += start_len - end_len;
		}
	}
	free(fullPathfileName);
	free(readingLine);
	fclose(fileHandler);
	return list;
}

pkgInfo *initPkgInfo(void)
{
	pkgInfo *package;
	package = Malloc(sizeof *package);
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
}

pkgList *initPkgList(void)
{
	pkgList *list = NULL;
	list = Malloc(sizeof *list);
	list->pkgs = Malloc(sizeof *list->pkgs);
	list->count = 0;
	return list;
}
void addPkgToPkgList(pkgList *list, pkgInfo *package)
{
	pkgInfo **realloc_tmp;
	
	realloc_tmp = realloc(list->pkgs,
        sizeof *list->pkgs * (list->count+1));

	if (realloc_tmp == NULL)
	{
		fprintf(stderr,"Failed to realloc %s\n","pkgs");
		exit(EXIT_FAILURE);
	}
	list->pkgs = realloc_tmp;
	list->pkgs[list->count] = package;

	++list->count;
}

itemList *findFile (const char *path)
{
	itemList *filesList = initItemList();
	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			 if ( strcmp (dir->d_name, ".") && strcmp (dir->d_name, "..") ) // ignore the directories dots
				addItemToItemList(filesList,dir->d_name);
		}
		closedir(d);
	}
	return filesList;
}

// vim:set ts=2 :
