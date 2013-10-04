//  pkgadd.h
// 
//  Copyright (c) 2000-2005 Per Liden
//  Copyright (c) 2006-2013 by CRUX team (http://crux.nu)
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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

#ifndef PKGADD_H
#define PKGADD_H

#include "string_utils.h"
#include "pkgdbh.h"
#include <vector>
#include <set>

#define PKGADD_CONF             "/etc/pkgadd.conf"
#define PKGADD_CONF_MAXLINE     1024

enum rule_event_t {
	UPGRADE,
	INSTALL
};

struct rule_t {
	rule_event_t event;
	string pattern;
	bool action;
};

class pkgadd : public pkgdbh {
public:
	pkgadd() : pkgdbh("pkgadd") {}
	virtual void run(int argc, char** argv);
	virtual void print_help() const;

private:
	vector<rule_t> read_config();
	set<string> make_keep_list(const set<string>& files, const vector<rule_t>& rules);
	set<string> apply_install_rules(const string& name, pkginfo_t& info, const vector<rule_t>& rules);
	void find_rules(const vector<rule_t>& rules, rule_event_t event, vector<rule_t>& found) const;
	bool rule_applies_to_file(const rule_t& rule, const string& file);
};

#endif /* PKGADD_H */
// vim:set ts=2 :
