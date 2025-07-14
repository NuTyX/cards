/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <string>
#include <set>

namespace cards {

class pkg
{	

	std::string 			m_description;
	std::string 			m_url;
	std::string 			m_maintainer;
	std::string 			m_packager;
	std::string 			m_contributors;

	std::string 			m_version;
	unsigned short int		m_release;

	std::set<std::string>   m_libs;
	std::set<std::string> 	m_alias;
	std::set<std::string> 	m_sets;
	std::set<std::string> 	m_categories;
	std::set<std::string>   m_conflicts;

	std::string				m_group;
	std::string				m_baseName;
	std::string				m_collection;
	std::string 			m_arch;
	std::string 			m_license;
	std::string 			m_signature;
	std::string 			m_origin;
	std::string 			m_branch;
	std::string 			m_system;

	std::string 			m_hash;
	time_t 					m_build;


public:
	pkg();
	~pkg();
	const std::string&		description();
	const std::string&		url();
	const std::string& 		maintainer();
	const std::string&		packager();
	const std::string&		contributors();

	const std::string& 		version();
	unsigned short int 		release();

	const std::string&		group();
	const std::string&		baseName();
	const std::string&		collection();
	const std::set<std::string>&    libs();
	const std::set<std::string>& 	alias();
	const std::set<std::string>& 	sets();
	const std::set<std::string>&	categories();
	const std::set<std::string>&    conflicts();
	const std::string& 		arch();
	const std::string& 		license();
	const std::string&		signature();
	const std::string&		origin();
	const std::string&		branch();
	const std::string&		system();
	const std::string&		hash();
	time_t					build();

	void					description(const std::string& description);
	void					url(const std::string& url);
	void					maintainer(const std::string& maintainer);
	void					packager(const std::string& packager);
	void					contributors(const std::string& contributors);

	void					version(const std::string& version);
	void					release(unsigned short int release);

	void					libs(const std::set<std::string>& libs);
	void					alias(const std::set<std::string>& alias);
	void					sets(const std::set<std::string>& sets);
	void					categories(const std::set<std::string>& categories);
	void					conflicts(const std::set<std::string>& conflicts);

	void					collection(const std::string& collection);
	void					baseName(const std::string& baseName);
	void					group(const std::string& group);
	void					arch(const std::string& arch);
	void					license(const std::string& license);
	void					signature(const std::string& signature);
	void					hash(const std::string& hash);
	void					origin(const std::string& origin);
	void					branch (const std::string& branch);
	void					system (const std::string& system);
	void					build(time_t time);

};

} //end of 'cards' namespace
