/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "conf.h"
#include "dwl.h"

namespace cards {
class sync
{
	std::string m_repoFile;
	std::string m_root;
	std::string m_configFile;

public:
	sync();
	sync(const std::string configFile);

	void run();
	void purge();
	static const std::string DEFAULT_REPOFILE;
};

}
