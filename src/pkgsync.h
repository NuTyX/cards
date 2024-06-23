/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "file_utils.h"
#include "file_download.h"
#include "pkgrepo.h"

#include <cstring>
#include <cstdlib>
#include <iostream>


#include <curl/curl.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

class pkgsync
{
public:
	/**
	 * constructors
	 *
	 */
	pkgsync();
	pkgsync(const std::string& url,
			const std::string& directory,
			const std::string& repoFile);

	virtual void treatErrors(const std::string& s) const;

	virtual void run();
	virtual void purge();
	static const std::string DEFAULT_REPOFILE;

	void setRootPath(const std::string& path);
	void setConfigFile(const std::string& file);

	std::set<std::string> getListOfLocalPackages(const std::string& path);
	std::set<std::string> getListOfRemotePackages(const std::string& pkgrepoFile);

private:

	unsigned int getLocalPackages(const std::string& path);
	unsigned int getRemotePackages(const std::string& pkgrepoFile);

	void deleteFolder(const std::string& folderName);

	std::set<std::string> m_localPackagesList;
	std::set<std::string> m_remotePackagesList;
	const std::string m_baseDirectory;
	const std::string m_remoteUrl;
	std::string m_repoFile;
	std::string m_root;
	std::string m_configFile;
	cards::ErrorEnum m_actualError;

	
};
