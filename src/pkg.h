#pragma once

#include <string>
#include <set>

namespace cards {

class Pkg
{	
public:
	Pkg();
	~Pkg();
	std::string description();
	std::string url();
	std::string maintainer();
	std::string packager();
	std::string contributors();
	std::set<std::pair<std::string,time_t>> dependencies();

	const std::string version();
	unsigned short int release();

	std::string alias();
	std::string group();
	std::string collection();
	std::set<std::string> sets();
	std::set<std::string> categories();
	std::string arch();
	std::string license();
	std::string signature();
	std::string origin();
	std::string branch();
	time_t build();

	void description(const std::string& description);
	void url(const std::string& url);
	void maintainer(const std::string& maintainer);
	void packager(const std::string& packager);
	void contributors(const std::string& contributors);
	void dependencies(const std::set<std::pair<std::string,time_t>> dependencies);

	void version(const std::string& version);
	void release(unsigned short int release);

	void alias(const std::string& alias);
	void group(const std::string& group);
	void sets(const std::set<std::string>& sets);
	void collection(const std::string& collection);
	void categories(const std::set<std::string>& categories);
	void arch(const std::string& arch);
	void license(const std::string& license);
	void signature(const std::string& signature);
	void origin(const std::string& origin);
	void branch (const std::string& branch);
	void build(time_t time);

private:
	std::string m_description;
	std::string m_url;
	std::string m_maintainer;
	std::string m_packager;
	std::string m_contributors;

	std::string m_version;
	unsigned short int m_release;

	std::string m_alias;
	std::string m_group;
	std::string m_collection;
	std::set<std::string> m_sets;
	std::set<std::string> m_categories;
	std::string m_arch;
	std::string m_license;
	std::string m_signature;
	std::string m_origin;
	std::string m_branch;
	std::set<std::pair<std::string,time_t>> m_dependencies;

	time_t m_build;


};

} //end of 'cards' namespace
