//  pkgdwl.h
// 
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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

#ifndef PKGDWL_H
#define PKGDWL_H

#include "cards.h"
#include <curl/curl.h>

#include <unistd.h>
#include <sys/stat.h>

#define PKGDWL_CONF   "/etc/pkgdwl.conf"
#define PARAM_DELIM     "="
#define PKG_MD5SUM    ".md5sum"

class pkgdwl : public cards {
public:
	pkgdwl() : cards("pkgdwl") 
	{
		// Check the configuration file
		if ( ! file_exists(PKGDWL_CONF))
		{
			cout << PKGDWL_CONF  << " missing: ";
			throw runtime_error ("Cannot find the configuration file");
		}
		string config_file = root + PKGDWL_CONF;
		url = get_configuration_value(config_file,PARAM_DELIM,"url");
		if ( url.size() < 4 )
			{
				cout << PKGDWL_CONF << ": ";
				throw runtime_error ("url not define. Did you put some spaces ...");
			}
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();
		if (! curl)
			throw runtime_error ("Curl error");
	}
	~pkgdwl()
	{
		curl_global_cleanup();
		curl_easy_cleanup(curl);
	}
	
	virtual void run(int argc, char** argv);
	virtual void print_help() const;

protected:

	static int handle_update_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
	static size_t handle_write_to_stream(void *buffer, size_t size, size_t nmemb, void *stream);

	int update_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
	size_t write_to_stream(void *buffer, size_t size, size_t nmemb, void *stream);

	void init_file_to_download(const char * _file);

	struct DownloadProgress
	{
		double lastruntime;
		CURL *curl;
	};
	struct DestinationFile
	{
		const char *filename;
		long int filetime;
		FILE *stream;
	};
	void download_file(const string& _filename);
	void update_progress();

	CURL* curl;
	CURLcode result;

	DownloadProgress download_progress;
	DestinationFile  destination_file;

	
	string content_file;
	string url;
	string download_filename;
};

#endif /* PKGDWL_H */
// vim:set ts=2 :
