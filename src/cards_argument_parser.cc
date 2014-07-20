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

ArgParser::APCmd CardsArgumentParser::CMD_CONFIG;
ArgParser::APCmd CardsArgumentParser::CMD_BASE;
ArgParser::APCmd CardsArgumentParser::CMD_SYNC;
ArgParser::APCmd CardsArgumentParser::CMD_INSTALL;
ArgParser::APCmd CardsArgumentParser::CMD_REMOVE;

ArgParser::APCmd CardsArgumentParser::CMD_LIST;
ArgParser::APCmd CardsArgumentParser::CMD_INFO;
ArgParser::APCmd CardsArgumentParser::CMD_DIFF;

ArgParser::APCmd CardsArgumentParser::CMD_LEVEL;
ArgParser::APCmd CardsArgumentParser::CMD_DEPENDS;
ArgParser::APCmd CardsArgumentParser::CMD_DEPTREE;

ArgParser::APCmd CardsArgumentParser::CMD_SEARCH;

ArgParser::APOpt CardsArgumentParser::OPT_BASE_HELP;

ArgParser::APOpt CardsArgumentParser::OPT_FORCE;
ArgParser::APOpt CardsArgumentParser::OPT_UPDATE;

ArgParser::APOpt CardsArgumentParser::OPT_DRY;
ArgParser::APOpt CardsArgumentParser::OPT_ALL;

ArgParser::APOpt CardsArgumentParser::OPT_INSTALLED;
ArgParser::APOpt CardsArgumentParser::OPT_BINARIES;
ArgParser::APOpt CardsArgumentParser::OPT_PORTS;

ArgParser::APOpt CardsArgumentParser::OPT_SEARCH_FILE;

ArgParser::APOpt CardsArgumentParser::OPT_REMOVE;

CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_CONFIG, "config",
		"show info about cards configuration",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_BASE, "base",
		"return to a basic system, will remove all packages not listed in the Base System list directory",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_SYNC, "sync",
		"synchronize local info with remote repository",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INFO, "info",
		"show info about a package",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_LIST, "list",
		"show a list of available packages",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INSTALL, "install",
		"install a binary package",
		ArgParser::MIN, 1, "<package name>");

	addCommand(CMD_REMOVE, "remove",
		"remove a binary package",
		ArgParser::MIN, 1 , "<package>");

	addCommand(CMD_LEVEL, "level",
		"generate all the levels",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DIFF, "diff",
		"list outdated packages",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPENDS, "depends",
		"show dependencies for the port",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_DEPTREE, "deptree",
		"show dependencies tree for the port",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_SEARCH, "search",
		"search for ports names, binaries names or description containing the 'expr'. Minimum 2 characters are requierd. ",
		ArgParser::EQ, 1, "<expr>");

	OPT_FORCE.init("force",
		'f',
		"  option to force the demand action.");

	OPT_UPDATE.init("update",
		'u',
		" update the concerned object(s).");

	OPT_INSTALLED.init("installed",
		'i',
		" this option deal with the installed packages");

	OPT_BINARIES.init("binaries",
		'b',
		"  this option deal with binaries available in depot");

	OPT_PORTS.init("ports",
		'p',
		"     this option deal with availables ports");

	OPT_SEARCH_FILE.init("file",
		'F',
		"show file names in footprints matching 'pattern'");

	OPT_BASE_HELP.init("info",
		'H',
		"   get some more info on the command");

	OPT_DRY.init("dry",
		'n',
		"    run the command without doing the action");

	OPT_REMOVE.init("remove",
		'r',
		" remove the packages founds, use with care.");

	OPT_ALL.init("all",
		'a',
		"    all the concerned objects");

	addOption(CMD_SYNC, OPT_INSTALLED, false);
	addOption(CMD_SYNC, OPT_PORTS, false);
	addOption(CMD_SYNC, OPT_BINARIES, false);


	
	addOption(CMD_LIST,OPT_INSTALLED, false);
	addOption(CMD_LIST,OPT_BINARIES, false);
	addOption(CMD_LIST,OPT_PORTS, false);

	addOption(CMD_INFO,OPT_INSTALLED, false);
	addOption(CMD_INFO,OPT_BINARIES, false);
	addOption(CMD_INFO,OPT_PORTS, false);

	addOption(CMD_DIFF,OPT_BINARIES, false);
	addOption(CMD_DIFF,OPT_PORTS, false);

	addOption(CMD_REMOVE,OPT_ALL, false);
	addOption(CMD_REMOVE,OPT_DRY, false);

	addOption(CMD_INSTALL,OPT_UPDATE,false);
	addOption(CMD_INSTALL,OPT_FORCE,false);
	addOption(CMD_INSTALL,OPT_DRY,false);
	addOption(CMD_INSTALL,OPT_ALL,false);

	addOption(CMD_BASE, OPT_DRY,false);
	addOption(CMD_BASE, OPT_REMOVE,false);
	addOption(CMD_BASE, OPT_BASE_HELP,false);
	
	addOption(CardsArgumentParser::CMD_DEPENDS, OPT_ALL, false);
}
// vim:set ts=2 :
