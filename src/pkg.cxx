/*
 * cards_package.cxx
 *
 * Copyright 2017 NuTyX <tnut@nutyx.org>
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

#include "pkg.h"

using namespace std;

/// Constructor
Pkg::Pkg()
{
	_status = (CPSTATUS) 0;
}

/// Destructor
Pkg::~Pkg()
{
// Nothing for moment
}
// Sets
void Pkg::setName(string& name)
{
	_name = name;
}
void Pkg::setDescription(string& description)
{
	_description = description;
}
void Pkg::setVersion(string& version)
{
	_version = version;
}
void Pkg::setCollection(string& collection)
{
	_collection = collection;
}
void Pkg::setPackager(string& packager)
{
	_packager = packager;
}
/// Return
string Pkg::getCollection()
{
	return _collection;
}

string Pkg::getName()
{
	return _name;
}

string Pkg::getVersion()
{
	return _version;
}

string Pkg::getPackager()
{
	return _packager;
}

string Pkg::getDescription()
{
	return _description;
}

bool Pkg::isInstalled()
{
	return _status & CPSTATUS::INSTALLED;
}

bool Pkg::isToBeInstalled()
{
	return _status & CPSTATUS::TO_INSTALL;
}

bool Pkg::isToBeRemoved()
{
	return _status & CPSTATUS::TO_REMOVE;
}

void Pkg::setStatus(CPSTATUS pstatus)
{
	_status = (CPSTATUS)(_status | pstatus);
}

void Pkg::unSetStatus(CPSTATUS pstatus)
{
	_status = (CPSTATUS)(_status & (~pstatus));
}

CPSTATUS Pkg::getStatus()
{
	return _status;
}
