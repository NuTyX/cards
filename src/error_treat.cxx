/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "error_treat.h"

RunTimeErrorWithErrno::RunTimeErrorWithErrno(const std::string& msg) throw()
	: std::runtime_error(msg + std::string(": ") + strerror(errno))
{
}
RunTimeErrorWithErrno::RunTimeErrorWithErrno(const std::string& msg, int e) throw()
	: std::runtime_error(msg + std::string(": ") + strerror(e))
{
}
