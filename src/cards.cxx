/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "cards_base.h"
#include "cards_info.h"
#include "install.h"
#include "cards_remove.h"
#include "upgrade.h"
#include "sync.h"
#include "create.h"
#include "depends.h"
#include "deptree.h"
#include "file_download.h"
#include "level.h"
#include "pkginfo.h"
#include "pkgrepo.h"
#include "pkgrm.h"

#include <cstdlib>
#include <memory>

using namespace std;

int main(int argc, char** argv)
{
    string command = basename(argv[1]);
    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    textdomain(GETTEXT_PACKAGE);
    string configFile = "/etc/cards.conf";
    try {
        CardsArgumentParser cardsArgPars;
        cardsArgPars.parse(argc, argv);
#ifdef DEBUG
        cout << cardsArgPars.getIdValue(cardsArgPars.command()) << endl;
        cout << cardsArgPars.getIdValue() << endl;
        cout << cardsArgPars.getCmdValue() << endl;
#endif
        if (cardsArgPars.isSet(CardsArgumentParser::OPT_CONFIG_FILE))
            configFile = cardsArgPars.getOptionValue(CardsArgumentParser::OPT_CONFIG_FILE);

        switch (cardsArgPars.getCmdValue()) {
        case ArgParser::CMD_HELP:
            cout << _("Usage: ") << BLUE << cardsArgPars.appName() << _(" command ") << NORMAL << "[options]" << endl;

            cout << _("Where possible") << BLUE << _(" command ") << NORMAL << _("is:") << endl;

            cout << GREEN << _("\nINFORMATION") << NORMAL << endl;
            cout << BLUE << "  help                       "
                 << NORMAL << _("show this help") << endl;
            cout << BLUE << "  list [-F][-b][-p]          "
                 << NORMAL << _("list of installed packages") << endl
                 << "                             "
                 << _("Available binaries in depot.") << endl
                 << "                             "
                 << _("Available ports.") << endl;

            cout << BLUE << _("  info") << NORMAL << " [-b][-p] <package>    "
                 << _("print info about install package") << endl
                 << "                             "
                 << _("Available binaries in depot.") << endl
                 << "                             "
                 << _("Available ports.") << endl;
            cout << BLUE << "  config" << NORMAL << "                     "
                 << _("show the configuration of your ")
                 << cardsArgPars.appName() << endl;
            cout << BLUE << "  level [-I]" << NORMAL << "                 "
                 << _("show all the ports founds. The list is showned by order of compilation")
                 << endl
                 << "                             "
                 << _("If -I it will ignore the WARNING about NOT FOUND <dependencies> from <port>")
                 << endl;
            cout << GREEN << _("\nPORTS SPECIFIC SCENARIO") << NORMAL << endl;
            cout << BLUE << "  depends" << NORMAL << " [-i] <port>        "
                 << _("show dependencies for the port in compilation order.")
                 << endl
                 << "                             "
                 << _("If -i it will shows the installed dependencies as well.")
                 << endl;
            cout << BLUE << "  deptree" << NORMAL << "   <port>           "
                 << _("show dependencies in a tree.") << endl;
            cout << BLUE << "  depcreate" << NORMAL << " <port>           "
                 << _("compile and install the port and its dependencies.")
                 << endl;
            cout << BLUE << "  create" << NORMAL << "    <port>           "
                 << _("install all the dependencies from binaries and then compile the port.")
                 << endl;

            cout << GREEN << _("\nDIFFERENCES / UPGRADE / CLEANUP") << NORMAL << endl;
            cout << BLUE << "  diff" << NORMAL << " [-p]                  "
                 << _("list outdated packages.") << endl
                 << "                             list outdated ports" << endl;
            cout << BLUE << "  upgrade" << NORMAL << "                    "
                 << _("upgrade outdated packages.") << endl;
            cout << BLUE << "  purge" << NORMAL << "                      "
                 << _("cleanup downloaded binaries in cache.") << endl;
            cout << GREEN << _("\nSEARCHING") << NORMAL << endl;
            cout << BLUE << "  search" << NORMAL << " <expr>              "
                 << _("show port names or description containing 'expr'")
                 << endl;
            cout << BLUE << "  query" << NORMAL << "  <file>              "
                 << _("list owner of file(s) matching the query.") << endl;
            cout << BLUE << "  files" << NORMAL << "  <package>           "
                 << _("list the file(s) owned by the <package>") << endl;
            cout << GREEN << _("\nSYNCHRONIZATION") << NORMAL << endl;
            cout << BLUE << "  sync" << NORMAL << "                       "
                 << _("synchronize the local and remote meta datas.") << endl;
            cout << GREEN << _("\nINSTALL, UPDATE and REMOVAL") << NORMAL << endl;
            cout << BLUE << "  install" << NORMAL << " [-u][-f] <package> "
                 << _("install the binary found on the mirror.") << endl
                 << "                             "
                 << _("If -u it will upgrade the installed package.") << endl
                 << "                             "
                 << _("If -f it will force the installation in case of files conflicts.") << endl;
            cout << BLUE << "  remove" << NORMAL << " [-a] <package>      "
                 << _("remove the installed package.") << endl
                 << "                             "
                 << _("If -a it will remove the sub-package as well.") << endl;
            cout << GREEN << _("\nBASE SYSTEM") << NORMAL << endl;
            cout << BLUE << "  base" << NORMAL << " -r                    "
                 << _("return to a base system.") << endl
                 << "                             "
                 << _("You need to have a valid 'base' directory") << endl
                 << "                             "
                 << _("configured in the /etc/cards.conf file otherwise the command will abort.") << endl;
            return EXIT_SUCCESS;

        case ArgParser::CMD_CONFIG: {
            cards::conf config(configFile);
            unsigned int index = 0;

            for (auto i : config.dirUrl()) {
                index++;
                cout << index << _(" Directory: ") << i.dir;
                if (i.url != "")
                    cout << _(" from ")
                         << i.url;
                cout << endl;
            }
            for (auto i : config.baseDir())
                cout << _("Base System list directory: ") << i << endl;
            cout << _("Binaries : ")
                 << config.arch() << endl;
            for (auto i : config.groups())
                cout << "Group    : " << i << endl;
            if (config.logdir() != "") {
                cout << _("log directory: ")
                     << config.logdir() << endl;
            }
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_BASE: {
            if ((!cardsArgPars.isSet(CardsArgumentParser::OPT_REMOVE)) && (!cardsArgPars.isSet(CardsArgumentParser::OPT_DRY))) {
                cardsArgPars.printHelp("base");
                return EXIT_SUCCESS;
            }
            unique_ptr<cards_base> i(new cards_base(cardsArgPars));
            i->run(argc, argv);
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_FILES: {
            unique_ptr<cards_info> i(new cards_info(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_SEARCH: {
            unique_ptr<cards_info> i(new cards_info(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_SYNC:
            if (getuid()) {
                string s = "";
                throw runtime_error(s + _(" only root can install / sync / purge / upgrade / remove packages"));
            }
            if (cardsArgPars.isSet(CardsArgumentParser::OPT_CONFIG_FILE)) {
				cards::sync sync(configFile);
				sync.run();
			} else {
				cards::sync sync;
				sync.run();
			}
            return EXIT_SUCCESS;

        case ArgParser::CMD_LIST: {
            unique_ptr<cards_info> i(new cards_info(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_QUERY: {
            unique_ptr<cards_info> i(new cards_info(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_INFO: {
            unique_ptr<cards_info> i(new cards_info(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_DIFF: {
            unique_ptr<cards_upgrade> i(new cards_upgrade(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_INSTALL: {
            unique_ptr<cards::install> i(new cards::install(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_REMOVE: {
            unique_ptr<cards_remove> i(new cards_remove("cards remove", cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_UPGRADE: {
            unique_ptr<cards_upgrade> i(new cards_upgrade(cardsArgPars, configFile.c_str()));
            if (cardsArgPars.isSet(CardsArgumentParser::OPT_DOWNLOAD_READY))
                return i->Isdownload();
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_PURGE: {
			cards::sync sync;
			sync.purge();
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_CREATE: {
            cards::create create(cardsArgPars);
            create.build(cardsArgPars.otherArguments()[0]);
            return EXIT_SUCCESS;
        }

        case ArgParser::CMD_LEVEL: {
            cards::level level;
            level.print();
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_DEPENDS: {
            cards::depends depends;
            depends.print(cardsArgPars.otherArguments()[0]);
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_DEPTREE: {
            cards::deptree tree;
            std::cout << "  dependencies ( '-->' = listed already)"
                      << std::endl;
            tree.print(cardsArgPars.otherArguments()[0], "", 0);
            return EXIT_SUCCESS;
        }

        default:
            cout << "not found" << endl;
            cout << cardsArgPars.getIdValue(cardsArgPars.command()) << endl;
        }
    } catch (runtime_error& e) {
        cerr << "cards " << VERSION << " " << command << ": " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
