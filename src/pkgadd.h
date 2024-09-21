/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "pkgdbh.h"

/**
 * \brief pkgadd class
 *
 * This class takes care of adding binaries package into the database
 *
 */

class pkgadd : public pkgdbh {

	std::set<std::string>
					getKeepFileList(const std::set<std::string>& files,
						const std::vector<rule_t>& rules);

	std::set<std::string>
					applyInstallRules(const std::string& name,
						cards::db& info,
						const std::vector<rule_t>& rules);

	void			applyPostInstallRules(const std::string& name,
						cards::db& info,
						const std::vector<rule_t>& rules);

	void			getInstallRulesList(const std::vector<rule_t>& rules,
						cards::rule_event_t event,
						std::vector<rule_t>& found) const;

	void 			getPostInstallRulesList(const std::vector<rule_t>& rules,
		std::vector<rule_t>& found) const;

protected:
	std::string 	m_packageArchiveName;
	bool 			m_runPrePost;
	bool 			m_upgrade;
	bool 			m_force;

public:
	/**
	 * Default constructor without any argument
	 */
	pkgadd();
	/**
	 * Alternative constructor for derivated classes
	 * 
	 * \param commandName Name of the command used in derivated class
	 */
	pkgadd(const std::string& commandName);

	void			parseArguments(int argc, char** argv);
	void			preRun();
	void			postRun();
	virtual void 	run();
	void run(std::string&
					packageName);
	virtual void 	printHelp() const;
};
