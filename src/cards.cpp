// cards.cpp
//
//  Copyright (c) 2014-2015 by NuTyX team (http://nutyx.org)
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

#include <iostream>
#include <cstdlib>
#include "file_download.h"
#include "cards_base.h"
#include "cards_sync.h"
#include "cards_depends.h"
#include "cards_install.h"
#include "cards_remove.h"
#include "cards_create.h"
#include "cards_info.h"
#include "cards_upgrade.h"

#include "pkgrepo.h"
#include "pkginfo.h"
#include "pkgrm.h"

using namespace std;


int main(int argc, char** argv)
{
	string command = basename(argv[1]);
	string configFile = "/etc/cards.conf";
	try {
		CardsArgumentParser cardsArgPars;
		cardsArgPars.parse(argc, argv);

		if (cardsArgPars.isSet(CardsArgumentParser::OPT_CONFIG_FILE))
			configFile=cardsArgPars.getOptionValue(CardsArgumentParser::OPT_CONFIG_FILE);

		if (cardsArgPars.command() == CardsArgumentParser::CMD_HELP) {
			cout << "Usage: " << BLUE << cardsArgPars.appName()  << " command " << NORMAL << "[options]"<< endl;

			cout << "where possible " << BLUE << "command" << NORMAL<< " are:" << endl;

			cout << GREEN << "\nINFORMATION" << NORMAL << endl;
			cout << BLUE << "  help" << NORMAL << "                       show this help" << endl;
			cout << BLUE << "  list" << NORMAL << " [-b][-p]              show a list of installed packages" << endl
				<< "                             available binaries" << endl
				<< "                             available ports" << endl;

			cout << BLUE << "  info" << NORMAL << " [-b][-p] <package>    print info about install package" << endl
				<< "                             available binaries" << endl
				<< "                             available ports" << endl;
			cout << BLUE << "  config" << NORMAL << "                     show the configuration of your " <<  cardsArgPars.appName() << endl;
			cout << BLUE << "  level [-I]" << NORMAL << "                 show all the ports founds. The list is showned by order \
of compilation" << endl
				<< "                             If -I it will ignore the WARNING about NOT FOUND <dependencies> from <port>" << endl;
			cout << GREEN << "\nPORTS SPECIFIC SCENARIO" << NORMAL << endl;
			cout << BLUE << "  depends" << NORMAL << " [-i] <port>        show dependencies for the port in compilation order." << endl
				<< "                             If -i it will shows the installed dependencies as well." << endl;
			cout << BLUE << "  deptree" << NORMAL << "   <port>           show dependencies in a tree" << endl;
			cout << BLUE << "  depcreate" << NORMAL << " <port>           compile and install the port and its dependencies." << endl;
			cout << BLUE << "  create" << NORMAL << "    <port>           install all the dependencies from binaries and then compile the port." << endl;

			cout << GREEN << "\nDIFFERENCES / CHECK FOR UPDATES / CLEANUP" << NORMAL << endl;
			cout << BLUE << "  diff" << NORMAL << " [-p]                  list outdated packages" << endl
				<< "                             list outdated ports" << endl;
			cout << BLUE << "  purge" << NORMAL << "                      cleanup downloaded binaries in cache." << endl;
			cout << GREEN << "\nSEARCHING" << NORMAL << endl;
			cout << BLUE << "  search" << NORMAL << " <expr>              show port names or description containing 'expr'" << endl;
			cout << BLUE << "  query" << NORMAL << "  <file>              list owner of file(s) matching the query" << endl;
			cout << BLUE << "  files" << NORMAL << "  <package>           list the file(s) owned by the <package>" << endl;
			cout << GREEN << "\nSYNCHRONISATION" << NORMAL << endl;
			cout << BLUE << "  sync" << NORMAL << "                       synchronize the local and remote meta datas" << endl;
			cout << GREEN << "\nINSTALL, UPDATE and REMOVAL" << NORMAL << endl;
			cout << BLUE << "  install" << NORMAL << " [-u][-f] <package> install the binary found on the mirror." << endl
				<< "                             If -u it will upgrade the installed package" << endl
				<< "                             If -f it will force the installation in case of files conflicts." << endl;
			cout << BLUE << "  remove" << NORMAL << " [-a] <package>      remove the installed package." << endl
				<< "                             If -a it will remove the sub-package as well." << endl;
			cout << GREEN << "\nBASE SYSTEM" << NORMAL << endl;
			cout << BLUE << "  base" << NORMAL << " -r                    return to a base system." << endl
				<< "                             You need to have a valid 'base' directory" << endl
				<< "                             define in /etc/cards.conf otherwise it will fails." << endl;
			return EXIT_SUCCESS;
		}
		if (cardsArgPars.command() == CardsArgumentParser::CMD_CONFIG) {
			Config config;
			Pkgrepo::parseConfig(configFile, config);
			unsigned int index = 0;
			string prtDir, url ;
			for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
				index++;
				DirUrl DU = *i ;
				prtDir = DU.Dir;
				url = DU.Url;
				cout << index << " Directory: " << prtDir ;
				if ( url != "" ) {
					cout << " from " << url  << endl;
				} else {
					cout << endl;
				}
			}
			for ( auto i : config.baseDir ) cout << "Base System list directory: " << i << endl;
			cout <<   "Binaries : " << config.arch << endl;
			for ( auto i : config.locale ) cout << "Locale   : " << i << endl;
			if ( config.logdir != "") {
				cout << "log directory: " << config.logdir << endl;
			}
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPCREATE) {
			// get the list of the dependencies
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			vector<string> listOfPackages = CD.getNeededDependencies();
			if ( listOfPackages.size() == 0) {
				cout << "The package " << cardsArgPars.otherArguments()[0] << " is already installed" << endl;
				return EXIT_SUCCESS;
			}
			// create (compile and install) the List of deps (including the final package)
			Cards_create CC(cardsArgPars,configFile,listOfPackages);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_CREATE) {
			if ( ( ! cardsArgPars.isSet(CardsArgumentParser::OPT_REMOVE)) &&
			( ! cardsArgPars.isSet(CardsArgumentParser::OPT_DRY)) ) {
				cardsArgPars.printHelp("create");
				return EXIT_SUCCESS;
			}
			if  ( ! cardsArgPars.isSet(CardsArgumentParser::OPT_DRY)) {
				// go back to a base system
				CardsBase CB(cardsArgPars);
				CB.run(argc, argv);
			}
			// get the list of the dependencies"
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			vector<string> listOfDeps = CD.getDependencies();
			Cards_install CI(cardsArgPars,configFile,cardsArgPars.otherArguments()[0]);
			if (listOfDeps.size()>1)
				Cards_install CI(cardsArgPars,configFile,listOfDeps);
			// compilation of the final port"
			Cards_create CC( cardsArgPars,configFile,cardsArgPars.otherArguments()[0]);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_BASE) {
			if ( ( ! cardsArgPars.isSet(CardsArgumentParser::OPT_REMOVE)) &&
			( ! cardsArgPars.isSet(CardsArgumentParser::OPT_DRY)) ) {
				cardsArgPars.printHelp("base");
				return EXIT_SUCCESS;
			}
			CardsBase CB(cardsArgPars);
			CB.run(argc, argv);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_SYNC) {
			CardsSync CS(cardsArgPars);
			CS.run();
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_PURGE) {
			CardsSync CS(cardsArgPars);
			CS.purge();
			return EXIT_SUCCESS;
		} else if ( (cardsArgPars.command() == CardsArgumentParser::CMD_UPGRADE) ||
					(cardsArgPars.command() == CardsArgumentParser::CMD_DIFF) ) {
			Cards_upgrade CU(cardsArgPars,configFile);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_INSTALL) {
				Cards_install CI(cardsArgPars,configFile);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_REMOVE) {
			Cards_remove CR(cardsArgPars);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LEVEL) {
			CardsDepends CD(cardsArgPars);
			CD.showLevel();
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPENDS) {
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			CD.showDependencies();
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPTREE) {
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			return CD.deptree();
		} else if ( (cardsArgPars.command() == CardsArgumentParser::CMD_INFO) ||
				(cardsArgPars.command() == CardsArgumentParser::CMD_LIST)   ||
				(cardsArgPars.command() == CardsArgumentParser::CMD_SEARCH) ||
				(cardsArgPars.command() == CardsArgumentParser::CMD_FILES)  ||
				(cardsArgPars.command() == CardsArgumentParser::CMD_QUERY)) {
			Cards_info CI(cardsArgPars,configFile);
			return EXIT_SUCCESS;
		}
	} catch (runtime_error& e) {
			cerr << "cards " << VERSION << " "<< command << ": " << e.what() << endl;
			return EXIT_FAILURE;
	}
}
// vim:set ts=2 :
