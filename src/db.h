/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "pkg.h"

namespace cards {

class db : public pkg
{
public:
	db();
	~db();
	
	time_t install();
	unsigned int space();
	bool dependency();
	std::set<std::string> files;
	std::set<std::pair<std::string,time_t>> dependencies();

	void install(time_t time);
	void space(unsigned int space); 
	void dependency(bool dependency);
	void dependencies(const std::set<std::pair<std::string,time_t>> dependencies);


private:
	unsigned int m_space;      // disk occupation onces installed
	time_t       m_install;    // date of last installation
	bool         m_dependency; // If TRUE it's automatically installed, it's a dep
	std::set<std::pair<std::string,time_t>> m_dependencies;
};

} //end of 'cards' namespace
