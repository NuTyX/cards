// cards_argument_parser.cc
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

#include "cards_argument_parser.h"

ArgParser::APCmd CardsArgumentParser::CMD_HELP;
ArgParser::APCmd CardsArgumentParser::CMD_CONFIG;
ArgParser::APCmd CardsArgumentParser::CMD_BASE;
ArgParser::APCmd CardsArgumentParser::CMD_FILES;
ArgParser::APCmd CardsArgumentParser::CMD_QUERY;
ArgParser::APCmd CardsArgumentParser::CMD_SYNC;
ArgParser::APCmd CardsArgumentParser::CMD_INSTALL;
ArgParser::APCmd CardsArgumentParser::CMD_REMOVE;
ArgParser::APCmd CardsArgumentParser::CMD_DEPCREATE;
ArgParser::APCmd CardsArgumentParser::CMD_CREATE;
ArgParser::APCmd CardsArgumentParser::CMD_LIST;
ArgParser::APCmd CardsArgumentParser::CMD_INFO;
ArgParser::APCmd CardsArgumentParser::CMD_DIFF;
ArgParser::APCmd CardsArgumentParser::CMD_LEVEL;
ArgParser::APCmd CardsArgumentParser::CMD_DEPENDS;
ArgParser::APCmd CardsArgumentParser::CMD_DEPTREE;
ArgParser::APCmd CardsArgumentParser::CMD_SEARCH;
ArgParser::APCmd CardsArgumentParser::CMD_PURGE;


ArgParser::APOpt CardsArgumentParser::OPT_FORCE;
ArgParser::APOpt CardsArgumentParser::OPT_UPDATE;

ArgParser::APOpt CardsArgumentParser::OPT_DRY;
ArgParser::APOpt CardsArgumentParser::OPT_ALL;

ArgParser::APOpt CardsArgumentParser::OPT_INSTALLED;
ArgParser::APOpt CardsArgumentParser::OPT_BINARIES;
ArgParser::APOpt CardsArgumentParser::OPT_PORTS;

ArgParser::APOpt CardsArgumentParser::OPT_IGNORE;

ArgParser::APOpt CardsArgumentParser::OPT_REMOVE;


CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_HELP, "help",
		"\tshows help about cards",
"You can also check 'man cards', 'man cards.conf', 'man pkgmk', 'man pkgmk.conf'",
	ArgParser::NONE, 0 , "");

	addCommand(CMD_CONFIG, "config",
		"\tprint info about cards configuration.",
"It can show the defined Directories where alls the packages are located. The locale which are going to be installed, the architecture of your machine, the base system directory and the logfile directory",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_BASE, "base",
		"\treturn to a basic system.",
"You should never use this command unless you knows exactly what it means.\n\n\
It will REMOVE ALL THE PACKAGES not listed in the base system list directory.\n\n\
It is mainly used for the packager.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_FILES, "files",
		"\tlist the file(s) of the installed <package>.",
"",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_SYNC, "sync",
		"\tsynchronize local and remote metadatas.",
"",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_QUERY, "query",
		"\tlist owner(s) of file(s) matching the query",
"",
		ArgParser::EQ, 1 , "<pattern>");

	addCommand(CMD_INFO, "info",
		"\tprint info about a package.",
"If -p or -b are passed as optional arguments, it will be the info of a port or the info of a binary available in the depot",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_LIST, "list",
		"\tlist installed packages.",
"If -p or -b are passed as optional arguments, It will respectively list the local available ports or the remote available binaries.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INSTALL, "install",
		"\tinstall a <package> or a <file>.",
"If the argument is a package name, it will first download the request package, then analyse it and finally download it's dependencies, then analyse them and so on. When all the dependencies are downloaded, they will be installed in the right order then finally the request package will be installed.\n\
If the argument is a file, it will simply installed it. The file can have any name as long it's a valid package.\n\
If -u is passed as optional argument, it will upgrade the package.\n\
If -f is passed as optional argument, it will force the install means overwrite conflicting files.\n",
		ArgParser::EQ, 1, "<package> | <file>");

	addCommand(CMD_REMOVE, "remove",
		"\tremove a package.",
"",
		ArgParser::EQ, 1 , "<package>");

	addCommand(CMD_LEVEL, "level",
		"\tgenerate all the levels.",
"This command is used for the packager. It allow to see which package depends on which dependencie. It is used for the compilation of a port.",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DIFF, "diff",
		"\tlist outdated packages.",
"If -p is passed, the list is checked against founds Pkgfile ports",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPENDS, "depends",
		"\tlist the dependencies of the port.",
"This command is used for the packager. It shows the list of the dependencies of the package. It is used for the compilation of a port.",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_DEPTREE, "deptree",
		"\tlist the dependencies tree of the port.",
"It's mainly use to know if they are no duplicate call of any dependency.",
		ArgParser::EQ, 1,"<port>");

	addCommand(CMD_SEARCH, "search",
		"\tsearch for <expr>.",
"It can be in ports names, in binaries names, in ports description or binaries description. The <expr> must be 2 characters minimum.",
		ArgParser::EQ, 1, "<expr>");

	addCommand(CMD_DEPCREATE, "depcreate",
		"create a package AND it's dependencies from the recept found in the ports.",
"This command is used for the packager. All the dependencies include the final package will be compiled.",
		ArgParser::EQ,1 , "<package>");

	addCommand(CMD_CREATE, "create",
		"\tcreate a package from the recept found in the port.",
"This command is used for the packager. All the dependencies must be compiled, up to date and available for the creation of the final package. If somes dependencies are missing, the command will abort. A compilation logfile can be define in /etc/cards.conf as: logdir /var/log/pkgbuild for exemple.",
		ArgParser::EQ, 1, "<package>");

	addCommand(CMD_PURGE, "purge",
		"\tremove archives from installed packages.",
"This command can be used if you want to save some space on the harddisk. It will delete all the downloads binaries which are located in the binaries sections directories.",
		ArgParser::NONE, 0 , "");

	OPT_FORCE.init("force",
		'f',
		"\tForce install, overwrite conflicting files.");

	OPT_UPDATE.init("upgrade",
		'u',
		"\tUpgrade the package with the same name.");

	OPT_INSTALLED.init("installed",
		'i',
		"\tIncluded the allready installed packages in the list.");

	OPT_BINARIES.init("binaries",
		'b',
		"\tAvailable binaries in depot.");

	OPT_PORTS.init("ports",
		'p',
		"\tAvailable ports.");

	OPT_DRY.init("dry",
		'n',
		"\t\tNot doing the action. Only show (if possible) what will be done.");

	OPT_REMOVE.init("remove",
		'r',
		"\tRemove the packages founds, use with care.");

	OPT_ALL.init("all",
		'a',
		"\t\tRemove the sub package(s) as well, if they exists.");

	OPT_IGNORE.init("ignore",
		'I',
		"\tIgnore errors and list the level anyway.");

	addOption(CMD_LIST, OPT_BINARIES, false);
	addOption(CMD_LIST, OPT_PORTS, false);

	addOption(CMD_INFO,OPT_BINARIES, false);
	addOption(CMD_INFO,OPT_PORTS, false);

	addOption(CMD_DIFF,OPT_PORTS, false);

	addOption(CMD_REMOVE,OPT_ALL, false);

	addOption(CMD_INSTALL,OPT_UPDATE,false);
	addOption(CMD_INSTALL,OPT_FORCE,false);

	addOption(CMD_BASE, OPT_DRY,false);
	addOption(CMD_BASE, OPT_REMOVE,false);

	addOption(CMD_CREATE, OPT_DRY,false);
	addOption(CMD_CREATE, OPT_REMOVE,true);

	addOption(CardsArgumentParser::CMD_DEPENDS, OPT_INSTALLED, false);

	addOption(CardsArgumentParser::CMD_LEVEL,OPT_IGNORE,false);
}
// vim:set ts=2 :
