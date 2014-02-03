//  file_download.cc
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

#include <iostream>
#include "file_download.h"
#include <fstream>
#include <iterator>

FileDownload::FileDownload(std::string url, std::string fileName)
	: m_url(url),m_downloadFileName(fileName)
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (! curl)
		throw runtime_error ("Curl error");


	initFileToDownload(m_downloadFileName.c_str());
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	downloadFile(m_url + "/" + m_downloadFileName,m_downloadFileName);
	cout << endl;	
}

	/*
	* url is the complete url adress including the file	
	* filename is the complete path 
	*/
FileDownload::FileDownload(std::string url, std::string fileName, std::string MD5Sum )
  : m_url(url),m_downloadFileName(fileName),m_MD5Sum(MD5Sum)
{
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (! curl)
    throw runtime_error ("Curl error");
  initFileToDownload(m_downloadFileName.c_str());
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
  downloadFile(m_url,m_downloadFileName);
  cout << endl;
}
void FileDownload::initFileToDownload(const char * _file)
{
	m_destinationFile.filename = _file;
	m_destinationFile.filetime = 0;
	m_destinationFile.stream = NULL;
	m_downloadProgress.lastruntime = 0;
	m_downloadProgress.curl = curl;
}
int FileDownload::downloadFile(const string& url_to_download, const string& file)
{
	int result;
	m_downloadProgress.lastruntime = 0;
	m_downloadProgress.curl = curl;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &FileDownload::writeToStreamHandle);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_destinationFile);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &FileDownload::updateProgressHandle);
	curl_easy_setopt(curl, CURLOPT_URL,url_to_download.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR,1L);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &m_downloadProgress);
	curl_easy_setopt(curl, CURLOPT_FILETIME,1L);
#ifndef NDEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	result=curl_easy_perform(curl);
	fclose(m_destinationFile.stream);
	return result;

/*	result = curl_easy_perform(curl);
	if (result != CURLE_OK)
	{
		actualError = CANNOT_DOWNLOAD_FILE;
		treatErrors(url_to_download);
	}
	if (destination_file.stream)
		fclose(destination_file.stream);
	else
	{
		actualError = CANNOT_CREATE_FILE;
		treatErrors(file);
	} */
}
size_t FileDownload::writeToStream(void *buffer, size_t size, size_t nmemb, void *stream)
{
	DestinationFile *outputf = (DestinationFile *)stream;
	if ( outputf && ! outputf->stream)
	{
		outputf->stream=fopen(outputf->filename, "w");
		if ( ! outputf->stream )
			return -1;
	}
	return fwrite(buffer,size,nmemb,outputf->stream);
}
int FileDownload::updateProgress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	struct dwlProgress *CurrentProgress = (struct dwlProgress *)p;
	CURL *curl = CurrentProgress->curl;
	double TotalTime = 0;
	double SpeedDownload = 0;
	long int FileTime = 0;
	curl_easy_getinfo(curl,CURLINFO_FILETIME,&FileTime);
	curl_easy_getinfo(curl,CURLINFO_SPEED_DOWNLOAD,&SpeedDownload);
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &TotalTime);
	fprintf(stderr,"\r%d b of %d b - %d b/sec - %d sec remain ",
	(int)dlnow, (int)dltotal, (int)SpeedDownload, (int)((dltotal - dlnow)/SpeedDownload) );
	
	return 0;
}
int FileDownload::updateProgressHandle(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	return static_cast<FileDownload*>(p)->updateProgress(p,dltotal,dlnow,ultotal,ulnow);
}
size_t FileDownload::writeToStreamHandle(void *buffer, size_t size, size_t nmemb, void *stream)
{
	return static_cast<FileDownload*>(stream)->writeToStream(buffer,size,nmemb,stream);
}
bool FileDownload::checkMD5sum()
{
	bool same = true;
	unsigned char md5sum[16];

	if ( findMD5sum(m_downloadFileName.c_str(),md5sum) ) {
		static char hexNumbers[] = {'0','1','2','3','4','5','6','7',
		                            '8','9','a','b','c','d','e','f'};

		unsigned char high, low;
		for (int i = 0; i < 16; ++i) {
			high = (md5sum[i] & 0xF0) >> 4;
			low = md5sum[i] & 0xF;
			if ( *(m_MD5Sum.c_str()+2*i) - hexNumbers[high] ||
				   *(m_MD5Sum.c_str()+2*i+1) - hexNumbers[low]) {
				same = false;
				break;
			}
		}
	} else {
		same = false;
	}
	return same;
}

// vim:set ts=2 :
