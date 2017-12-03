/*
 * cards_package.h
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

#ifndef  CARDS_PACKAGE_H
#define  CARDS_PACKAGE_H

#include <cstddef>
#include <string>
#include <libcards.h>

using namespace std;

class Cards_wrapper;

class Cards_package
{
	friend Cards_wrapper;
public:
	Cards_package();
	~Cards_package();
	string getCollection();
	string getName();
	string getVersion();
	string getPackager();
	string getDescription();
	bool isInstalled();
	bool isToBeInstalled();
	bool isToBeRemoved();
	void setStatus(CPSTATUS pstatus);
	void unSetStatus(CPSTATUS pstatus);
	CPSTATUS getStatus();

protected:
	string _collection;
	string _name;
	string _version;
	string _packager;
	string _description;
	CPSTATUS _status;
};

#endif
