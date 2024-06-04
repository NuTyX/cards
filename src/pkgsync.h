//
//  pkgsync.h
//
//  Copyright (c) 2017 - 2020 by NuTyX team (http://nutyx.org)
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
#ifndef PKGSYNC_H
#define PKGSYNC_H

#include "file_utils.h"
#include "file_download.h"
#include "pkgrepo.h"

#include <cstring>
#include <cstdlib>
#include <iostream>


#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>

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

#endif /* PKGSYNC_H */
// vim:set ts=2 :
