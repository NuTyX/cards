/*
 * cards_package.cxx
 *
 * Copyright 2017 - 2021 NuTyX <tnut@nutyx.org>
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
	m_status = (CPSTATUS) 0;
}

/// Destructor
Pkg::~Pkg()
{
// Nothing for moment
}
// Sets
void Pkg::setName(const std::string& name)
{
	m_name = name;
}
void Pkg::setDescription(const std::string& description)
{
	m_description = description;
}
void Pkg::setVersion(const std::string& version)
{
	m_version = version;
}
void Pkg::setCollection(const std::string& collection)
{
	m_collection = collection;
}
void Pkg::setSet(const std::string& set)
{
	m_set = set;
}
void Pkg::setPackager(const std::string& packager)
{
	m_packager = packager;
}
/// Return
std::vector<std::string> Pkg::getSet()
{
	m_setList = parseDelimitedVectorList (m_set, " ");
	return m_setList;
}
std::string Pkg::getPrimarySet()
{
	m_setList = parseDelimitedVectorList (m_set, " ");
	return m_setList[0];
}
string Pkg::getCollection()
{
	return m_collection;
}

string Pkg::getName()
{
	return m_name;
}

string Pkg::getVersion()
{
	return m_version;
}

string Pkg::getPackager()
{
	return m_packager;
}

string Pkg::getDescription()
{
	return m_description;
}

bool Pkg::isInstalled()
{
	return m_status & CPSTATUS::INSTALLED;
}

bool Pkg::isToBeInstalled()
{
	return m_status & CPSTATUS::TO_INSTALL;
}

bool Pkg::isToBeRemoved()
{
	return m_status & CPSTATUS::TO_REMOVE;
}

void Pkg::setStatus(CPSTATUS pstatus)
{
	m_status = (CPSTATUS)(m_status | pstatus);
}

void Pkg::unSetStatus(CPSTATUS pstatus)
{
	m_status = (CPSTATUS)(m_status & (~pstatus));
}

CPSTATUS Pkg::getStatus()
{
	return m_status;
}
