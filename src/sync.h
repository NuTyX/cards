/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "conf.h"
#include "dwl.h"

#include <zstd.h>

namespace cards {
class sync
{
	std::string m_pkgRepoFile;
	std::string m_pkgFilesFile;
	std::string m_root;
	std::string m_configFile;
	bool        m_progress;

	cards::conf                 m_config;
	void        uncompress(const std::string& fileName);

public:
	sync(const std::string& configFileName,
		const bool& progress);

	void        run();

	static const std::string DEFAULT_PKG_REPO;
	static const std::string DEFAULT_PKG_FILES;
};

}
