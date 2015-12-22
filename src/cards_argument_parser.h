// cards_argument_parser.h
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

#ifndef CARDS_ARGUMENT_PARSER_H
#define CARDS_ARGUMENT_PARSER_H

#include "argument_parser.h"

class CardsArgumentParser : public ArgParser
{
	public:
		CardsArgumentParser();
		virtual ~CardsArgumentParser() {};

		static ArgParser::APCmd CMD_HELP;
		static ArgParser::APCmd CMD_CONFIG;
		static ArgParser::APCmd CMD_BASE;
		static ArgParser::APCmd CMD_FILES;
		static ArgParser::APCmd CMD_SYNC;
		static ArgParser::APCmd CMD_INFO;
		static ArgParser::APCmd CMD_QUERY;
		static ArgParser::APCmd CMD_LIST;
		static ArgParser::APCmd CMD_INSTALL;
		static ArgParser::APCmd CMD_DEPCREATE;
		static ArgParser::APCmd CMD_CREATE;
		static ArgParser::APCmd CMD_REMOVE;
		static ArgParser::APCmd CMD_DIFF;
		static ArgParser::APCmd CMD_LEVEL;
		static ArgParser::APCmd CMD_DEPENDS;
		static ArgParser::APCmd CMD_DEPTREE;
		static ArgParser::APCmd CMD_SEARCH;
		static ArgParser::APCmd CMD_PURGE;
		static ArgParser::APCmd CMD_UPGRADE;

		static ArgParser::APOpt OPT_CHECK;
		static ArgParser::APOpt OPT_SIZE;

		static ArgParser::APOpt OPT_FORCE;
		static ArgParser::APOpt OPT_UPDATE;

		static ArgParser::APOpt OPT_INSTALLED;
		static ArgParser::APOpt OPT_BINARIES;
		static ArgParser::APOpt OPT_PORTS;

		static ArgParser::APOpt OPT_IGNORE;

		static ArgParser::APOpt OPT_DRY;
		static ArgParser::APOpt OPT_REMOVE;
		static ArgParser::APOpt OPT_ALL;
		static ArgParser::APOpt OPT_LEVEL_SILENCE;

		std::string getAppIdentification() const
		{
			return std::string("cards ")  + VERSION + "\n";
		}
};

#endif
// vim:set ts=2 :
