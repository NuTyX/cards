// cards_argument_parser.h
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

#ifndef CARDS_ARGUMENT_PARSER_H
#define CARDS_ARGUMENT_PARSER_H

#include "argument_parser.h"

class CardsArgumentParser : public ArgParser
{
	public:
		CardsArgumentParser();
		virtual ~CardsArgumentParser() {};

		static ArgParser::APCmd CMD_SYNC;
		static ArgParser::APCmd CMD_INFO;
		static ArgParser::APCmd CMD_LIST;
		static ArgParser::APCmd CMD_LISTINST;
		static ArgParser::APCmd CMD_INSTALL;
		static ArgParser::APCmd CMD_DEPINST;
		static ArgParser::APCmd CMD_DIFF;
		static ArgParser::APCmd CMD_LEVEL;
		static ArgParser::APCmd CMD_DEPENDS;
		static ArgParser::APCmd CMD_DEPTREE;
		static ArgParser::APCmd CMD_SEARCH;
		static ArgParser::APCmd CMD_DSEARCH;
		static ArgParser::APCmd CMD_FSEARCH;

		static ArgParser::APOpt OPT_SIGNATURE;
		static ArgParser::APOpt OPT_DEPENDENCIES;

		std::string getAppIdentification() const
		{
			return std::string("cards ")  + VERSION + "\n";
		}
};

#endif
// vim:set ts=2 :
