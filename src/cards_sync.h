//
// cards_sync.h
// 
//  Copyright (c) 2013-2017 by NuTyX team (http://nutyx.org)
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

#ifndef CARDS_SYNC_H
#define CARDS_SYNC_H

#include "file_utils.h"
#include "file_download.h"
#include "cards_argument_parser.h"
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

class Cards_sync
{
public:
	Cards_sync (const CardsArgumentParser& argParser);
	Cards_sync (const CardsArgumentParser& argParser,
		const std::string& url,
		const std::string& directory,
		const std::string& repoFile);
	virtual void treatErrors(const std::string& s) const;

	void run();
	void purge();
	static const std::string DEFAULT_REPOFILE;
 
	
private:

	unsigned int getLocalPackages(const std::string& path);
	unsigned int getRemotePackages(const std::string& pkgrepoFile);

	void deleteFolder(const std::string& folderName);

	std::set<std::string> m_localPackagesList;
	std::set<std::string> m_remotePackagesList;
	const std::string m_baseDirectory;
	const std::string m_remoteUrl;
	std::string m_repoFile;
	error m_actualError;

	const CardsArgumentParser& m_argParser;
	
};
#endif /* CARDS_SYNC_H */
// vim:set ts=2 :
