/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "pkgdbh.h"

class pkgrm : public pkgdbh {
public:
	pkgrm(const std::string& commandName) : pkgdbh(commandName) {}
	pkgrm();
	void run();
	void printHelp() const;

protected:
	void getListOfManInstalledPackages ();
	std::set<std::string> m_listOfManInstalledPackages;
	std::set<std::string> m_listofDependencies;
private:
	void getDirectDependencies(std::string& name);

};
