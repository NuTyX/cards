/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "file_download.h"
#include "repo.h"

class repodwl: public repo {
	std::string m_packageFileName;
	std::string m_packageFileNameSignature;

public:

/**
 * Constructor
 *
 */
	repodwl(const char *fileName);

/**
 * download the packagefileName
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * populater: nothing
 *
 * add: The packageFileName from the mirror
 *
 */
	void downloadPackageFileName(const std::string& packageName);

	bool checkBinaryExist(const std::string& packageName);
	std::string getPackageFileName(const std::string& packageName);
	std::string getPackageFileNameSignature(const std::string& packageName);

	void setPackageFileName(const std::string& packageFileName);
};
