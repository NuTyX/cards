//  error_treat.h
// 
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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


#ifndef ERRORTREAT_H
#define ERRORTREAT_H

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define NORMAL "\033[1;0m"

#define ACTION "\033[1;33maction:\033[1;0m"
#define USAGE "\033[1;33musage: \033[1;0m"
#define OPTIONS "\033[1;33moptions\033[1;0m"
#define REQUIRED "\033[1;33mrequired\033[1;0m"
#define DESCRIPTION  "\033[1;33mdescription: \033[1;0m"
#define COMMAND "\033[1;34mcommand\033[1;0m"

#include <cstring>
#include <string>
#include <stdexcept>
#include <cerrno>

enum error
{
CANNOT_DOWNLOAD_FILE,
CANNOT_CREATE_FILE,
CANNOT_OPEN_FILE,
CANNOT_FIND_FILE,
CANNOT_READ_FILE,
CANNOT_COPY_FILE,
CANNOT_PARSE_FILE,
CANNOT_READ_DIRECTORY,
CANNOT_WRITE_FILE,
CANNOT_SYNCHRONIZE,
CANNOT_RENAME_FILE,
CANNOT_DETERMINE_NAME_BUILDNR,
WRONG_ARCHITECTURE,
EMPTY_PACKAGE,
CANNOT_FORK,
WAIT_PID_FAILED,
DATABASE_LOCKED,
CANNOT_LOCK_DIRECTORY,
CANNOT_REMOVE_FILE,
CANNOT_CREATE_DIRECTORY,
CANNOT_RENAME_DIRECTORY,
OPTION_ONE_ARGUMENT,
INVALID_OPTION,
OPTION_MISSING,
TOO_MANY_OPTIONS,
ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE,
PACKAGE_NOT_FOUND,
PACKAGE_ALLREADY_INSTALL,
PACKAGE_NOT_INSTALL,
PACKAGE_NOT_PREVIOUSLY_INSTALL,
LISTED_FILES_ALLREADY_INSTALLED,
PKGADD_CONFIG_LINE_TOO_LONG,
PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS,
PKGADD_CONFIG_UNKNOWN_ACTION,
PKGADD_CONFIG_UNKNOWN_EVENT,
CANNOT_COMPILE_REGULAR_EXPRESSION,
CANNOT_GENERATE_LEVEL,
NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE
};

class RunTimeErrorWithErrno : public std::runtime_error
{
	public:
		explicit RunTimeErrorWithErrno(const std::string& msg) throw();
		explicit RunTimeErrorWithErrno(const std::string& msg, int e) throw();
};
#endif /* ERRORTREAT_H */
// vim:set ts=2 :
