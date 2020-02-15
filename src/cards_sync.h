//
// cards_sync.h
// 
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
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

#ifndef CARDS_SYNC_H
#define CARDS_SYNC_H

#include "pkgsync.h"
#include "cards_argument_parser.h"


using namespace std;

/**
 * \brief cards_sync class
 *
 * This class takes care of synchronising packages metadatas with remote server
 *
 */

class Cards_sync : public Pkgsync {
public:
	/**
	 * Default constructor without any argument
	 */
	Cards_sync();

	Cards_sync (const CardsArgumentParser& argParser);
	Cards_sync (const CardsArgumentParser& argParser,
		const std::string& url,
		const std::string& directory,
		const std::string& repoFile);

private:

	const CardsArgumentParser& m_argParser;
	
};
#endif /* CARDS_SYNC_H */
// vim:set ts=2 :
