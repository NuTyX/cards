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

ArgParser::APCmd CardsArgumentParser::CMD_LIST;
ArgParser::APCmd CardsArgumentParser::CMD_INFO;

ArgParser::APCmd CardsArgumentParser::CMD_LEVEL;
ArgParser::APCmd CardsArgumentParser::CMD_DIFF;
ArgParser::APCmd CardsArgumentParser::CMD_DEPENDS;
ArgParser::APCmd CardsArgumentParser::CMD_DEPTREE;

ArgParser::APCmd CardsArgumentParser::CMD_SEARCH;

ArgParser::APOpt CardsArgumentParser::OPT_BASE_HELP;
ArgParser::APOpt CardsArgumentParser::OPT_INSTALL_DEVEL;
ArgParser::APOpt CardsArgumentParser::OPT_INSTALL_DRY;
ArgParser::APOpt CardsArgumentParser::OPT_LIST_INSTALL;
ArgParser::APOpt CardsArgumentParser::OPT_SEARCH_DESCRIPTION;
ArgParser::APOpt CardsArgumentParser::OPT_SEARCH_FILE;
ArgParser::APOpt CardsArgumentParser::OPT_REMOVE_PACKAGES;
ArgParser::APOpt CardsArgumentParser::OPT_PACKAGEFILES;
ArgParser::APOpt CardsArgumentParser::OPT_SYNCALL;
ArgParser::APOpt CardsArgumentParser::OPT_SHOW_ALL_DEPENDENCIES;

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

	addCommand(CMD_LEVEL, "level",
		"generate all the levels",
		ArgParser::NONE, 0 , "");

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

	OPT_LIST_INSTALL.init("inst",
		'i',
		"show a list of installed ports");

	OPT_SEARCH_DESCRIPTION.init("descr",
		'd',
		"show ports containing 'expr' in the name or description");

	OPT_SEARCH_FILE.init("file",
		'f',
		"show file names in footprints matching 'pattern'");

	OPT_BASE_HELP.init("info",
		'H',
		"get some more info on the command");

	OPT_INSTALL_DRY.init("dry",
		'N',
		"run the command without installing");

	OPT_INSTALL_DEVEL.init("dev",
		'D',
		"install the devel subpackages if available");

	OPT_REMOVE_PACKAGES.init("remove",
		'R',
		"remove the packages founds, use with care.");

	OPT_PACKAGEFILES.init("pkgfile",
		'p',
		"get the Pkgfile and others to be able to build the package(s)");

	OPT_SYNCALL.init("bin",
		'b',
		"get the Pkgfile and others to be able to build the package(s) AND the binaries if they are available");

	OPT_SHOW_ALL_DEPENDENCIES.init("all",
    'a',
		"show all the dependencies, included the one allready installed");

	addOption(CMD_BASE, OPT_BASE_HELP,false);
	addOption(CMD_BASE, OPT_REMOVE_PACKAGES,false);

	addOption(CMD_SYNC, OPT_PACKAGEFILES, false);
	addOption(CMD_SYNC, OPT_SYNCALL, false);

	addOption(CMD_LIST,OPT_LIST_INSTALL, false);


	addOption(CMD_SEARCH,OPT_SEARCH_DESCRIPTION, false);
	addOption(CMD_SEARCH,OPT_SEARCH_FILE,false);

	addOption(CMD_INSTALL,OPT_INSTALL_DRY,false);
	addOption(CMD_INSTALL,OPT_INSTALL_DEVEL,false);


	addOption(CMD_DEPENDS, OPT_SHOW_ALL_DEPENDENCIES, false);
}
// vim:set ts=2 :
