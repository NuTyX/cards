#pragma once

#include "string_utils.h"
#include <string>
#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <dirent.h>
#include <sys/file.h>
#include <csignal>

namespace cards {
class lock {
public:
	lock(const std::string& m_root, bool exclusive);
	~lock();

private:
	DIR* m_dir;
	struct sigaction m_sa;
};

} // end of cards namespace

// vim:set ts:2 :
