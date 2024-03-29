//
//  pkgrm.h
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
 * Pkgrm class.
 */

#ifndef PKGRM_H
#define PKGRM_H

#include "pkgdbh.h"

class Pkgrm : public Pkgdbh {
public:
	Pkgrm(const std::string& commandName) : Pkgdbh(commandName) {}
	Pkgrm();
	void run();
	void printHelp() const;

protected:
	void getListOfManInstalledPackages ();
	std::set<std::string> m_listOfManInstalledPackages;
	std::set<std::string> m_listofDependencies;
private:
	void getDirectDependencies(std::string& name);

};

#endif /* PKGRM_H */
// vim:set ts=2 :
