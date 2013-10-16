//  pkgdwl.cc
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

#include "pkgdwl.h"
#include <fstream>
#include <iterator>

void pkgdwl::run(int argc, char** argv)
{
	// Check command line options
	string o_package;
	string o_root;
	bool o_info = false;

	for (int i = 1; i < argc; i++) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option == "-i" || option == "--info") {
			o_info = true;
		} else if (option[0] == '-' || !o_package.empty()) {
			actualError = INVALID_OPTION;
			treatErrors(option);
		} else {
			o_package = option;
		}
	}
	if (o_package.empty())
	{
		actualError = OPTION_MISSING;
		treatErrors("");
	}
	if (o_info) // Get the package via it's name's md5sum
	{

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		initFileToDownload(PKG_MD5SUM);
		downloadFile(url + "/" + PKG_MD5SUM + "/" + o_package,PKG_MD5SUM);

	} else {
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		initFileToDownload(PKG_MD5SUM);
		downloadFile(url + "/" + PKG_MD5SUM + "/" + o_package,PKG_MD5SUM);

		// Build up the tar to download we need to replace the symbol "#" with "%23" 

		string tar_file;                                           // Name of the archive file
		set<string> md5sum_value;                                  // Even if its a set they will be only one line
		keyValue decomp_tar_file;                           // Little trick ... parameter = name, value = 4.7.1-1.pkg.tar.xz  or whatever
		md5sum_value=getKeysList(PKG_MD5SUM, "  ");        // get the content of the md5sum is ...
		set<string>::iterator it = md5sum_value.begin();           // ... just one line
		tar_file=getValueOfKey(PKG_MD5SUM, "  ",*it);   // Get the archive name
		decomp_tar_file=split_keyValue(tar_file,"#");			 // Split it.

		// It time to get the real archive
		string tar_file_to_download = decomp_tar_file.parameter + "%23" + decomp_tar_file.value; // Rebuild de download file
		initFileToDownload(tar_file.c_str());
		// Get the archive with progressInfo pleeeeease
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		downloadFile(url + "/" + tar_file_to_download,tar_file);
		cout << endl;	
	}
}
void pkgdwl::initFileToDownload(const char * _file)
{
	destination_file.filename = _file;
	destination_file.filetime = 0;
	destination_file.stream = NULL;
	downloadProgress.lastruntime = 0;
	downloadProgress.curl = curl;
}
void pkgdwl::downloadFile(const string& url_to_download, const string& file)
{
	downloadProgress.lastruntime = 0;
	downloadProgress.curl = curl;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &pkgdwl::writeToStreamHandle);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &destination_file);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &pkgdwl::updateProgressHandle);
	curl_easy_setopt(curl, CURLOPT_URL,url_to_download.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR,1L);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &downloadProgress);
	curl_easy_setopt(curl, CURLOPT_FILETIME,1L);
#ifndef NDEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	result = curl_easy_perform(curl);
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
	}
}
size_t pkgdwl::writeToStream(void *buffer, size_t size, size_t nmemb, void *stream)
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
int pkgdwl::updateProgress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
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
int pkgdwl::updateProgressHandle(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	return static_cast<pkgdwl*>(p)->updateProgress(p,dltotal,dlnow,ultotal,ulnow);
}
size_t pkgdwl::writeToStreamHandle(void *buffer, size_t size, size_t nmemb, void *stream)
{
	return static_cast<pkgdwl*>(stream)->writeToStream(buffer,size,nmemb,stream);
}
void pkgdwl::printHelp() const
{
	cout << "usage: " << utilName << " [options] <package>" << endl
	     << "options:" << endl
			 << "  -i, --info          download md5sum info file only" << endl
	     << "  -v, --version       print version and exit" << endl
	     << "  -h, --help          print help and exit" << endl;
}
// vim:set ts=2 :
