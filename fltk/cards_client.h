/*
 * cards_client.h
 *
 * Copyright 2015-2017 Thierry Nuttens <tnut@nutyx.org>
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

#ifndef  CARDS_CLIENT_H
#define  CARDS_CLIENT_H

#include <cstddef>

#include <libcards.h>

using namespace std;

// Define Cards_wrapper singleton for friendship
class Cards_wrapper;

class Cards_client : public Pkgdbh, public Pkgrepo
{
	// Only Cards_wrapper can own this class
	friend Cards_wrapper;

protected:
	Cards_client (const string& pConfigFileName);
	~Cards_client ();
};

#endif // CARDS_WRAPPER_H
