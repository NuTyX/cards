/*
 * cards_info.cpp
 * 
 * Copyright 2015-2016 Thierry Nuttens <tnut@nutyx.org>
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

#include "cards_info.h"

Cards_info::Cards_info(const CardsArgumentParser& argParser, const std::string& configFileName)
	: Pkginfo(),Pkgrepo(configFileName), m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root=m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT);

	if (m_root.empty())
		m_root="/";
	else
		m_root=m_root+"/";
	if ((m_argParser.command() == CardsArgumentParser::CMD_INFO) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			getBinaryPackageInfo(m_argParser.otherArguments()[0]);
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_PORTS)) {
			getPortInfo(m_argParser.otherArguments()[0]);
		} else {
			m_details_mode=1;
			m_arg=m_argParser.otherArguments()[0];
			run();
		}
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_LIST) ) {
		if (m_argParser.isSet(CardsArgumentParser::OPT_BINARIES)) {
			getBinaryPackageList();
		} else if (m_argParser.isSet(CardsArgumentParser::OPT_PORTS)) {
			getPortsList();
		} else {
			m_installed_mode=1;
			run();
		}
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_QUERY) ) {
		m_owner_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_FILES) ) {
		m_list_mode=1;
		m_arg=m_argParser.otherArguments()[0];
		run();		
	}
	if ((m_argParser.command() == CardsArgumentParser::CMD_SEARCH) ) {
		search(m_argParser.otherArguments()[0]);
	}
}
// vim:set ts=2 :
