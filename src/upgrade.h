/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "sync.h"
#include "pkgadd.h"
#include "pkgrepo.h"
#include "cards_argument_parser.h"

namespace cards {
	struct m_package {
		std::string description;
		std::string collection;
		time_t installed_build;
		time_t available_build;
		std::string installed_version;
		std::string available_version;
		unsigned int installed_space;
		unsigned int available_space;
		UpgradeEnum status;

	};

class upgrade : public pkgadd {

	std::set<std::string>                    m_ListOfPackagesToDelete;
	std::set<std::pair<std::string, time_t>> m_ListOfPackagesToUpdate;
	std::map<std::string, m_package>         m_ListOfPackagesToDeal;
	const CardsArgumentParser&               m_argParser;
	cards::sync                              m_sync;
	cards::pkgrepo                           m_pkgrepo;
	cards::conf                              m_config;

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
	void show();
	
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
