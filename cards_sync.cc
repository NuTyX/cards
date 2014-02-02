// cards_sync.cc
//
//  Copyright (c) 2002-2005 Johannes Winkelmann jw at tks6 dot net 
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <cstring>
#include <cstdlib>

#include "file_utils.h"
#include "file_download.h"
#include "config_parser.h"
#include "cards_sync.h"


using namespace std;

const string CardsSync::DEFAULT_REPOFILE = "MD5SUM";
const int CardsSync::DEFAULT_TIMEOUT = 60;

CardsSync::CardsSync (const CardsArgumentParser& argParser)
	: m_argParser(argParser)
{
	m_repoFile = DEFAULT_REPOFILE;
}
CardsSync::CardsSync ( const CardsArgumentParser& argParser,
		const string& url, const string& baseDirectory,
		const string& repoFile)
		: m_baseDirectory(baseDirectory),
		m_remoteUrl(url),
		m_argParser(argParser)
{
	if (repoFile != "") {
		m_repoFile = repoFile;
	} else {
		m_repoFile = DEFAULT_REPOFILE;
	}
}

int CardsSync::exec(ExecType type)
{
	struct stat info;
	if (stat(m_baseDirectory.c_str(), &info)) {
		if (createRecursiveDirs(m_baseDirectory)) {
			cerr << "Failed to create base directory "
				<< m_baseDirectory << endl;
			return -1;
		}
	}

	Config config;
	ConfigParser::parseConfig("/etc/cards.conf", config);
	if (type == TYPE_SYNC) {
		for (unsigned int i = 0; i< config.prtDir.size();++i) {
			cout << " Synchronising: " << config.prtDir[i]<< endl;
			set<string> localPackagesList;
			// Let see what we have so for locally
			localPackagesList=findFile(config.prtDir[i]);

			if (localPackagesList.size() == 0 ) {
				cout << "Still nothing" << endl;
				string category = basename(const_cast<char*>(config.prtDir[i].c_str()));
				string remoteUrl = config.Url + "/" + category;
				FileDownload MD5Sum(remoteUrl,m_repoFile);
				// Build up direcly the directories
				FILE* fp = fopen(m_repoFile.c_str(),"r");
				if (!fp) {
					cerr << "Couldn't open " << m_repoFile << endl;
					return -1;
				}
				string dirToStat;
				char input[512];
				while (fgets(input, 512, fp)) {
					input[strlen(input)-1] = '\0';
					string dir = input + 32 + 1; // md5sum string + ":"
					dirToStat = config.prtDir[i] + "/" + dir;
					createRecursiveDirs(dirToStat);
				}
				fclose(fp);
				continue;
			}
		}
	}
	return 0;	
}
// vim:set ts=2 :
