// cards_argument_parser.cc
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

#include "cards_argument_parser.h"

ArgParser::APCmd CardsArgumentParser::CMD_SYNC;
ArgParser::APCmd CardsArgumentParser::CMD_INSTALL;
ArgParser::APCmd CardsArgumentParser::CMD_DEPINST;

ArgParser::APCmd CardsArgumentParser::CMD_LIST;
ArgParser::APCmd CardsArgumentParser::CMD_LISTINST;

ArgParser::APCmd CardsArgumentParser::CMD_INFO;

ArgParser::APCmd CardsArgumentParser::CMD_LEVEL;
ArgParser::APCmd CardsArgumentParser::CMD_DIFF;
ArgParser::APCmd CardsArgumentParser::CMD_DEPENDS;
ArgParser::APCmd CardsArgumentParser::CMD_DEPTREE;

ArgParser::APCmd CardsArgumentParser::CMD_SEARCH;
ArgParser::APCmd CardsArgumentParser::CMD_DSEARCH;
ArgParser::APCmd CardsArgumentParser::CMD_FSEARCH;

ArgParser::APOpt CardsArgumentParser::OPT_SIGNATURE;
ArgParser::APOpt CardsArgumentParser::OPT_DEPENDENCIES;

CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_SYNC, "sync",
		"synchronize local info with remote repository",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INFO, "info",
		"show info about a port",
		ArgParser::EQ, 1 , "<port>");

	addCommand(CMD_LIST, "list",
		"show a list of available ports",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_LISTINST, "listinst",
		"show a list of installed ports",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INSTALL, "install",
		"install ports",
		 ArgParser::MIN, 1, "<port1 port2 ...>");

	addCommand(CMD_LEVEL, "level",
		"generate all the levels",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPINST, "depinst",
		"install ports and their dependencies",
		ArgParser::MIN, 1, "<port1 port2 ...>"); 

	addCommand(CMD_DIFF, "diff",
		"list outdated packages (or check args for change)",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPENDS, "depends",
		"show dependencies for the port",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_DEPTREE, "deptree",
		"show dependencies tree for the port",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_SEARCH, "search",
		"show port names containing 'expr'",
		ArgParser::EQ, 1, "<expr>");

	addCommand(CMD_DSEARCH, "dsearch",
		"show ports containing 'expr' in the name or description",
		ArgParser::EQ, 1, "<expr>");

	addCommand(CMD_FSEARCH, "fsearch",
		"show file names in footprints matching 'pattern'",
		ArgParser::EQ, 1, "<pattern>");

	OPT_SIGNATURE.init("signature",
		's',
		"get the signature of the port");

	OPT_DEPENDENCIES.init("dependencies",
		'd',
		"get dependencies files");

	addOption(CMD_SYNC, OPT_SIGNATURE,false);
	addOption(CMD_SYNC, OPT_DEPENDENCIES, false);
}
// vim:set ts=2 :
