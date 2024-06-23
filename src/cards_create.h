/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkginst.h"
#include "cards_argument_parser.h"

class cards_create : public pkginst
{
	public:
		cards_create(const CardsArgumentParser& argParser,
			const char *configFileName,
			const std::vector<std::string>& listOfPackages);
		cards_create(const CardsArgumentParser& argParser,
			const char *configFileName,
			const std::string& packageName);
		void parseArguments();
			
	private:
		void createBinaries(const char *configFileName,
			const std::string& packageName);
		const CardsArgumentParser& m_argParser;
};
