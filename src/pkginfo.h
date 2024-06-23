/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "pkgdbh.h"
#include "runtime_dependencies_utils.h"

#include <sstream>
#include <iomanip>


class pkginfo : public pkgdbh {
public:
	pkginfo(const std::string& commandName);
	pkginfo();
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
	int m_number_mode;
	int m_list_mode;
	int m_owner_mode;
	int m_details_mode;
	int m_libraries_mode;
	int m_runtime_mode;
	int m_epoc;
	bool m_fulllist_mode;

};
