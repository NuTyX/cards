//
// cards_upgrade.h
// 
//  Copyright (c) 2015 - 2020 by NuTyX team (http://nutyx.org)
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

#ifndef CARDS_UPGRADE_H
#define CARDS_UPGRADE_H

#include "pkgsync.h"
#include "pkginst.h"
#include "cards_argument_parser.h"

class Cards_upgrade : public Pkginst {
private:
	std::set<std::string>                   m_ListOfPackagesToDelete;
	std::set<std::pair<std::string,time_t>> m_ListOfPackages;
	const CardsArgumentParser& m_argParser;
	Pkgsync m_pkgsync;

public:
	Cards_upgrade(const CardsArgumentParser& argParser,
		const char *configFileName);
	void upgradePackages();
	// show the packages that should be update without upgrade them
	void dry();
	
	// number of updates availables
	void size();
	
	// True if some updates availables
	void Isuptodate();
	void upgrade();
};
#endif /* CARDS_UPGRADE_H */
// vim:set ts=2 :
