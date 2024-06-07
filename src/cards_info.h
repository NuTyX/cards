/*
 * cards_info.h
 * 
 * Copyright 2015 - 2020 Thierry Nuttens <tnut@nutyx.org>
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
 * cards_info class.
 */

#ifndef CARDS_INFO_H
#define CARDS_INFO_H

#include "pkginfo.h"
#include "pkgrepo.h"
#include "cards_argument_parser.h"

/**
 * \brief Cards info class
 *
 * This class takes care of printing out information about installed packages, 
 * availables packages on remote server or available ports that can be compiled.
 *
 */

class cards_info: public pkginfo, public pkgrepo
{
	public:
		/**
		 * Default constructor
		 * 
		 * \param argParser CardsArgumentParser type
		 * \param configFileName Specify the configuration file name to use
		 */
		cards_info(const CardsArgumentParser& argParser,const std::string& configFileName);
			
	private:
		const CardsArgumentParser& m_argParser;
};

#endif /* CARDS_INFO_H */
// vim:set ts=2 :
