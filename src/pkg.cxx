/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "pkg.h"

namespace cards {
pkg::pkg()
{
// nothing to do ATM
}
pkg::~pkg()
{
// nothing to do ATM
}
void pkg::description(const std::string& description)
{
	m_description = description;
}
const std::string& pkg::description()
{
	return m_description;
}
void pkg::url(const std::string& url)
{
	m_url = url;
}
const std::string& pkg::url()
{
	return m_url;
}
void pkg::maintainer(const std::string& maintainer)
{
	m_maintainer = maintainer;
}
const std::string& pkg::maintainer()
{
	return m_maintainer;
}
void pkg::packager(const std::string& packager)
{
	m_packager = packager;
}
const std::string& pkg::packager()
{
	return m_packager;
}
void pkg::contributors(const std::string& contributors)
{
	m_contributors = contributors;
}
const std::string& pkg::contributors()
{
	return m_contributors;
}
void pkg::version(const std::string& version)
{
	m_version = version;
}
const std::string& pkg::version()
{
	return m_version;
}
const std::string& pkg::hash()
{
	return m_hash;
}
void pkg::hash(const std::string& hash)
{
	m_hash = hash;
}
void pkg::release(unsigned short int release)
{
	m_release = release;
}
unsigned short int pkg::release()
{
	return m_release;
}
void pkg::libs(const std::set<std::string>& libs)
{
	m_libs = libs;
}
const std::set<std::string>& pkg::libs()
{
	return m_libs;
}
void pkg::alias(const std::set<std::string>& alias)
{
	m_alias = alias;
}
const std::set<std::string>& pkg::alias()
{
	return m_alias;
}
void pkg::group(const std::string& group)
{
	m_group = group;
}
const std::string& pkg::group()
{
	return m_group;
}
void pkg::sets(const std::set<std::string>& sets)
{
	m_sets = sets;
}
const std::set<std::string>& pkg::sets()
{
	return m_sets;
}
void pkg::baseName(const std::string& name)
{
	m_baseName=name;
}
const std::string& pkg::baseName()
{
	return m_baseName;
}
void pkg::collection(const std::string& collection)
{
	m_collection = collection;
}
const std::string& pkg::collection()
{
	return m_collection;
}
void pkg::categories(const std::set<std::string>& categories)
{
	m_categories = categories;
}
const std::set<std::string>& pkg::categories()
{
	return m_categories;
}
void pkg::conflicts(const std::set<std::string>& conflicts)
{
	m_conflicts = conflicts;
}
const std::set<std::string>& pkg::conflicts()
{
	return m_conflicts;
}
void pkg::arch(const std::string& arch)
{
	m_arch = arch;
}
const std::string& pkg::arch()
{
	return m_arch;
}
void pkg::license(const std::string& license)
{
	m_license = license;
}
const std::string& pkg::license()
{
	return m_license;
}
void pkg::signature(const std::string& signature)
{
	m_signature = signature;
}
const std::string& pkg::signature()
{
	return m_signature;
}
void pkg::origin(const std::string& origin)
{
	m_origin = origin;
}
const std::string& pkg::origin()
{
	return m_origin;
}
void pkg::branch(const std::string& branch)
{
	m_branch = branch;
}
const std::string& pkg::branch()
{
	return m_branch;
}
void pkg::system(const std::string& system)
{
	m_system = system;
}
const std::string& pkg::system()
{
	return m_system;
}
void pkg::build(const time_t build)
{
	m_build = build;
}
time_t pkg::build()
{
	return m_build;
}
} // end of cards namespace 
