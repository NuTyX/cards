/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "db.h"

namespace cards {

db::db()
{
	//nothing todo ATM
}
db::~db()
{
	//nothing todo ATM
}
void db::space(unsigned int space)
{
	m_space = space;
}
unsigned int db::space()
{
	return m_space;
}
time_t db::install()
{
	return m_install;
}
void db::install(time_t time)
{
	m_install = time;
}
bool db::dependency()
{
	return m_dependency;
}
void db::dependency(bool dependency)
{
	m_dependency = dependency;
}
void db::dependencies(std::set<std::pair<std::string,time_t>> dependencies)
{
	m_dependencies=dependencies;
}
std::set<std::pair<std::string,time_t>>& db::dependencies()
{
	return m_dependencies;
}
} // end of cards namespace
