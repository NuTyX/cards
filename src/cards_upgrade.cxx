// cards_upgrade.cxx
//
//  Copyright (c) 2015-2017 by NuTyX team (http://nutyx.org)
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

#include "cards_upgrade.h"
Cards_upgrade::Cards_upgrade(const CardsArgumentParser& argParser,
	const char *configFileName)
	: Pkginst("cards upgrade",configFileName), m_argParser(argParser)
{
	using namespace std;
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

	parsePkgRepoCollectionFile();
	buildSimpleDatabase();
	for (auto i : m_listOfInstPackages) {
		if (!checkBinaryExist(i.first)) {
			cerr << i.first << " not exist" << endl;
			continue;
		}
		pair<string,time_t> packageNameBuildDate;
		packageNameBuildDate.first = i.first ;
		packageNameBuildDate.second = getBinaryBuildTime(i.first);
		if ( checkPackageNameBuildDateSame(packageNameBuildDate)) {
			continue;
		}
		m_ListOfPackages.insert(packageNameBuildDate);
	}
	if ( m_argParser.command() == CardsArgumentParser::CMD_UPGRADE) {
		if ( m_argParser.isSet(CardsArgumentParser::OPT_CHECK))
			Isuptodate();
		if ( m_argParser.isSet(CardsArgumentParser::OPT_SIZE))
			size();
		if ( (! m_argParser.isSet(CardsArgumentParser::OPT_SIZE)) &&
			(! m_argParser.isSet(CardsArgumentParser::OPT_CHECK)) )
			upgrade();
	}
	if ( m_argParser.command() == CardsArgumentParser::CMD_DIFF) {
		dry();
	}
}
void Cards_upgrade::size()
{
	std::cout << m_ListOfPackages.size() << std::endl;
}
void Cards_upgrade::Isuptodate()
{
	if ( m_ListOfPackages.size() == 0)
		std::cout << "no" << std::endl;
	else
		std::cout << "yes" << std::endl;
}
void Cards_upgrade::dry()
{
	for (auto i : m_ListOfPackages ) std::cout << i.first  << std::endl;
}
void Cards_upgrade::upgrade()
{
	for (auto i : m_ListOfPackages) generateDependencies(i);

	if (m_argParser.isSet(CardsArgumentParser::OPT_DRY))
		dry();
	else {
		for (auto i : m_dependenciesList) {
			m_packageArchiveName=getPackageFileName(i);
			if (checkPackageNameExist(i)) {
				m_upgrade=1;
			} else {
				m_upgrade=0;
			}
		run();
		}
	}
}
// vim:set ts=2 :
