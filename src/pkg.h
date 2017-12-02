/*
 * pkg.h
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

#ifndef  PKG_H
#define  PKG_H

#include <string>

enum CPSTATUS
{
	INSTALLED = 0x01,
	TO_INSTALL = 0x02,
	TO_REMOVE = 0x04,
	TO_UPGRADE = 0x08
};

class Pkg
{
public:
	Pkg();
	~Pkg();
	std::string getCollection();
	std::string getName();
	std::string getVersion();
	std::string getPackager();
	std::string getDescription();
	void setName(std::string& name);
	void setDescription(std::string& description);
	void setVersion(std::string& version);
	void setCollection(std::string& collection);
	void setPackager(std::string& packager);
	bool isInstalled();
	bool isToBeInstalled();
	bool isToBeRemoved();
	void setStatus(CPSTATUS pstatus);
	void unSetStatus(CPSTATUS pstatus);
	CPSTATUS getStatus();

protected:
	std::string _collection;
	std::string _name;
	std::string _version;
	std::string _packager;
	std::string _description;
	CPSTATUS _status;
};

#endif /* PKG_H */
// vim:set ts=2 :
