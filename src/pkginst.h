//
//  pkginst.h
//
//  Copyright (c) 2015 -2020 by NuTyX team (http://nutyx.org)
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

#ifndef PKGINST_H
#define PKGINST_H

#include "pkgadd.h"
#include "repodwl.h"

class pkginst : public pkgadd, public repodwl {
public:
	pkginst(const std::string& commandName, const char *configFileName);
	void generateDependencies(const std::pair<std::string,time_t>& packageName);
	void generateDependencies();

protected:
	std::vector< std::pair<std::string,time_t> > m_dependenciesList;
};
#endif /* PKGINST_H */
// vim:set ts=2 :
