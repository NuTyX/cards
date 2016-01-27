//  process.h
//
//  AUTHORS:     Johannes Winkelmann, jw@tks6.net
//              Output redirection by Logan Ingalls, log@plutor.org
//  Copyright (c) 2013-2015 by NuTyX team (http://nutyx.org)
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
#ifndef PROCESS_H
#define PROCESS_H

#include "system_utils.h"
  
/* To execute a process 
	Arguments:
	1. Application to run
	2. Arguments to pass to the application
	3. File descriptor of the log file 
*/

class process
{
public:
	process( const std::string& app, const std::string& args, int fileDescriptorLog=0 );
	/* execute the process
		return the exit status of the application */
	int execute();
	/* execute the process using the shell 
	return the exit status of the application */
	int executeShell();

private:

	int exec(const int argc, char** argv);
	int execLog(const int argc, char** argv);

	int execShell(const char* shell);
	int execShellLog(const char* shell);

	string application;
	string arguments;
	int fileDescriptorLog;
};

#endif /* _PROCESS_H_ */
// vim:set ts=2 :
