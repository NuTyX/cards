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
} // end of cards namespace
