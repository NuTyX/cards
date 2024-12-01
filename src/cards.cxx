/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "create.h"
#include "dependent.h"
#include "depends.h"
#include "deptree.h"
#include "dwl.h"
#include "info.h"
#include "install.h"
#include "level.h"
#include "pkginfo.h"
#include "pkgrepo.h"
#include "pkgrm.h"
#include "remove.h"
#include "repo.h"
#include "sync.h"
#include "upgrade.h"

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
            cout << BLUE << "  list" << NORMAL << " [-b][-S]              "
                 << NORMAL << _("list of installed packages") << endl
                 << "                             "
                 << _("Available binaries in depot.") << endl
                 << "                             "
                 << _("Available sets of packages.") << endl;
            cout << BLUE << "  info" << NORMAL << " [-b][-S] <package>    "
                 << _("print info about install package") << endl
                 << "                             "
                 << _("Available binaries in depot.") << endl
                 << "                             "
                 << _("Available sets of packages.") << endl;
            cout << BLUE << "  config" << NORMAL << "                     "
                 << _("show the configuration of your ")
                 << cardsArgPars.appName() << endl;
            cout << BLUE << "  level" << NORMAL << "                      "
                 << _("show all the ports founds. The list is showned by order of compilation")
                 << endl;
            cout << GREEN << _("\nBUILDING PACKAGES SPECIFIC SCENARIO") << NORMAL << endl;
            cout << BLUE << "  depends" << NORMAL << "   <port>           "
                 << _("show dependencies for the port in compilation order.")
                 << endl;
            cout << BLUE << "  dependent" << NORMAL << " <port>           "
                 << _("show reverse dependencies list that needs the port.")
                 << endl;
            cout << BLUE << "  deptree" << NORMAL << "   <port>           "
                 << _("show dependencies in a tree.") << endl;
            cout << BLUE << "  keys" << NORMAL << "                       "
                 << _("generates the keys pair for the signature of the packages.")
                 << endl;
            cout << BLUE << "  create" << NORMAL << "    <port>           "
                 << _("install all the dependencies from binaries and then compile the port.")
                 << endl;
            cout << BLUE << "  repo" << NORMAL <<   "      [collection]     "
                 << _("check or signing a collection of packages depending if a private key is present.")
                 << endl;
            cout << GREEN << _("\nDIFFERENCES / UPGRADE / CLEANUP") << NORMAL << endl;
            cout << BLUE << "  diff" << NORMAL << "                       "
                 << _("list outdated packages.") << endl;
            cout << BLUE << "  upgrade" << NORMAL << "                    "
                 << _("upgrade outdated packages.") << endl;
            cout << BLUE << "  purge" << NORMAL << "                      "
                 << _("cleanup downloaded binaries in cache.") << endl;
            cout << GREEN << _("\nSEARCHING") << NORMAL << endl;
            cout << BLUE << "  search" << NORMAL << " [-n] <expr>         "
                 << _("show port names or description containing 'expr'")
                 << endl
                 << "                             "
                 << _("If -n it will search in the name of the available packages only.")
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
            cout << BLUE << "  remove" << NORMAL << "      <package>      "
                 << _("remove the installed package.") << endl;
            return EXIT_SUCCESS;

        case ArgParser::CMD_CONFIG: {
            cards::conf config(configFile);
            unsigned int index = 0;

            for (auto i : config.dirUrl()) {
                index++;
                cout << index
                     << _(" Directory: ")
                     << i.depot
                     << "/"
                     << i.collection;
                if (i.url != "")
                    cout << _(" from ")
                         << i.url
                         << "/"
                         << config.arch()
                         << "/"
                         << config.version()
                         << "/"
                         << i.collection;
                cout << endl;
            }
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

        case ArgParser::CMD_FILES: {
            cards::info info(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_SEARCH: {
            cards::info info(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
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
        case ArgParser::CMD_KEYS: {
            if (!getuid()) {
                string s = "";
                throw runtime_error(s + _(" this command should not be used under root."));
            }
            cards::pkgrepo pkgrepo(configFile.c_str());
            pkgrepo.generateKeys();
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_REPO: {
            if (!getuid()) {
                string s = "";
                throw runtime_error(s + _(" this command should not be used under root."));
            }
            cards::repo repo(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_LIST: {
            cards::info info(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_QUERY: {
            cards::info info(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_INFO: {
            cards::info info(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_DIFF: {
            cards::upgrade upgrade(cardsArgPars, configFile.c_str());
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_INSTALL: {
            unique_ptr<cards::install> i(new cards::install(cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_REMOVE: {
            unique_ptr<cards::remove> i(new cards::remove("cards remove", cardsArgPars, configFile.c_str()));
        }
            return EXIT_SUCCESS;

        case ArgParser::CMD_UPGRADE: {
            cards::upgrade upgrade(cardsArgPars, configFile.c_str());
            if (cardsArgPars.isSet(CardsArgumentParser::OPT_DOWNLOAD_READY))
                return upgrade.Isdownload();

            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_PURGE: {
            cards::sync sync;
            sync.purge();
            return EXIT_SUCCESS;
        }
        case ArgParser::CMD_CREATE: {
            cards::create create(cardsArgPars);
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
        case ArgParser::CMD_DEPENDENT: {
            cards::dependent dependent;
            dependent.print(cardsArgPars.otherArguments()[0]);
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
        cerr << "cards " << PACKAGE_VERSION << " " << command << ": " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
