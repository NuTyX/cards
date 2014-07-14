// cards.cc
//
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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
#include "cards_info.h"

#include "config_parser.h"

#include "pkginfo.h"

using namespace std;


int main(int argc, char** argv)
{
	string command = basename(argv[1]);
	try {
		CardsArgumentParser cardsArgPars;
		cardsArgPars.parse(argc, argv);
		if (cardsArgPars.command() == CardsArgumentParser::CMD_CONFIG) {
			Config config;
			ConfigParser::parseConfig("/etc/cards.conf", config);
			unsigned int index = 0;
			string prtDir, url ;
			for (vector<string>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
				index++;
				string val = *i ;
				string::size_type pos = val.find('|');
				if (pos != string::npos) {
      		prtDir = stripWhiteSpace(val.substr(0,pos));
      		url = stripWhiteSpace(val.substr(pos+1));
				} else {
					prtDir = val;
					url = "" ;
				}
				cout << index << " Directory: " << prtDir ;
				if ( url != "" ) {
					cout << " from " << url  << endl;
				} else {
					cout << endl;
				}
			}
			for (vector<string>::iterator i = config.baseDir.begin();i != config.baseDir.end();++i) {
				cout << "Base System list directory: " << *i << endl;
			}
			cout <<   "Binaries : " << config.arch << endl;
			for (vector<string>::iterator i = config.locale.begin();i != config.locale.end();++i) {
				cout << "Locale   : " << *i << endl;
			}
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_BASE) {
			CardsBase CB(cardsArgPars);
			CB.run(argc, argv);
			return EXIT_SUCCESS;
		} else	if (cardsArgPars.command() == CardsArgumentParser::CMD_SYNC) {
			CardsSync CS(cardsArgPars);
			CS.run();
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DIFF) {
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_INSTALL) {
			CardsInstall CI(cardsArgPars);
			CI.run(argc, argv);
			if (cardsArgPars.isSet(CardsArgumentParser::OPT_INSTALL_DRY)) {
				CI.printDependenciesList();
			} else {
				CI.install();
			}
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_REMOVE) {
			CardsRemove CR(cardsArgPars);
			CR.run(argc, argv);
			return EXIT_SUCCESS;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LEVEL) {
			CardsDepends CD(cardsArgPars);
			return CD.level();
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPENDS) {
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			return CD.depends();
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPTREE) {
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			return CD.deptree();
  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LIST) {
			CardsInfo CList(cardsArgPars);
			if (cardsArgPars.isSet(CardsArgumentParser::OPT_LIST_INSTALL)) {
				CList.listInstalled();
			} else if (cardsArgPars.isSet(CardsArgumentParser::OPT_LIST_BINARIES)) {
				CList.listBinaries();
			} else if (cardsArgPars.isSet(CardsArgumentParser::OPT_LIST_PORTS)) {
				CList.listPorts();
			}
			return EXIT_SUCCESS;
  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_INFO) {
			CardsInfo CInfo(cardsArgPars);
			if (cardsArgPars.isSet(CardsArgumentParser::OPT_INFO_INSTALL)) {
				CInfo.infoInstall();
			} else if (cardsArgPars.isSet(CardsArgumentParser::OPT_INFO_BINARY)) {
				CInfo.infoBinary();
			} else if (cardsArgPars.isSet(CardsArgumentParser::OPT_INFO_PORT)) {
				CInfo.infoPort();
			}
			return EXIT_SUCCESS;

  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_SEARCH) {
			return EXIT_SUCCESS;
		} else {
		cerr << "Supported commands so far:\n"
		<< "  config\n"
		<< "  base\n"
		<< "  sync\n"
		<< "  depends\n"
		<< "  install\n"
		<< "  remove\n"
		<< "  info\n"
		<< "  list\n"
		<< "  diff\n"
		<< "  search\n"
			<< "\n"
			<< " use cards <command> -h " << endl;
		exit(-1);
		}
		return EXIT_SUCCESS;
	} catch (runtime_error& e) {
		
		cerr << "cards " << VERSION << " "<< command << ": " << e.what() << endl;
		return -1;
	}
}
// vim:set ts=2 :
