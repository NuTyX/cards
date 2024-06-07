//
// cards_sync.cxx
//
//  Copyright (c) 2002 - 2005 Johannes Winkelmann jw at tks6 dot net 
//  Copyright (c) 2014 - 2020 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

#include "cards_sync.h"

cards_sync::cards_sync (const CardsArgumentParser& argParser)
	: m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		setRootPath(m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT)+"/");
}
cards_sync::cards_sync ( const CardsArgumentParser& argParser,
		const std::string& url, const std::string& baseDirectory,
		const std::string& repoFile)
		: m_argParser(argParser),
		pkgsync(url, baseDirectory,repoFile)
{
}
// vim:set ts=2 :
