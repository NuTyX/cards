// cards_upgrade.h
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
#ifndef CARDSUPGRADE_H
#define CARDSUPGRADE_H
#include "pkginst.h"
#include "cards_argument_parser.h"

class CardsUpgrade : public Pkginst {
private:
	set<pair<string,time_t>> m_ListOfPackages;
	const CardsArgumentParser& m_argParser;
public:
	CardsUpgrade(const CardsArgumentParser& argParser);
	void upgradePackages();
	void size(); // number of updates availables
	void Isuptodate(); // True if some updates availables
	void run(int argc, char** argv);
	void printHelp() const;
};
#endif
// vim:set ts=2 :
