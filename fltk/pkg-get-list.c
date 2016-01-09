#include <stdio.h>
#include <string.h>
#include <pkg-get.h>
int getPkgIndex(pkgList *list, const char *packageName)
{
	int i;

	for (i=0;i<list->count; i++)
	{
		if ( ! strcmp (packageName,list->pkgs[i]->name))
			return i;
	}
	return -1;
	
}
	
int main(int argc, char** argv)
{
	pkgList *List = NULL;
	pkgList *ListInst = NULL;
	mirList *listOfMirrors = NULL;
	pkgInfo *pkg = NULL;
	listOfMirrors = getAvailableMirrors("");
	ListInst = getInstalledPackages();
	List = getAvailablePackages(listOfMirrors);
	addDepsToPkgList(listOfMirrors,List);
	unsigned int i;
	int k;
	// Recherche des paquets installés ds la liste des paquets dispo
	for (i=0;i < ListInst->count; i++)
	{
		k=getPkgIndex(List,ListInst->pkgs[i]->name);
		if (k>-1)
		{
			ListInst->pkgs[i]->status=k;
			List->pkgs[k]->status=1;
		}
		else
			ListInst->pkgs[i]->status=-1; /* Doesn't exist in depot */	
	}
	for (i=0;i < ListInst->count ; i++)
	{
		if ( ListInst->pkgs[i]->status>-1) /* Does it exist in the depot */
		{
			pkg=getNewestPackageFromList(List,ListInst->pkgs[i]->name);
			if (compareVersionString(ListInst->pkgs[i]->version,pkg->version) < 0)
			{
				k=ListInst->pkgs[i]->status;
				List->pkgs[k]->status=2;
			}
		}
	}
	for (i=0;i < List->count ; i++)
	{
		printf("%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s",List->pkgs[i]->status,List->pkgs[i]->name,List->pkgs[i]->version,List->pkgs[i]->description,List->pkgs[i]->md5sum\
		,List->pkgs[i]->url,List->pkgs[i]->maintainer,List->pkgs[i]->packager,List->pkgs[i]->depends);
		printf("\n");
	}
	freePkgList(List);
	freePkgList(ListInst);
	freeMirrorList(listOfMirrors);
	return 0;
}
	
