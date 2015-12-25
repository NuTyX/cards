// cards_upgrade.cc
//
//  Copyright (c) 2015 by NuTyX team (http://nutyx.org)
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
CardsUpgrade::CardsUpgrade(const CardsArgumentParser& argParser)
	: Pkginst("cards upgrade","/etc/cards.conf",true), m_argParser(argParser)
{
	parsePkgRepoCollectionFile();
	for (auto i : m_listOfInstPackages) {
		if (!checkBinaryExist(i.first)) {
			cout << i.first << " not exist" << endl;
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
	if ( m_argParser.isSet(CardsArgumentParser::OPT_CHECK))
		Isuptodate();
	if ( m_argParser.isSet(CardsArgumentParser::OPT_SIZE))
		size();
	if ( (! m_argParser.isSet(CardsArgumentParser::OPT_SIZE)) &&
		(! m_argParser.isSet(CardsArgumentParser::OPT_CHECK)) )
		run(0,NULL);
}
void CardsUpgrade::size()
{
	cout << m_ListOfPackages.size() << endl;
}
void CardsUpgrade::Isuptodate()
{
	if ( m_ListOfPackages.size() == 0)
		cout << "no" << endl;
	else
		cout << "yes" << endl;
}
void CardsUpgrade::run(int argc, char** argv)
{
	for (auto i : m_ListOfPackages) {
#ifndef NDEBUG
		cerr << i.first << " " << i.second << endl;
#endif
		generateDependencies(i);
	}
	if (!m_dependenciesList.empty())
		addPackagesList(m_argParser.isSet(CardsArgumentParser::OPT_FORCE)  );
}
void CardsUpgrade::printHelp() const
{
}
// vim:set ts=2 :
