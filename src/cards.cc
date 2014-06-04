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
			cout << "Not yet implemented, be patient" << endl;
			return 0;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_INSTALL) {
			cout << "Sorry, this function has to be completly review" << endl;
			return 0;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LEVEL) {
			CardsDepends CD(cardsArgPars);
			return CD.level();
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPINST) {
			cout << "Not yet implemented, be patient" << endl;
			return 0;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPENDS) {
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			return CD.depends();
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DEPTREE) {
			CardsDepends CD(cardsArgPars,const_cast<char*>(cardsArgPars.otherArguments()[0].c_str()));
			return CD.deptree();
  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LISTINST) {
			Pkginfo * packagesInfo =  new Pkginfo;
			itemList * commandList = initItemList();
			addItemToItemList(commandList,"pkginfo");
			addItemToItemList(commandList, "-i");
			packagesInfo->run(commandList->count,commandList->items);
			cout << endl << packagesInfo->getNumberOfPackages() << " packages installed" << endl;
			freeItemList(commandList);
			delete packagesInfo;
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_ISINST) {
			Pkginfo * packagesInfo =  new Pkginfo;
			packagesInfo->getNumberOfPackages();
			if ( packagesInfo->isInstalled(argv[2]) )	{
				cout << argv[2] << " is installed" << endl;
				delete packagesInfo;
				return 1;
			} else {
				cout << argv[2] << " is not installed" << endl;
				delete packagesInfo;
				return 0;
			}
		} else if (cardsArgPars.command() == CardsArgumentParser::CMD_LIST) {
			Config config;
			ConfigParser::parseConfig("/etc/cards.conf", config);
			unsigned int  numberOfPorts = 0;
			set<string> localPackagesList;
			for (vector<string>::iterator i = config.dirUrl.begin();i != config.dirUrl.end();++i) {
				string val = *i ;
				string prtDir;
				string::size_type pos = val.find('|');
				if (pos != string::npos) {
					prtDir = stripWhiteSpace(val.substr(0,pos));
				} else {
					prtDir = val;
				}
				if ( findFile(localPackagesList, prtDir) != 0 ) {
					cerr << " Cannot read "  << prtDir << endl;
				}
			}
			if (localPackagesList.size() == 0 ) {
			cout << "You need to cards sync first" << endl;
			return 0;
			} else {
				string name, version, val;
				for (set<string>::const_iterator li = localPackagesList.begin(); li != localPackagesList.end(); li++) {
					val = *li;
					string::size_type pos = val.find('@');
					if (pos != string::npos) {
						name = val.substr(0,pos);
						version = val.substr(pos+1);
					cout << name + " " + version << endl;
					}
				}
				numberOfPorts = numberOfPorts + localPackagesList.size();
			}
			cout << endl << "Number of availables ports : " << numberOfPorts << endl << endl;

  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_INFO) {
			cout << "Not yet implemented, be patient" << endl;
			return 0;

  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_SEARCH) {
			cout << "Not yet implemented, be patient" << endl;
			return 0;

  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_DSEARCH) {
			cout << "Not yet implemented, be patient" << endl;
			return 0;

  	} else if (cardsArgPars.command() == CardsArgumentParser::CMD_FSEARCH) {
			cout << "Not yet implemented, be patient" << endl;
			return 0;
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
	} catch (runtime_error& e) {
		
		cerr << "cards " << VERSION << " "<< command << ": " << e.what() << endl;
		return -1;
	}

}

// vim:set ts=2 :
