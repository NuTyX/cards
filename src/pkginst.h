/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

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
