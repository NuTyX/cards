/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "sync.h"
#include "pkgadd.h"
#include "pkgrepo.h"
#include "diff.h"
#include "cards_argument_parser.h"

namespace cards {

class upgrade : public pkgadd {

	std::set<std::string>                    m_ListOfPackagesToDelete;
	std::set<std::pair<std::string, time_t>> m_ListOfPackagesToUpdate;
	const CardsArgumentParser&               m_argParser;
	cards::sync                              m_sync;
	cards::pkgrepo                           m_pkgrepo;
	cards::conf                              m_config;
	cards::diff                              m_diff;

public:
	/**
	 *
	 *  Constructor
	 */
	upgrade(const CardsArgumentParser& argParser,
		const std::string& configFileName);

	void upgradePackages();

	int  Isdownload();
};
}
