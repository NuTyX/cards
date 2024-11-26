/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "cards_argument_parser.h"

ArgParser::APOpt CardsArgumentParser::OPT_DOWNLOAD_ONLY;
ArgParser::APOpt CardsArgumentParser::OPT_DOWNLOAD_READY;
ArgParser::APOpt CardsArgumentParser::OPT_CHECK;
ArgParser::APOpt CardsArgumentParser::OPT_SIZE;
ArgParser::APOpt CardsArgumentParser::OPT_NO_SYNC;

ArgParser::APOpt CardsArgumentParser::OPT_FULL;
ArgParser::APOpt CardsArgumentParser::OPT_FORCE;
ArgParser::APOpt CardsArgumentParser::OPT_UPDATE;

ArgParser::APOpt CardsArgumentParser::OPT_DRY;

ArgParser::APOpt CardsArgumentParser::OPT_INSTALLED;
ArgParser::APOpt CardsArgumentParser::OPT_BINARIES;
ArgParser::APOpt CardsArgumentParser::OPT_SETS;

ArgParser::APOpt CardsArgumentParser::OPT_DISABLE;
ArgParser::APOpt CardsArgumentParser::OPT_NO_META;

ArgParser::APOpt CardsArgumentParser::OPT_NOLOGENTRY;

ArgParser::APOpt CardsArgumentParser::OPT_REMOVE;

ArgParser::APOpt CardsArgumentParser::OPT_ROOT;
ArgParser::APOpt CardsArgumentParser::OPT_CONFIG_FILE;

ArgParser::APOpt CardsArgumentParser::OPT_NAMES;

CardsArgumentParser::CardsArgumentParser()
{
    addCommand(CMD_HELP, "help",
        _("shows help about cards."),
        _("You can also check 'man cards', 'man cards.conf', 'man pkgmk', 'man pkgmk.conf'."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_CONFIG, "config",
        _("print info about cards configuration."),
        _("It can show the defined Directories where alls the packages are located.\n\
The locale which are going to be installed, the architecture of your machine,\n\
the base system directory and the logfile directory."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_FILES, "files",
        _("list the file(s) of the installed <package>."),
        "",
        ArgParser::EQ, 1, _("<package>"));

    addCommand(CMD_SYNC, "sync",
        _("synchronize local and remote metadatas."),
        "",
        ArgParser::NONE, 0, "");

    addCommand(CMD_QUERY, "query",
        _("list owner of file(s) matching the query."),
        "",
        ArgParser::EQ, 1, _("<pattern>"));

    addCommand(CMD_INFO, "info",
        _("print info about a package."),
        _("print details of an available package on the remote server."),
        ArgParser::EQ, 1, _("<package>"));

    addCommand(CMD_LIST, "list",
        _("list packages."),
        _("It will list available packages on the remote server."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_SEARCH, "search",
        _("search for <expr>."),
        _("Search in your local depot. Searching sequence in categories, collections, set,\n\
name, description, URL, packager and version.\n\
The <expr> must be 2 characters minimum."),
        ArgParser::EQ, 1, "<expr>");

    addCommand(CMD_INSTALL, "install",
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

    addCommand(CMD_REMOVE, "remove",
        _("remove one or more packages or a complete collection."),
        _("It can remove one or more packages.\n\
If you specify an existing collection, all packages \
that belong to this collection will be deleted."),
        ArgParser::MIN, 1, _("<package> | <collection>"));

    addCommand(CMD_UPGRADE, "upgrade",
        _("upgrade you installation with a single command."),
        _("This command can upgrade at onces alls your installed out of date packages."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_PURGE, "purge",
        _("remove archives from installed packages."),
        _("This command can be used if you want to save some space on the harddisk.\n\
It will delete all the downloads binaries which are located in the binaries sections directories."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_DIFF, "diff",
        _("list outdated packages."),
        _("An outdated package is a installed package older then the one in the depot."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_LEVEL, "level",
        _("generate all the levels."),
        _("This command is used by the packager.\n\
It allows to see which package depends on which dependency.\n\
It is used for the compilation of a port."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_DEPENDS, "depends",
        _("list the dependencies of the port."),
        _("This command is used for the packager.\n\
It shows the list of the dependencies of the package.\n\
It is used for the compilation of a port."),
        ArgParser::EQ, 1, _("<port>"));

    addCommand(CMD_DEPTREE, "deptree",
        _("list the dependencies tree of the port."),
        _("It's mainly use to know if they are no duplicate call of any dependency."),
        ArgParser::EQ, 1, _("<port>"));

    addCommand(CMD_CREATE, "create",
        _("create a package from the recipe found in the port."),
        _("This command is used for the packager.\n\
All the dependencies must be compiled, up to date and available for the creation of the final package.\n\
If somes dependencies are missing, the command will abort.\n\
A compilation logfile can be define in /etc/cards.conf as:\n\
logdir /var/log/pkgbuild for example."),
        ArgParser::EQ, 1, _("<package>"));

    addCommand(CMD_DEPENDENT, "dependent",
        _("list of ports using this port."),
        _("This command is used for the packager.\n\
It shows the list of the reversed dependencies of the package.\n\
It is used when checking which port is using this port."),
        ArgParser::EQ, 1, _("<port>"));

    addCommand(CMD_KEYS, "keys",
        _("generate the keys pair."),
        _("This command is used to generate the keys needed for\n\
signing and verifying the packages.\n\
This command is normally used by the packager only."),
        ArgParser::NONE, 0, "");

    addCommand(CMD_REPO, "repo",
        _("verify/generate locale repositories meta data."),
        _("Without private key: this command is used to verify signatures of all the packages.\n\
With a found private key: It will generate repositories meta data.\n\
Location of the keys is defined in cards.conf file.\n\
As for the generation, a private key is needed, it is used by the packager only."),
        ArgParser::MIN, 0, "");

    OPT_NO_SYNC.init("no-sync",
        0,
        _("\t   Only upgrade what's possible."));

    OPT_DOWNLOAD_READY.init("ready",
        0,
        _("\t   Only check if all binaries that are needed are downloaded."));

    OPT_DOWNLOAD_ONLY.init("download-only",
        0,
        _("Only download the binaries that are needed to be upgraded."));

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

    OPT_SETS.init("sets",
        'S',
        _("     Available sets of packages in depot server."));

    OPT_DRY.init("dry",
        0,
        _("\t   Not doing the action. Only simulate (if possible) what will be done."));

    OPT_NO_META.init("no-meta",
        0,
        _("\t   Do not tracks meta-datas of installed packages."));

    OPT_REMOVE.init("remove",
        'r',
        _("   Remove the packages founds, use with care."));

    OPT_NOLOGENTRY.init("nolog",
        0,
        _("    don't add a log entry for this install set."));

    OPT_DISABLE.init("disable",
        'd',
        _("  Ignore Pre and Post install scripts."));

    OPT_SIZE.init("size",
        's',
        _("\t   Return the number of updates."));

    OPT_CHECK.init("check",
        'c',
        _("\t   Check if they are some updates."));

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

    OPT_NAMES.init("name",
        'n',
        _("\t   Search only in the name of the package."));

    addOption(CMD_CONFIG, OPT_CONFIG_FILE, false);

    addOption(CMD_UPGRADE, OPT_DOWNLOAD_READY, false);
    addOption(CMD_UPGRADE, OPT_DOWNLOAD_ONLY, false);
    addOption(CMD_UPGRADE, OPT_NO_SYNC, false);
    addOption(CMD_UPGRADE, OPT_SIZE, false);
    addOption(CMD_UPGRADE, OPT_CHECK, false);
    addOption(CMD_UPGRADE, OPT_DRY, false);
    addOption(CMD_UPGRADE, OPT_ROOT, false);
    addOption(CMD_UPGRADE, OPT_CONFIG_FILE, false);

    addOption(CMD_LIST, OPT_BINARIES, false);
    addOption(CMD_LIST, OPT_SETS, false);
    addOption(CMD_LIST, OPT_CONFIG_FILE, false);

    addOption(CMD_INFO, OPT_BINARIES, false);
    addOption(CMD_INFO, OPT_SETS, false);
    addOption(CMD_INFO, OPT_CONFIG_FILE, false);

    addOption(CMD_SEARCH, OPT_NAMES, false);

    addOption(CMD_REMOVE, OPT_ROOT, false);
    addOption(CMD_REMOVE, OPT_CONFIG_FILE, false);

    addOption(CMD_INSTALL, OPT_DISABLE, false);
    addOption(CMD_INSTALL, OPT_UPDATE, false);
    addOption(CMD_INSTALL, OPT_FORCE, false);
    addOption(CMD_INSTALL, OPT_ROOT, false);
    addOption(CMD_INSTALL, OPT_NOLOGENTRY, false);
    addOption(CMD_INSTALL, OPT_CONFIG_FILE, false);

    addOption(CMD_CREATE, OPT_DRY, false);
    addOption(CMD_CREATE, OPT_NO_META, false);
    addOption(CMD_CREATE, OPT_ROOT, false);

    addOption(CMD_SYNC, OPT_CONFIG_FILE, false);

}
