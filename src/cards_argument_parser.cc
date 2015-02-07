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
ArgParser::APCmd CardsArgumentParser::CMD_CREATE;
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

ArgParser::APOpt CardsArgumentParser::OPT_REMOVE;

ArgParser::APOpt CardsArgumentParser::OPT_OUTOFDATE;


CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_CONFIG, "config",
		"show info about cards configuration.\n\t\t\t\
It can show the defined Directories where alls the packages are located.\n\t\t\t\
the locale which are going to be installed\n\t\t\t\
the architecture of your machine\n\t\t\t\
the base system directory\n\t\t\t\
the logfile directory",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_BASE, "base",
		"return to a basic system.\n\t\t\tIt will remove all packages not listed in the\n\t\t\tBase System list directory.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_SYNC, "sync",
		"synchronize local info with remote repository",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INFO, "info",
		"show info about a package.\n\t\t\t\
It can be an installed one, a port or\n\t\t\t\
a binary available in the depot",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_LIST, "list",
		"show a list of available packages",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INSTALL, "install",
		"install a binary package.\n\t\t\t\
It will first download the request package, then\n\t\t\t\
it will analyse it and download it's dependancies, then\n\t\t\t\
analyse them and so on. When all the dependancies\n\t\t\t\
are downloaded, they will be installed in the right order\n\t\t\t\
then finally the request package will be installed",
		ArgParser::MIN, 1, "<package name>");

	addCommand(CMD_REMOVE, "remove",
		"remove a binary package",
		ArgParser::MIN, 1 , "<package>");

	addCommand(CMD_LEVEL, "level",
		"generate all the levels.\n\t\t\t\
This command is used for the packager.\n\t\t\t\
It allow to see which package depends on which\n\t\t\t\
for the compilation only",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DIFF, "diff",
		"list outdated packages.\n\t\t\t\
It can be checked against ports and binaries",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPENDS, "depends",
		"show dependencies for the port.\n\t\t\t\
This command is used for the packager.\n\t\t\t\
It allow to see which package depends on\n\t\t\t\
the package, for the compilation only",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_DEPTREE, "deptree",
		"show dependencies tree for the port.\n\t\t\t\
It's mainly use to know if they are no duplicate call\n\t\t\t\
of any dependency.",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_SEARCH, "search",
		"search for ports names\n\t\t\tbinaries names or description containing the 'expr'.\n\t\t\tMinimum 2 characters are requierd.",
		ArgParser::EQ, 1, "<expr>");

	addCommand(CMD_CREATE, "create",
		"create the binary.\n\t\t\tAll dependencies must be compiled and up to date\n\t\t\t\
for the creation of the final package without failures.\n\t\t\t\
If dependencies are missing, the command will abort.\n\t\t\t\
A compilation logfile can be define in /etc/cards.conf as:\n\t\t\t\
logdir /var/log/pkgbuild for exemple.",
		ArgParser::EQ, 1, "<port>");

	OPT_FORCE.init("force",
		'f',
		"\toption to force the demand action.");

	OPT_UPDATE.init("update",
		'u',
		"\tupdate the concerned object(s).");

	OPT_INSTALLED.init("installed",
		'i',
		"this option deal with the installed packages");

	OPT_BINARIES.init("binaries",
		'b',
		"\tthis option deal with binaries available in depot");

	OPT_PORTS.init("ports",
		'p',
		"\tthis option deal with availables ports");

	OPT_BASE_HELP.init("info",
		'H',
		"\tget some more info on the command");

	OPT_DRY.init("dry",
		'n',
		"\trun the command without doing the action");

	OPT_REMOVE.init("remove",
		'r',
		"\tremove the packages founds, use with care.");

	OPT_ALL.init("all",
		'a',
		"\tall the concerned objects");

	OPT_OUTOFDATE.init("old",
		'o',
		"\tcheck old packages");

	addOption(CMD_SYNC, OPT_INSTALLED, false);
	addOption(CMD_SYNC, OPT_PORTS, false);
	addOption(CMD_SYNC, OPT_BINARIES, false);


	
	addOption(CMD_LIST, OPT_INSTALLED, false);
	addOption(CMD_LIST, OPT_BINARIES, false);
	addOption(CMD_LIST, OPT_PORTS, false);
	addOption(CMD_LIST, OPT_OUTOFDATE, false);

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

	addOption(CMD_CREATE, OPT_DRY,false);
	addOption(CMD_CREATE, OPT_REMOVE,false);

	addOption(CardsArgumentParser::CMD_DEPENDS, OPT_ALL, false);

	addOption(CardsArgumentParser::CMD_LEVEL,OPT_FORCE,false);
}
// vim:set ts=2 :
