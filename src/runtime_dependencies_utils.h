/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "string_utils.h"
#include "file_utils.h"
#include "elf_common.h"
#include "elf.h"

#include <sys/stat.h>


int getRuntimeLibrariesList (std::set<std::string>& runtimeLibrariesList,
		const std::string& filename);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once
