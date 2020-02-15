//
//  pkginfo.h
// 
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

/**
 * Pkginfo class.
 */

#ifndef PKGINFO_H
#define PKGINFO_H

#include "pkgdbh.h"
#include "runtime_dependencies_utils.h"

#include <sstream>
#include <iomanip>


class Pkginfo : public Pkgdbh {
public:
	Pkginfo(const std::string& commandName);
	Pkginfo();
	virtual void run(int argc, char** argv);
	virtual void parseArguments(int argc, char** argv);
	virtual void run();
	virtual void finish();
	virtual void printHelp() const;

protected:
	std::string m_arg;
	int m_runtimedependencies_mode;
	int m_footprint_mode;
	int m_archiveinfo;
	int m_installed_mode;
	int m_list_mode;
	int m_owner_mode;
	int m_details_mode;
	int m_libraries_mode;
	int m_runtime_mode;
	int m_epoc;

};

#endif /* PKGINFO_H */

// vim:set ts=2 :
