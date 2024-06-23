/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "system_utils.h"

#include <cstring>

class RunTimeErrorWithErrno : public std::runtime_error
{
	public:
		explicit RunTimeErrorWithErrno(const std::string& msg) throw();
		explicit RunTimeErrorWithErrno(const std::string& msg, int e) throw();
};
