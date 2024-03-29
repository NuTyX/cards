#include "cache.h"

namespace cards {

Cache::Cache()
{
	m_status = STATUS_ENUM_UNKNOWN;
}
Cache::~Cache()
{
	//nothing todo ATM
}
void Cache::size(unsigned int size)
{
	m_size = size;
}
unsigned int Cache::size()
{
	return m_size;
}
void Cache::setStatus(StatusEnum status)
{
	m_status = (StatusEnum)(m_status | status);
}
void Cache::unsetStatus(StatusEnum status)
{
	m_status = (StatusEnum)(m_status & (~status));
}
StatusEnum Cache::status()
{
	return m_status;
}
void Cache::name(const std::string& name)
{
	m_name=name;
}
const std::string Cache::name()
{
	return m_name;
}
void Cache::dirName(const std::string& dirname)
{
	m_dirName=dirname;
}
const std::string Cache::dirName()
{
	return m_dirName;
}
void Cache::fileDate(const std::string& filedate)
{
	m_fileDate=filedate;
}
const std::string Cache::fileDate()
{
	return m_fileDate;
}
bool Cache::installed()
{
	return m_status & StatusEnum::STATUS_ENUM_INSTALLED;
}
bool Cache::toinstall()
{
	return m_status & StatusEnum::STATUS_ENUM_TO_INSTALL;
}
bool Cache::toremove()
{
	return m_status & StatusEnum::STATUS_ENUM_TO_REMOVE;
}

} // endof cards namespace
