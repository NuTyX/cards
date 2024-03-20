#include "pkg.h"

namespace cards {
Pkg::Pkg()
{
// nothing to do ATM
}
Pkg::~Pkg()
{
// nothing to do ATM
}
void Pkg::description(const std::string& description)
{
	m_description = description;
}
std::string Pkg::description()
{
	return m_description;
}
void Pkg::url(const std::string& url)
{
	m_url = url;
}
std::string Pkg::url()
{
	return m_url;
}
void Pkg::maintainer(const std::string& maintainer)
{
	m_maintainer = maintainer;
}
std::string Pkg::maintainer()
{
	return m_maintainer;
}
void Pkg::packager(const std::string& packager)
{
	m_packager = packager;
}
std::string Pkg::packager()
{
	return m_packager;
}
void Pkg::contributors(const std::string& contributors)
{
	m_contributors = contributors;
}
std::string Pkg::contributors()
{
	return m_contributors;
}
void Pkg::version(const std::string& version)
{
	m_version = version;
}
const std::string Pkg::version()
{
	return m_version;
}
void Pkg::release(unsigned short int release)
{
	m_release = release;
}
unsigned short int Pkg::release()
{
	return m_release;
}
void Pkg::alias(const std::string& alias)
{
	m_alias = alias;
}
std::string Pkg::alias()
{
	return m_alias;
}
void Pkg::group(const std::string& group)
{
	m_group = group;
}
std::string Pkg::group()
{
	return m_group;
}
void Pkg::sets(const std::set<std::string>& sets)
{
	m_sets = sets;
}
std::set<std::string> Pkg::sets()
{
	return m_sets;
}
void Pkg::collection(const std::string& collection)
{
	m_collection = collection;
}
std::string Pkg::collection()
{
	return m_collection;
}
void Pkg::categories(const std::set<std::string>& categories)
{
	m_categories = categories;
}
std::set<std::string> Pkg::categories()
{
	return m_categories;
}
void Pkg::arch(const std::string& arch)
{
	m_arch = arch;
}
std::string Pkg::arch()
{
	return m_arch;
}
void Pkg::license(const std::string& license)
{
	m_license = license;
}
std::string Pkg::license()
{
	return m_license;
}
void Pkg::signature(const std::string& signature)
{
	m_signature = signature;
}
std::string Pkg::signature()
{
	return m_signature;
}
void Pkg::origin(const std::string& origin)
{
	m_origin = origin;
}
std::string Pkg::origin()
{
	return m_origin;
}
void Pkg::branch(const std::string& branch)
{
	m_branch = branch;
}
std::string Pkg::branch()
{
	return m_branch;
}
void Pkg::build(const time_t build)
{
	m_build = build;
}
time_t Pkg::build()
{
	return m_build;
}
void Pkg::dependencies(std::set<std::pair<std::string,time_t>> dependencies)
{
	m_dependencies=dependencies;
}
std::set<std::pair<std::string,time_t>> Pkg::dependencies()
{
	return m_dependencies;
}

} // end of cards namespace 
