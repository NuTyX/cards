/* SPDX-License-Identifier: LGPL-2.1-or-later */

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
