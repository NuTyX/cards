/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgsync.h"
#include "pkginst.h"
#include "cards_argument_parser.h"

class cards_upgrade : public pkginst {
private:
	std::set<std::string>                   m_ListOfPackagesToDelete;
	std::set<std::pair<std::string,time_t>> m_ListOfPackages;
	const CardsArgumentParser&              m_argParser;
	pkgsync m_pkgSync;

public:
	cards_upgrade(const CardsArgumentParser& argParser,
		const char *configFileName);
	void upgradePackages();
	// show the packages that should be update without upgrade them
	void dry();
	
	// number of updates availables
	void size();
	
	// True if some updates availables
	void Isuptodate();
	int  Isdownload();
	void upgrade();
	void summary();
};
