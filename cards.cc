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
#include "cards_sync.h"
#include "cards_depends.h"
#include "config_parser.h"

#define PKG_DB_DIR       "/var/lib/pkg/DB/"

using namespace std;


int main(int argc, char** argv)
{

	CardsArgumentParser cardsArgPars;
	cardsArgPars.parse(argc, argv);


	if (cardsArgPars.command() == CardsArgumentParser::CMD_SYNC) {
		CardsSync CS(cardsArgPars);
		return CS.exec();
  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_DIFF) {


	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_INSTALL) {

	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LEVEL) {

		return cards_level();
	
	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPINST) {

		return cards_depinst(argv[2]);

	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPENDS) {

		return cards_depends(argv[2]);

	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPTREE) {

		return cards_deptree(argv[2]);
	
  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_LISTINST) {
		set<string> installedPackagesList;
		if ( findFile(installedPackagesList, PKG_DB_DIR) != 0 ) {
			cerr << " Cannot read "  << PKG_DB_DIR
				<< endl;
			return -1;
		}
		if (installedPackagesList.size() > 0) {
			for (set<string>::const_iterator packageName = installedPackagesList.begin();
			packageName != installedPackagesList.end();
			packageName++ ) {
				unsigned int pos = packageName -> find("_");
				if ( pos != std::string::npos) {
					string name = packageName -> substr(0,pos); 
					string version = packageName -> substr(pos+1);
					cout << name + " " + version << endl;
				}
			}
			cout << endl << installedPackagesList.size() << " packages installed" << endl;
		} else {
			cout << endl <<  "No packages found" << endl;
		}
  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_LIST) {
		Config config;
		ConfigParser::parseConfig("/etc/cards.conf", config);
		unsigned int  numberOfPorts = 0;
		set<string> localPackagesList;
		for (unsigned int indCat = 0; indCat < config.prtDir.size();++indCat) {
			if ( findFile(localPackagesList, config.prtDir[indCat]) != 0 ) {
				cerr << " Cannot read "  << config.prtDir[indCat]
					<< endl;
			}
		}
		if (localPackagesList.size() == 0 ) {
			cout << "You need to cards sync first" << endl;
			return 0;
		} else {
			for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); li++) {
				cout << *li << endl;
			}
			numberOfPorts = numberOfPorts + localPackagesList.size();
		}
		cout << endl << "Number of availables ports : " << numberOfPorts << endl << endl;

  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_INFO) {

  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_SEARCH) {

  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_DSEARCH) {

  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_FSEARCH) {

	} else {
		cerr << "Supported commands so far:\n"
		<< "  sync\n"
		<< "  install\n"
		<< "  depinst\n"
		<< "  info\n"
		<< "  list\n"
		<< "  listinst\n"
		<< "  search\n"
		<< "  dsearch\n"
		<< "  fsearch\n"
			<< "\n"
			<< " use cards <command> -h " << endl;
		exit(-1);
	}
	return 0;
}

// vim:set ts=2 :
