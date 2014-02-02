// cards_sync.h
// 
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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
#ifndef CARDS_H
#define CARDS_H

#include <string>
#include <list>
#include <map>

#include <curl/curl.h>
#include "cards_argument_parser.h"

class CardsSync
{
public:
	enum Action
	{ 
		NOP,
		DIR_CREATE,
		MOVE_DIR,
		FILE_GET,
		NEW_FILE_GET,
		REMOVE
	};

	enum ExecType
	{
		TYPE_SYNC,
		TYPE_COPY
	};

	CardsSync (const CardsArgumentParser& argParser);
	CardsSync (const CardsArgumentParser& argParser,
		const std::string& url,
		const std::string& directory,
		const std::string& repoFile);

	int exec ( ExecType execType);

	static const std::string DEFAULT_REPOFILE;
	static const std::string URLINFO;

	static const int DEFAULT_TIMEOUT;
 
	
private:


	const std::string m_baseDirectory;
	const std::string m_remoteUrl;
	std::string m_repoFile;

	const CardsArgumentParser& m_argParser;
	
};
#endif
// vim:set ts=2 :
