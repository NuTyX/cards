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

#include "string_utils.h"
#include "file_utils.h"
#include "pkgdbh.h"
#include <curl/curl.h>

#include <unistd.h>
#include <sys/stat.h>

#define PKGDWL_CONF   "/etc/pkgdwl.conf"
#define PKG_MD5SUM    ".md5sum"

class pkgdwl : public pkgdbh {
public:
	pkgdwl() : pkgdbh("pkgdwl") 
	{
		// Check the configuration file
		if ( ! checkFileExist(PKGDWL_CONF))
		{
			cout << PKGDWL_CONF  << " missing: ";
			throw runtime_error ("Cannot find the configuration file");
		}
		string config_file = root + PKGDWL_CONF;
		url = getValueOfKey(config_file,PARAM_DELIM,"url");
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
	virtual void printHelp() const;

protected:

	static int updateProgressHandle(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
	static size_t writeToStreamHandle(void *buffer, size_t size, size_t nmemb, void *stream);

	int updateProgress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
	size_t writeToStream(void *buffer, size_t size, size_t nmemb, void *stream);

	void initFileToDownload(const char * _file);

	struct dwlProgress
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
	void downloadFile(const string& url_to_download , const string& filename);
	void updateProgress();

	CURL* curl;
	CURLcode result;

	dwlProgress downloadProgress;
	DestinationFile  destination_file;

	
	string contentFile;
	string url;
	string downloadFileName;
};

#endif /* PKGDWL_H */
// vim:set ts=2 :
