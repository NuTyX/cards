/* SPDX-License-Identifier: LGPL-2.1-or-later */

//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - 2024 by NuTyX team (https://nutyx.org)

#pragma once

#include "enum.h"
#include "string_utils.h"

#include <archive.h>
#include <archive_entry.h>

class archive {

	unsigned int long        m_size;

	std::string              m_fileName;
	std::string              m_packageName;
	std::string              m_packageArch;

	std::vector<std::string> m_contentMtree;
	std::vector<std::string> m_contentMeta;

	std::set<std::string>    m_rtDependenciesList;
	std::set<std::string>    m_filesList;
	std::set<std::string>    m_aliasList;
	std::set<std::string>    m_categoriesList;
	std::set<std::string>    m_setsList;

	cards::archive_error     m_actualError;

	std::set<std::pair<std::string, time_t>>
	                         m_rtDependenciesEpochList;


	void getRunTimeDependencies();
	void getRunTimeDependenciesEpoch();
	void getAliasList();
	void getCategoriesList();
	void getSetsList();

public:
	archive(const std::string& fileName);
	virtual ~archive();

	virtual void treatErrors(const std::string& s) const;

	// Print Out the dependencies
	void printDeps();

	// Print Out the .META file
	void printMeta();

	// list the files to stdio
	void list();

	std::string getPackageName();
	std::string getPackageArch();
	void extractFileContent(std::vector<std::string>& list, const char* fileName);

	// Numbers of files in the archive
	unsigned int long size();

	std::set<std::string> setofFiles();
	std::set<std::string> listofDependencies();
	std::set<std::string> listofAlias();
	std::set<std::string> listofCategories();
	std::set<std::string> listofSets();

	// return an order set of dependencies,BuildDate
	std::set<std::pair<std::string, time_t>> listofDependenciesBuildDate();
	std::string arch();
	std::string version();
	int         release();
	std::string description();
	std::string url();
	std::string contributors();
	std::string maintainer();
	std::string collection();
	std::string license();
	std::string packager();

	// return the date like Mon Mar 24 10:16:00 2014
	std::string builddate();

	std::string name();
	std::string group();

	// return the name + epochvalue
	std::string namebuildn();

	// return the epochvalue in string format
	std::string epochBuildDate();

	// return the epoch value
	time_t build();

};

