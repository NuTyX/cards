/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkginfo.h"
#include "repo.h"
#include "cards_argument_parser.h"

/**
 * \brief Cards info class
 *
 * This class takes care of printing out information about installed packages, 
 * availables packages on remote server or available ports that can be compiled.
 *
 */

class cards_info: public pkginfo, public repo
{
	public:
		/**
		 * Default constructor
		 * 
		 * \param argParser CardsArgumentParser type
		 * \param configFileName Specify the configuration file name to use
		 */
		cards_info(const CardsArgumentParser& argParser,const std::string& configFileName);
			
	private:
		const CardsArgumentParser& m_argParser;
};

