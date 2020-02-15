/*
 * cards_install.h
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


#ifndef CARDS_INSTALL_H
#define CARDS_INSTALL_H

#include "pkginst.h"
#include "cards_argument_parser.h"

class Cards_install: public Pkginst
{
	public:
		Cards_install(const CardsArgumentParser& argParser,
			const char *configFileName);

		/* To install the dependencies of a package
		 * created with cards create command
		 */
		Cards_install(const CardsArgumentParser& argParser,
			const char *configFileName,
			const std::vector<std::string>& listOfPackages);
			
	private:
		const CardsArgumentParser& m_argParser;
		void parseArguments();
		void getLocalePackagesList();
		Config m_config;

};

#endif /* CARDS_INSTALL_H */
// vim:set ts=2 :
