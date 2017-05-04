//
//  repodwl.h
//
//  Copyright (c) 2002-2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014-2017 by NuTyX team (http://nutyx.org)
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
#ifndef REPODWL_H
#define REPODWL_H

#include "file_download.h"
#include "pkgrepo.h"

class Repodwl: public Pkgrepo {
public:
		Repodwl(const char *fileName);
/**
 * download the .PKGREPO of packageName
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * populate: nothing
 *
 * add: the .PKGREPO of the packageName port from the mirror
 *
 */
    void downloadPortsPkgRepo(const std::string& packageName);

/**
 * download the packagefileName
 *
 * Depends on: parsePkgRepoCollectionFile
 *
 * populater: nothing
 *
 * add: The packageFileName from the mirror
 *
 */
    void downloadPackageFileName(const std::string& packageFileName);
		bool checkBinaryExist(const std::string& packageName);			
		std::string getPackageFileName(const std::string& packageName);
};
#endif /* REPODWL_H */
// vim:set ts=2 :
