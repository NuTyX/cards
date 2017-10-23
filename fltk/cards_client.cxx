/*
 * cards_client.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
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

#include "cards_client.h"

Cards_client::Cards_client(const string& pConfigFileName)
	: Pkginst("",pConfigFileName.c_str())
{
	m_root="/";
}

Cards_client::~Cards_client()
{
}

set<string> Cards_client::ListOfInstalledPackages()
{
	getListOfPackageNames (m_root);
	buildDatabase(false,true,false,false,"");
	set<string> ListOfInstalledPackages;
	string packageDetails;
	for (auto i : m_listOfInstPackages)
	{
#ifndef NDEBUG
		cerr << i.first << endl;
#endif
		packageDetails = i.first + '\t'
			+ i.second.version + '\t'
			+ i.second.description + '\t';

		ListOfInstalledPackages.insert(packageDetails);
	}
	return ListOfInstalledPackages;
}
