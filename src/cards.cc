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
#include "cards_install.h"
#include "config_parser.h"
#include "pkginfo.h"

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

		return cards_install(argv[2]);

	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LEVEL) {

		return cards_level();
	
	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPINST) {


	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPENDS) {

		return cards_depends(argv[2]);

	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPTREE) {

		return cards_deptree(argv[2]);
	
  } else if (cardsArgPars.command() == CardsArgumentParser::CMD_LISTINST) {

		Pkginfo * packagesInfo =  new Pkginfo;
		itemList * commandList = initItemList();
		addItemToItemList(commandList,"pkginfo");
		addItemToItemList(commandList, "-i");
		packagesInfo->run(commandList->count,commandList->items);
		cout << endl << packagesInfo->numberOfPackages() << " packages installed" << endl;
		freeItemList(commandList);
		delete packagesInfo;
	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_ISINST) {
		Pkginfo * packagesInfo =  new Pkginfo;
		if ( packagesInfo->isInstalled(argv[2]) )	{
			cout << argv[2] << " is installed" << endl;
		} else {
			 cout << argv[2] << " is not installed" << endl;
		}
		delete packagesInfo;
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
		<< "  isinst\n"
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
