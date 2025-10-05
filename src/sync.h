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

	cards::conf m_config;
	void        uncompress(std::string fileName);

public:
	sync();
	sync(const std::string configFile);

	void        run();

	static const std::string DEFAULT_PKG_REPO;
	static const std::string DEFAULT_PKG_FILES;
};

}
