//  pkgcrea.c
// 
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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

int deps_recursive (itemList *filesList, pkgList *packagesList, depList *dependenciesList, unsigned int nameIndex,  unsigned int niveau)
{
	niveau++;
	int retVal = 0;
	unsigned int dInd, fdInd;
	if ( packagesList->pkgs[nameIndex]->dependences->count == 0)
	{
		return 0;
	}
	depList *localDependenciesList = initDepsList();
	for ( dInd=0; dInd < packagesList->pkgs[nameIndex]->dependences->count; dInd++)	
			addDepToDepList(localDependenciesList,packagesList->pkgs[nameIndex]->dependences->depsIndex[dInd],0);
	for ( dInd=0; dInd < localDependenciesList->count; dInd++)
	{
		int found = 0;
		for ( fdInd=0; fdInd < dependenciesList->count;fdInd++)
		{
			if ( dependenciesList->depsIndex[fdInd] == localDependenciesList->depsIndex[dInd] )
				found = 1;
		}
		if ( found == 0)
		{
			addDepToDepList(dependenciesList,localDependenciesList->depsIndex[dInd],niveau);
				retVal = deps_recursive(filesList,packagesList,dependenciesList,localDependenciesList->depsIndex[dInd],niveau);
				unsigned int tmpNameIndex = 0;
				unsigned int fdInd = 0;
				while (fdInd < dependenciesList->count)
				{
						if ( dependenciesList->depsIndex[fdInd] == packagesList->pkgs[nameIndex]->dependences->depsIndex[dInd]  && tmpNameIndex == 0 )
						{
									tmpNameIndex = dependenciesList->depsIndex[fdInd];
						}
						if ( tmpNameIndex != 0 && ( fdInd + 1 < dependenciesList->count ))
						{
									dependenciesList->depsIndex[fdInd] = dependenciesList->depsIndex[fdInd + 1];
						}
					++fdInd;
				}

				if ( tmpNameIndex != 0 )
					dependenciesList->depsIndex[dependenciesList->count -1 ] = tmpNameIndex;
		}
	}

	return 0;
}
int deps_tree (pkgList *packagesList, depList *dependenciesList,unsigned int nameIndex, unsigned int niveau)
{
	niveau++;
	int retVal = 0;
	unsigned int dInd, fdInd;
	if ( packagesList->pkgs[nameIndex]->dependences->count == 0)
		return 0;
	depList *localDependenciesList = initDepsList();
	for ( dInd=0; dInd < packagesList->pkgs[nameIndex]->dependences->count; dInd++)
		addDepToDepList(localDependenciesList,packagesList->pkgs[nameIndex]->dependences->depsIndex[dInd],0);
	for ( dInd=0; dInd < localDependenciesList->count; dInd++)
	{
		int found = 0;
		for ( fdInd=0; fdInd < dependenciesList->count;fdInd++)
		{
			if ( dependenciesList->depsIndex[fdInd] == localDependenciesList->depsIndex[dInd] )
//				addDepToDepList(dependenciesList,packagesList->pkgs[nameIndex]->dependences->depsIndex[dInd],0);
				found = 1;
		}
		if ( found == 0)
		{
				addDepToDepList(dependenciesList,localDependenciesList->depsIndex[dInd],niveau);
				retVal = deps_tree(packagesList,dependenciesList,localDependenciesList->depsIndex[dInd],niveau);
		}
	}
	return 0;
}
int deps_direct (itemList *filesList, pkgList *packagesList, depList *dependenciesList,const char* pkgName, unsigned int niveau)
{
	unsigned int nInd,dInd;
  for (nInd=0; nInd < filesList->count;nInd++)
	{
		if ( strcmp(filesList->items[nInd],pkgName) == 0 )
		{
			if (packagesList->pkgs[nInd]->dependences->count > 0)
			{
				for(dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++)
				{
					
					addDepToDepList(dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],niveau);
					if (niveau > 0)
					{
						deps_tree (packagesList,dependenciesList,packagesList->pkgs[nInd]->dependences->depsIndex[dInd],niveau);
					}
				}
			}
			else
			{
				printf("%s has no deps\n");
			}
			return 0;
		}
	}
	printf("%s not found\n");
	return 0;
}

int generate_niveaux ( itemList *filesList, pkgList *packagesList, unsigned int niveau)
{
	unsigned int nameIndex, depIndex, nameIndexHaveThisDep;
	int found = 0;
	int retVal = 0;
#ifndef NDEBUG
	printf("Niveau %d: \n",niveau);
#endif
	/* Pour tous les paquets existants */
	for (nameIndex = 0; nameIndex < packagesList->count; nameIndex++)
	{
		/* Si le paquet nameIndex n'a plus de dépendance son décompte est à zero */

		if ( packagesList->pkgs[nameIndex]->dependences->decount == 0 )
		{
				found = 1;
#ifndef NDEBUG
				printf(" %d) Paquet: %s\n",niveau,filesList->items[nameIndex]);
#endif
				/* Ce paquet ne doit plus faire parti au prochain tour. on met donc son décompte à -1 */
				packagesList->pkgs[nameIndex]->dependences->decount = -1 ;

				/* Le niveau du paquet nameIndex est mis à la valeur de "niveau" */
				packagesList->pkgs[nameIndex]->niveau = niveau;

				/* Pour tous les paquets existants, on cherche dans la liste des dépendances si elle est + grande que 1 */
				for (nameIndexHaveThisDep = 0; nameIndexHaveThisDep < packagesList->count; nameIndexHaveThisDep++)
				{
					/* Et si le paquet [nameIndexHaveThisDep] contient des dépendances */
					if (packagesList->pkgs[nameIndexHaveThisDep]->dependences->decount > 0 )
					{
						/* Pour toute les dépendances du paquet [nameIndexHaveThisDep] */
						for (depIndex = 0; depIndex < packagesList->pkgs[nameIndexHaveThisDep]->dependences->count;depIndex++)
						{
							/* Si le Paquet [nameIndex] est présent dans la liste des deps */
							if ( packagesList->pkgs[nameIndexHaveThisDep]->dependences->depsIndex[depIndex] == nameIndex )
							{
								/* On incrémente de UN le nombre de dépendances */
#ifndef NDEBUG
								printf("  %d) for %s. Dep: %s  %d > ",niveau, filesList->items[nameIndex],filesList->items[nameIndexHaveThisDep],packagesList->pkgs[nameIndexHaveThisDep]->dependences->decount);
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
	for (nameIndex = 0; nameIndex < packagesList->count; nameIndex++)
	{
		/* Si il faut mettre à jour le nombres de deps du paquet [nameIndex] */
		if (packagesList->pkgs[nameIndex]->dependences->decrement > 0)
		{
			/* Le nouveau nombre de deps = l'actuel - le nombre de deps supprimés */
			packagesList->pkgs[nameIndex]->dependences->decount = packagesList->pkgs[nameIndex]->dependences->decount - packagesList->pkgs[nameIndex]->dependences->decrement;
#ifndef NDEBUG
			printf(" niveau %d: for %s, increment: %d new niveau: %d \n",niveau,filesList->items[nameIndex], packagesList->pkgs[nameIndex]->dependences->decrement,packagesList->pkgs[nameIndex]->dependences->decount);
#endif
			packagesList->pkgs[nameIndex]->dependences->decrement = 0;
		}
	}
	retVal=niveau;
#ifndef NDEBUG
	printf("\n");
#endif
	if ( found == 0)
	{	
		return niveau;
	}
	else
		generate_niveaux (filesList,packagesList,niveau + 1);
}

int main (int argc, char* argv[])
{
	unsigned int nInd,dInd;
	itemList *filesList;

	filesList = findFile("pakxe/");

	int niveau= 0;
	pkgInfo *package = NULL;
	pkgList *packagesList = initPkgList();

	for (nInd=0;nInd <filesList->count;nInd++)
	{
		package = addInfoToPkgInfo(nInd);
		addPkgToPkgList(packagesList,package);
		packagesList->pkgs[nInd]->dependences=readDependenciesList(filesList,nInd);
	}

	 niveau = generate_niveaux (filesList,packagesList,0);
	
	printf("Number of niveaux: %d\n",niveau);
	if ( argc < 2) /* Printout all the packages with they dependencies */
	{
		for (nInd = 0; nInd<packagesList->count;nInd++)
		{
			printf("%s has %d deps:\n",filesList->items[nInd], packagesList->pkgs[nInd]->dependences->count);
			for(dInd=0;dInd < packagesList->pkgs[nInd]->dependences->count;dInd++)
				printf("%d) %s\n", dInd + 1, filesList->items[packagesList->pkgs[nInd]->dependences->depsIndex[dInd]]);
		}
		return 0;
	}
	depList *dependenciesList = NULL;
	dependenciesList = initDepsList();
	if ( argc == 2 ) /* Printout the direct dependencies of the request packagename */
	{
		int returnVal;
		returnVal = deps_direct (filesList,packagesList,dependenciesList,argv[1],0);
		if (dependenciesList ->count > 0)
		{
			for (dInd=0; dInd < dependenciesList ->count; dInd++)
				printf("%d) %s\n",dInd + 1,filesList->items[dependenciesList->depsIndex[dInd]]); 
		}
		return returnVal;
	}
	if (argc == 3 ) /* Printout the dependencies of the request packagename */
	{
		int returnVal = 0;
		int choice = 0;
		if (strcmp(argv[1],"depends") == 0)
			choice = 1;
		if (strcmp(argv[1],"deptree") == 0)
			choice = 2;
		if (strcmp (argv[1],"level") == 0)
			choice = 3;
		unsigned int nameIndex;
		switch (choice)
		{
				case 1 :
					for (nameIndex=0; nameIndex< filesList->count;nameIndex++)
					{
							
							if ( strcmp(filesList->items[nameIndex],argv[2]) == 0 )
							{
								printf("%s has followings deps:\n",argv[2]);
								returnVal = deps_recursive (filesList,packagesList,dependenciesList,nameIndex,1);
								
								break;
							}
					}
					if ( nameIndex == filesList->count )
					{
							printf("%s not found\n",argv[2]);
							return 0;
					}
					if (dependenciesList ->count > 0)
					{
						for (dInd=0; dInd < dependenciesList ->count; dInd++)
						{
							printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
						}
					}
					 printf("%s \n",argv[2]);
					break;
				case 2 :
					returnVal = deps_direct (filesList,packagesList,dependenciesList,argv[2],1);
					if (dependenciesList ->count > 0)
					{
						printf("0) %s \n",argv[2]);
						for (dInd=0; dInd < dependenciesList ->count; dInd++)
						{
							printf("  ");
							unsigned int j=1;
							while ( j < dependenciesList->niveau[dInd])
							{
								printf("  ");
							j++;
							}
							printf("%d) %s\n",dependenciesList->niveau[dInd],filesList->items[dependenciesList->depsIndex[dInd]]);
						}
					}
					break;
				case 3:
					returnVal = deps_direct (filesList,packagesList,dependenciesList,argv[2],1);
					if (dependenciesList ->count > 0)
					{
						int currentNiveau = 0;
						while ( currentNiveau <= niveau)
						{
							 printf("Niveau: %d\n",currentNiveau);
							for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
								if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == currentNiveau )
									printf("  %s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
									currentNiveau++;
						}
/*						int niveau = 0;
						printf("Niveau: %d\n",niveau);
						for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
							if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
								printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
						printf("\n");
						niveau++;
						printf("Niveau: %d\n",niveau);
						for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
							if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
								printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
						printf("\n");
						niveau++;
						printf("Niveau: %d\n",niveau);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
						printf("\n");
						niveau++;
						printf("Niveau: %d\n",niveau);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            niveau++;
						printf("Niveau: %d\n",niveau);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            niveau++;
						printf("Niveau: %d\n",niveau);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            niveau++;
						printf("Niveau: %d\n",niveau);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            niveau++;
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
       			niveau++;
						printf("Niveau: %d\n",niveau);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);            niveau++;
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
            for ( dInd=0; dInd < dependenciesList ->count; dInd++ )
              if ( packagesList->pkgs[dependenciesList->depsIndex[dInd]]->niveau == niveau )
                printf("%s\n",filesList->items[dependenciesList->depsIndex[dInd]]);
			*/		}
					break;
				default:
					return 0;
					break;
		}
	}
	free(filesList);
	return 0;
	
}
// vim:set ts=2 :
