/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "sync.h"
#include "pkgadd.h"
#include "pkgrepo.h"
#include "cards_argument_parser.h"

namespace cards {

class upgrade : public pkgadd {

	std::set<std::string>                     m_ListOfPackagesToDelete;
	std::set<std::pair<std::string, time_t>>  m_ListOfPackagesToUpdate;

	const CardsArgumentParser&                m_argParser;
	cards::sync                               m_sync;
	cards::pkgrepo                            m_pkgrepo;
	cards::conf                               m_config;

public:
	/**
	 *
	 *  Constructor
	 */
	upgrade(const CardsArgumentParser& argParser,
		const std::string& configFileName);

	void upgradePackages();
	/**
	 *
	 * Show the packages that should be update without upgrade them
	 */
	void dry();
	
	/**
	 *
	 *  Number of updates availables
	 */
	void size();
	
	/**
	 *
	 * Are they some updates availables ?
	 */
	void Isuptodate();
	int  Isdownload();
	void go();
	void summary();
};
}
