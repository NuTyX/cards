//
//  pkgadd.h
// 
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
// 
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
//  USA.
//

/**
 * Pkgadd class.
 */

#ifndef PKGADD_H
#define PKGADD_H

#include "pkgdbh.h"

/**
 * \brief Pkgadd class
 *
 * This class takes care of adding binaries package into the database
 *
 */

class Pkgadd : public Pkgdbh {
public:
	/**
	 * Default constructor without any argument
	 */
	Pkgadd();
	/**
	 * Alternative constructor for derivated classes
	 * 
	 * \param commandName Name of the command used in derivated class
	 */
	Pkgadd(const std::string& commandName);
	void parseArguments(int argc, char** argv);
	void preRun();
	void postRun();
	virtual void run();
	virtual void printHelp() const;

protected:
	bool m_runPrePost;
	bool m_upgrade;
	bool m_force;
private:
	std::set<std::string> getKeepFileList(const std::set<std::string>& files,
		const std::vector<rule_t>& rules);
	std::set<std::string> applyInstallRules(const std::string& name,
		cards::Db& info,
		const std::vector<rule_t>& rules);
	void applyPostInstallRules(const std::string& name,
		cards::Db& info,
		const std::vector<rule_t>& rules);
	void getInstallRulesList(const std::vector<rule_t>& rules,
		rule_event_t event,
		std::vector<rule_t>& found) const;
	void getPostInstallRulesList(const std::vector<rule_t>& rules,
		std::vector<rule_t>& found) const;
};

#endif /* PKGADD_H */
// vim:set ts=2 :
