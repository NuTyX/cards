/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "cache.h"

namespace cards {

cache::cache()
{
	m_status = STATUS_ENUM_UNKNOWN;
}
cache::~cache()
{
	//nothing todo ATM
}
void cache::size(unsigned int size)
{
	m_size = size;
}
unsigned int cache::size()
{
	return m_size;
}
void cache::setStatus(StatusEnum status)
{
	m_status = (StatusEnum)(m_status | status);
}
void cache::unsetStatus(StatusEnum status)
{
	m_status = (StatusEnum)(m_status & (~status));
}
StatusEnum cache::status()
{
	return m_status;
}
void cache::name(const std::string& name)
{
	m_name=name;
}
const std::string cache::name()
{
	return m_name;
}
void cache::baseName(const std::string& name)
{
	m_name=name;
}
const std::string cache::baseName()
{
	return m_name;
}
const std::string cache::fileName()
{
	return m_fileName;
}
void cache::fileName(const std::string& filename)
{
	m_fileName = filename;
}
void cache::dirName(const std::string& dirname)
{
	m_dirName = dirname;
}
const std::string cache::dirName()
{
	return m_dirName;
}
void cache::fileDate(const std::string& filedate)
{
	m_fileDate = filedate;
}
const std::string cache::fileDate()
{
	return m_fileDate;
}
bool cache::installed()
{
	return m_status & StatusEnum::STATUS_ENUM_INSTALLED;
}
bool cache::toinstall()
{
	return m_status & StatusEnum::STATUS_ENUM_TO_INSTALL;
}
bool cache::toremove()
{
	return m_status & StatusEnum::STATUS_ENUM_TO_REMOVE;
}

} // endof cards namespace
