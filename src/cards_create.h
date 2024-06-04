/*
 * cards_create.h
 * 
 * Copyright 2016 - 2020 Thierry Nuttens <tnut@nutyx.org>
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


#ifndef CARDS_CREATE_H
#define CARDS_CREATE_H

#include "pkginst.h"
#include "cards_argument_parser.h"

class Cards_create : public pkginst
{
	public:
		Cards_create(const CardsArgumentParser& argParser,
			const char *configFileName,
			const std::vector<std::string>& listOfPackages);
		Cards_create(const CardsArgumentParser& argParser,
			const char *configFileName,
			const std::string& packageName);
		void parseArguments();
			
	private:
		void createBinaries(const char *configFileName,
			const std::string& packageName);
		const CardsArgumentParser& m_argParser;
};

#endif /* CARDS_CREATE_H */ 
