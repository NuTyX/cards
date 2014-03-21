//  file_download.h
// 
//  Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
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

#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H


#include <curl/curl.h>

#include <unistd.h>
#include <sys/stat.h>

#include "string_utils.h"
#include "file_utils.h"

class FileDownload
{
	public:

	FileDownload(std::string url, std::string dirName, std::string fileName, bool progress);
	FileDownload(std::string url, std::string dirName, std::string fileName, std::string MD5Sum , bool progress);
	~FileDownload()
	{
		curl_global_cleanup();
		curl_easy_cleanup(curl);
	}
	bool checkMD5sum();
	int downloadFile();

	private:
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

	void updateProgress();

	CURL* curl;
	CURLcode result;

	dwlProgress 		m_downloadProgress;
	DestinationFile	m_destinationFile;

	string m_url;
	string m_downloadFileName;
	bool m_checkMD5;
	string m_MD5Sum;
};

#endif /* FILEDOWNLOAD_H */
// vim:set ts=2 :
