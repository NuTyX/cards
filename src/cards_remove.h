/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgrm.h"
#include "cards_argument_parser.h"
#include "pkgrepo.h"

/**
 * \brief Cards remove class
 *
 * This class takes care of removing installed packages, installed collection
 * or installed family of packages (base attribute).
 *
 */


class cards_remove: public pkgrm
{
	public:
		/**
		 * Default constructor
		 * 
		 * \param commandName Specify the used command name
		 * \param argParser CardsArgumentParser type
		 * \param configFileName Specify the configuration file name to use
		 */
		cards_remove(const std::string& commandName,
		const CardsArgumentParser& argParser,
		const char *configFileName);

	private:
		const CardsArgumentParser& m_argParser;
};
