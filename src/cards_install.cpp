/*
 * cards_install.cpp
 * 
 * Copyright 2015 - 2016 Thierry Nuttens <tnut@nutyx.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include "cards_install.h"


Cards_install::Cards_install(const CardsArgumentParser& argParser,const std::string& configFileName)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";

	if  ( ! m_argParser.otherArguments().empty() ) {
		if (getuid()) {
			m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
			treatErrors("");
		}

		buildDatabaseWithNameVersion();

		for( auto i : m_argParser.otherArguments() ) {
			m_packageName = i;
			generateDependencies();
		}

		for ( auto i : m_dependenciesList ) {
			m_packageArchiveName = getPackageFileName(i);
			run();
		}
	}	
}
Cards_install::Cards_install(const CardsArgumentParser& argParser, const std::string& configFileName, const std::vector<string>& listOfPackages)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
}
Cards_install::Cards_install(const CardsArgumentParser& argParser, const std::string& configFileName, const std::string& packageName)
	: Pkginst("cards install",configFileName),m_argParser(argParser)
{
}

