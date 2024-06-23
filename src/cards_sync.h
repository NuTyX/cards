/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgsync.h"
#include "cards_argument_parser.h"

/**
 * \brief cards_sync class
 *
 * This class takes care of synchronising packages metadatas with remote server
 *
 */

class cards_sync : public pkgsync {
public:
	/**
	 * Default constructor without any argument
	 */
	cards_sync();

	cards_sync (const CardsArgumentParser& argParser);
	cards_sync (const CardsArgumentParser& argParser,
		const std::string& url,
		const std::string& directory,
		const std::string& repoFile);

private:

	const CardsArgumentParser& m_argParser;
	
};
