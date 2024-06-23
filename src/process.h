/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Johannes Winkelmann, jw@tks6.net
//  Output redirection by Logan Ingalls, log@plutor.org
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "string_utils.h"

#include <list>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

  
/**
  * To execute a process
	* Arguments:
	* 1. Application to run
	* 2. Arguments to pass to the application
	* 3. File descriptor of the log file
*/

class process
{
public:
	process();
	process(const std::string& app,
		const std::string& args,
		int fileDescriptorLog=0 );

	void execute(const std::string& app,
		const std::string& arguments,
		int fileDescriptorLog=0  );

	/**
		* execute the process
		* return the exit status of the application
		*/
	int execute();

	/**
		*  execute the process using the shell
		*  return the exit status of the application
		*/
	int executeShell();

	/**
		*  return the name of the application
		*/
	std::string name();

	/**
		*  return the arguments of the application
		*/
	std::string args();

private:

	int exec(const int argc, char** argv);
	int execLog(const int argc, char** argv);

	int execShell(const char* shell);
	int execShellLog(const char* shell);

	std::string m_application;
	std::string m_arguments;
	int m_fileDescriptorLog;
};
