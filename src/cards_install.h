/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkginst.h"
#include "cards_argument_parser.h"

class cards_install: public pkginst
{
	public:
		cards_install(const CardsArgumentParser& argParser,
			const char *configFileName);

		/* To install the dependencies of a package
		 * created with cards create command
		 */
		cards_install(const CardsArgumentParser& argParser,
			const char *configFileName,
			const std::vector<std::string>& listOfPackages);
			
	private:
		const CardsArgumentParser& m_argParser;
		void parseArguments();
		void getLocalePackagesList();
		std::string m_configFileName;
//		Config m_config;

};
