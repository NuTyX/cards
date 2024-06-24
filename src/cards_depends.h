/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "cards_argument_parser.h"
#include "compile_dependencies_utils.h"
#include "string_utils.h"
#include "pkgdbh.h"


#include <locale.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>

struct LevelName {
	unsigned int l;
	std::string name;
};

class cards_depends
{
public:

	cards_depends (const CardsArgumentParser& argParser);
	~cards_depends ();

	void level();

	virtual void treatErrors(cards::ErrorEnum action, const std::string& s) const;

	void showDependencies();

	std::vector<std::string>& getDependencies();
	std::vector<std::string>& getNeededDependencies();

	int deptree();

private:
	/*
	* Populate the List of dependancies for each found package.
	* We need to check if each dependancy exist if yes
	* add the index reference to the depList.
	* If not add it to the missingDepsList.
	*/
	depList* readDependenciesList(itemList *filesList, unsigned int nameIndex);

	void checkConfig();
	int depends();

	std::vector<std::string> m_dependenciesList;
	std::vector<std::string> m_neededDependenciesList;
	std::vector<LevelName>   m_levelList;
	std::set<std::string>    m_missingDepsList;

	const CardsArgumentParser& m_argParser;
	const char* m_packageName;
	char* m_longPackageName;

	cards::conf m_config;
	pkgInfo*  m_package;
	pkgList*  m_packagesList;
	itemList* m_filesList;
};
