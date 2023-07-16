#include "db.h"

namespace cards {

Db::Db()
{
	//nothing todo ATM
}
Db::~Db()
{
	//nothing todo ATM
}
void Db::space(unsigned int space)
{
	m_space = space;
}
unsigned int Db::space()
{
	return m_space;
}
time_t Db::install()
{
	return m_install;
}
void Db::install(time_t time)
{
	m_install = time;
}
void Db::files(std::set<std::string> files)
{
	m_files = files;
}
std::set<std::string> Db::files()
{
	return m_files;
}
bool Db::dependency()
{
	return m_dependency;
}
void Db::dependency(bool dependency)
{
	m_dependency = dependency;
}
} // end of cards namespace
