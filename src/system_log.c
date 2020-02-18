//
// system_log.c
//
//  Copyright (c) 2016 - 2020 by NuTyX team (http://nutyx.org)
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


#include "system_log.h"

int main(int argc, char** argv)
{
	if (argc < 3 )
		return 0;
	openlog(argv[1],LOG_CONS,LOG_LOCAL7);
	syslog (LOG_INFO,"%s",argv[2]);
	void closelog(void);
	return 0;
}
