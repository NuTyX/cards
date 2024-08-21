/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkgadd.h"
#include "pkgrepo.h"

class pkginst : public pkgadd, public cards::pkgrepo {
public:
	pkginst(const std::string& commandName, const char *configFileName);
	void generateDependencies(const std::pair<std::string,time_t>& packageName);
	void generateDependencies();
	void downloadPackageFileName(const std::string& packageName);

protected:
	std::vector< std::pair<std::string,time_t> > m_dependenciesList;
};
