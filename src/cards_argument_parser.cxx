//
//  cards_argument_parser.cxx
// 
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
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
ArgParser::APCmd CardsArgumentParser::CMD_UPGRADE;

ArgParser::APOpt CardsArgumentParser::OPT_DOWNLOAD_ONLY;
ArgParser::APOpt CardsArgumentParser::OPT_CHECK;
ArgParser::APOpt CardsArgumentParser::OPT_SIZE;

ArgParser::APOpt CardsArgumentParser::OPT_FULL;
ArgParser::APOpt CardsArgumentParser::OPT_FORCE;
ArgParser::APOpt CardsArgumentParser::OPT_UPDATE;

ArgParser::APOpt CardsArgumentParser::OPT_DRY;
ArgParser::APOpt CardsArgumentParser::OPT_ALL;

ArgParser::APOpt CardsArgumentParser::OPT_INSTALLED;
ArgParser::APOpt CardsArgumentParser::OPT_BINARIES;
ArgParser::APOpt CardsArgumentParser::OPT_PORTS;

ArgParser::APOpt CardsArgumentParser::OPT_DISABLE;
ArgParser::APOpt CardsArgumentParser::OPT_IGNORE;
ArgParser::APOpt CardsArgumentParser::OPT_NOLOGENTRY;


ArgParser::APOpt CardsArgumentParser::OPT_REMOVE;

ArgParser::APOpt CardsArgumentParser::OPT_ROOT;
ArgParser::APOpt CardsArgumentParser::OPT_CONFIG_FILE;

CardsArgumentParser::CardsArgumentParser()
{
	addCommand(CMD_HELP, "help",  			//1
		_("shows help about cards."),
_("You can also check 'man cards', 'man cards.conf', 'man pkgmk', 'man pkgmk.conf'."),
	ArgParser::NONE, 0 , "");

	addCommand(CMD_CONFIG, "config",		//2
		_("print info about cards configuration."),
_("It can show the defined Directories where alls the packages are located.\n\
The locale which are going to be installed, the architecture of your machine,\n\
the base system directory and the logfile directory."),
		ArgParser::NONE, 0 , "");

	addCommand(CMD_BASE, "base",			//3
		_("return to a basic system."),
_("It will REMOVE ALL THE PACKAGES not listed in the base system list directory."),
		ArgParser::NONE, 0 , "");

	addCommand(CMD_FILES, "files",			//4
		_("list the file(s) of the installed <package>."),
"",
		ArgParser::EQ, 1 , _("<package>"));

	addCommand(CMD_SYNC, "sync",			//5
		_("synchronize local and remote metadatas."),
"",
		ArgParser::NONE, 0 , "");

	addCommand(CMD_QUERY, "query",			//6
		_("list owner of file(s) matching the query."),
"",
		ArgParser::EQ, 1 , _("<pattern>"));

	addCommand(CMD_INFO, "info",			//7
		_("print info about a package."),
_("If -p or -b are passed as optional arguments, it will be the info of a port or the info of a binary available in the depot."),
		ArgParser::EQ, 1 , _("<package>"));

	addCommand(CMD_LIST, "list",			//8
		_("list installed packages by the user."),
_("If -p is passed as optional argument, it will list the local available ports.\n\
If -b is passed as optional argument, it will list the remote available binaries.\n\
If -F is passed as optional argument, it will list the automatically installed \n\
packages as well."),
		ArgParser::NONE, 0 , "");

	addCommand(CMD_INSTALL, "install",		//9
		_("install one or more <package>, a <file> or a <collection>."),
_("If the argument is a package name, it will first download the request package, \n\
then analyse it and finally download it's dependencies, then analyse them and so on. \n\
When all the dependencies are downloaded, they will be installed in the right order \n\
then finally the request package will be installed.\n\
If the argument is a file, it will simply installed it. \n\
The file can have any name as long it's a valid package.\n\
If -u is passed as optional argument, it will upgrade the package.\n\
If -f is passed as optional argument, it will force the install means overwrite conflicting files."),
		ArgParser::MIN, 1, "<package> | <file> | <collection>");

	addCommand(CMD_REMOVE, "remove",		//10
		_("remove one or more packages or a complete collection."),
_("It can remove one or more packages.\n\
If you want to remove a package from the base list, \
you should pass the -a argument. \n\
If you specify an existing collection, all packages  \
that belong to this collection will be deleted."),
		ArgParser::MIN, 1 , _("<package> | <collection>"));

	addCommand(CMD_LEVEL, "level",			//11
		_("generate all the levels."),
_("This command is used by the packager.\n\
It allows to see which package depends on which dependency.\n\
It is used for the compilation of a port."),
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DIFF, "diff",			//12
		_("list outdated packages."),
_("If -p is passed, the list is checked against founds Pkgfile ports."),
		ArgParser::NONE, 0 , "");

	addCommand(CMD_DEPENDS, "depends",		//13
		_("list the dependencies of the port."),
_("This command is used for the packager.\n\
It shows the list of the dependencies of the package.\n\
It is used for the compilation of a port."),
		ArgParser::EQ, 1,_("<port>"));

	addCommand(CMD_DEPTREE, "deptree",		//14
		_("list the dependencies tree of the port."),
_("It's mainly use to know if they are no duplicate call of any dependency."),
		ArgParser::EQ, 1,_("<port>"));

	addCommand(CMD_SEARCH, "search",		//15
		_("search for <expr>."),
_("Search in your local depot. Searching sequence in categories, collections, set,\n\
name, description, URL, packager and version.\n\
The <expr> must be 2 characters minimum."),
		ArgParser::EQ, 1, "<expr>");

	addCommand(CMD_DEPCREATE, "depcreate",	//16
		_("create a package AND it's dependencies from the recipe found in the ports."),
_("This command is used for the packager.\n\
All the dependencies include the final package will be compiled."),
		ArgParser::EQ,1 , _("<package>"));

	addCommand(CMD_CREATE, "create",		//17
		_("create a package from the recipe found in the port."),
_("This command is used for the packager.\n\
All the dependencies must be compiled, up to date and available for the creation of the final package.\n\
If somes dependencies are missing, the command will abort.\n\
A compilation logfile can be define in /etc/cards.conf as:\n\
logdir /var/log/pkgbuild for example."),
		ArgParser::EQ, 1, _("<package>"));

	addCommand(CMD_PURGE, "purge",			//18
		_("remove archives from installed packages."),
_("This command can be used if you want to save some space on the harddisk.\n\
It will delete all the downloads binaries which are located in the binaries sections directories."),
		ArgParser::NONE, 0 , "");

	addCommand(CMD_UPGRADE, "upgrade",		//19
		_("upgrade you installation with a single command."),
_("This command can upgrade at onces alls your installed out of date packages."),
		ArgParser::NONE, 0 , "");

	OPT_DOWNLOAD_ONLY.init("download-only",
		0,
		_("Only download the binaries that needs to be upgrade."));

	OPT_FULL.init("full",
		'F',
		_("\t   Full list, including automatically installed packages."));

	OPT_FORCE.init("force",
		'f',
		_("\t   Force install, overwrite conflicting files."));

	OPT_UPDATE.init("upgrade",
		'u',
		  _("  Upgrade the package with the same name."));

	OPT_INSTALLED.init("installed",
		'i',
		_("Included the allready installed packages in the list."));

	OPT_BINARIES.init("binaries",
		'b',
		_(" Available binaries in depot server."));

	OPT_PORTS.init("ports",
		'p',
		_("    Local available ports."));

	OPT_DRY.init("dry",
		0,
		_("\t   Not doing the action. Only simulate (if possible) what will be done."));

	OPT_REMOVE.init("remove",
		'r',
		  _("   Remove the packages founds, use with care."));

	OPT_ALL.init("all",
		'a',
		_("\t   Remove the package(s) even if they are in the base list."));

	OPT_IGNORE.init("ignore",
		'I',
		_("   Ignore WARNINGS and list the level anyway."));

	OPT_NOLOGENTRY.init("nolog",
		'n',
		_("    don't add a log entry for this install set."));


	OPT_DISABLE.init("disable",
		'd',
		_("  Ignore Pre and Post install scripts."));

	OPT_SIZE.init("size",
		's',
		_("\t   Just return the number of updates."));

	OPT_CHECK.init("check",
		'c',
		_("\t   Just check if they are some updates."));

	OPT_ROOT.init("root",
		0,
		_("  Specify alternative installation root"),
		true,
		_("<path>"));

	OPT_CONFIG_FILE.init("conf",
		0,
		_("  Specify alternative 'cards.conf' configuration file"),
		true,
		_("<file>"));

	addOption(CMD_CONFIG, OPT_CONFIG_FILE,false);


	addOption(CMD_UPGRADE, OPT_DOWNLOAD_ONLY, false);
	addOption(CMD_UPGRADE, OPT_SIZE, false);
	addOption(CMD_UPGRADE, OPT_CHECK, false);
	addOption(CMD_UPGRADE, OPT_DRY,false);
	addOption(CMD_UPGRADE, OPT_ROOT,false);
	addOption(CMD_UPGRADE, OPT_CONFIG_FILE,false);

	addOption(CMD_LIST, OPT_FULL, false);
	addOption(CMD_LIST, OPT_BINARIES, false);
	addOption(CMD_LIST, OPT_PORTS, false);
	addOption(CMD_LIST, OPT_ROOT,false);
	addOption(CMD_LIST, OPT_CONFIG_FILE,false);

	addOption(CMD_INFO, OPT_BINARIES, false);
	addOption(CMD_INFO, OPT_PORTS, false);
	addOption(CMD_INFO, OPT_ROOT,false);
	addOption(CMD_INFO, OPT_CONFIG_FILE,false);

	addOption(CMD_DIFF, OPT_PORTS, false);

	addOption(CMD_REMOVE, OPT_ALL, false);
	addOption(CMD_REMOVE, OPT_ROOT,false);
	addOption(CMD_REMOVE, OPT_CONFIG_FILE,false);

	addOption(CMD_INSTALL, OPT_DISABLE,false);
	addOption(CMD_INSTALL, OPT_UPDATE,false);
	addOption(CMD_INSTALL, OPT_FORCE,false);
	addOption(CMD_INSTALL, OPT_ROOT,false);
	addOption(CMD_INSTALL, OPT_NOLOGENTRY,false);
	addOption(CMD_INSTALL, OPT_CONFIG_FILE,false);

	addOption(CMD_BASE, OPT_DRY,false);
	addOption(CMD_BASE, OPT_REMOVE,false);
	addOption(CMD_BASE, OPT_ROOT,false);
	addOption(CMD_BASE, OPT_CONFIG_FILE,false);

	addOption(CMD_CREATE, OPT_DISABLE,false);
	addOption(CMD_CREATE, OPT_DRY,false);
	addOption(CMD_CREATE, OPT_REMOVE,true);
	addOption(CMD_CREATE, OPT_ROOT,false);

	addOption(CMD_SYNC,OPT_ROOT,false);

	addOption(CardsArgumentParser::CMD_DEPENDS, OPT_INSTALLED, false);

	addOption(CardsArgumentParser::CMD_LEVEL,OPT_IGNORE,false);
}
// vim:set ts=2 :
