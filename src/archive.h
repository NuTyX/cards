/* SPDX-License-Identifier: LGPL-2.1-or-later */

//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - 2024 by NuTyX team (https://nutyx.org)

#pragma once

#include "enum.h"
#include "string_utils.h"

#include <archive.h>
#include <archive_entry.h>

class archive {
public:
	archive(const std::string& fileName);
	virtual ~archive();

	virtual void treatErrors(const std::string& s) const;

	void printDeps(); // Print Out the dependencies
	void printMeta(); // Print Out the .META file
	void printInfo(); // the .INFO file
	void list(); // list the files to stdio

	std::string getPackageName();
	std::string getPackageArch();
	std::vector<std::string> extractFileContent(const char* fileName);
	unsigned int long size(); // Numbers of files in the archive
	std::set<std::string> setofFiles(); // return a order set of string
	std::set<std::string> listofDependencies(); // return an order set of dependencies
	std::set<std::string> listofAlias(); // return the alias list
	std::set<std::pair<std::string, time_t>> listofDependenciesBuildDate(); // return an order set of dependencies,BuildDate
	std::string arch(); // return the arch of the package
	std::string version(); // return the version of the package
	int release(); // return the release of the package
	std::string description(); // return the description	of the package
	std::string url(); // return the url of the package
	std::string contributors(); // return the Contributor(s) of the package
	std::string maintainer(); // return the Maintainer(s) of the package
	std::string collection(); // return the collection of the package
	std::string packager(); // return the Packager(s) of the package
	std::string builddate(); // return the date like Mon Mar 24 10:16:00 2014
	std::string name(); // return the name of the package
	std::string group(); // return the group of the package
	std::string namebuildn(); // return the name + epochvalue
	std::string epochBuildDate(); // return the epochvalue in string format
	time_t buildn(); // return the epoch value

private:
	void getRunTimeDependencies();
	void getRunTimeDependenciesEpoch();
	void getAliasList();

	unsigned int long m_size;

	std::vector<std::string> m_contentMtree;
	std::vector<std::string> m_contentMeta;
	std::vector<std::string> m_contentInfo;

	std::set<std::string> m_rtDependenciesList;
	std::set<std::pair<std::string, time_t>> m_rtDependenciesEpochList;
	std::string m_fileName;
	std::string m_packageName;
	std::string m_packageArch;
	std::set<std::string> m_filesList;
	std::set<std::string> m_aliasList;

	cards::archive_error m_actualError;
};

