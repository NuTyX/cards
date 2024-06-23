/* SPDX-License-Identifier: LGPL-2.1-or-later */

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
