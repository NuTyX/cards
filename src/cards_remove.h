/*
 * cards_remove.h
 *
 * Copyright 2013-2017 Thierry Nuttens <tnut@nutyx.org>
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

/**
 * Cards_remove class.
 */

#ifndef CARDS_REMOVE_H
#define CARDS_REMOVE_H

#include "pkgrm.h"
#include "cards_argument_parser.h"

/**
 * \brief Cards remove class
 *
 * This class takes care of removing installed packages, installed collection
 * or installed family of packages (base attribute).
 *
 */


class Cards_remove: public Pkgrm
{
	public:
		/**
		 * Default constructor
		 * 
		 * \param commandName Specify the used command name
		 * \param argParser CardsArgumentParser type
		 * \param configFileName Specify the configuration file name to use
		 */
		Cards_remove(const std::string& commandName,
		const CardsArgumentParser& argParser,
		const char *configFileName);

	private:
		const CardsArgumentParser& m_argParser;
};

#endif /* CARDS_REMOVE_H */
// vim:set ts=2 :
