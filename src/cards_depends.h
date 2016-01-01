// cards_depends.h
// 
//  Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
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
#ifndef CARDSDEPENDS_H
#define CARDSDEPENDS_H

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
#include "string_utils.h"
#include "pkgdbh.h"
#include "cards_argument_parser.h"
#include "compile_dependencies_utils.h"

struct LevelName {
	int l;
	std::string name;
};

class CardsDepends: public Pkgdbh
{
public:

	CardsDepends (const CardsArgumentParser& argParser, const char * packageName);
	CardsDepends (const CardsArgumentParser& argParser);

	void parseArguments();

	virtual void treatErrors(const std::string& s) const;

	void showDependencies();
	void showLevel();
	
	std::vector<string>& getDependencies();
	std::vector<string>& getNeededDependencies();
	std::vector<LevelName>& getLevel();

	int deptree();

private:
	/*
	* Populate the List of dependance for each found package.
	* We need to check if each dependance exist if yes add the index reference to the list
	* if not printout a warning message
	* We check for TWO deps files, it's need because some binaries need manual add of deps like
	* xorg-server: xorg-font, may be some perl module, some icons theme what ever those are not
	* found automatically
	* kde: all the necessary apps
	* xfce4: same
	*/
	depList *readDependenciesList(itemList *filesList, unsigned int nameIndex);

	int depends();
	int level();

	std::vector<string> m_dependenciesList;
	std::vector<string> m_neededDependenciesList;
	std::vector<LevelName> m_levelList;
	std::set<string> m_missingDepsList;
	
	const CardsArgumentParser& m_argParser;
	const char* m_packageName;
	error m_actualError;
};
#endif
// vim:set ts=2 :
