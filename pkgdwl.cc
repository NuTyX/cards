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
			assert_argument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option == "-i" || option == "--info") {
			o_info = true;
		} else if (option[0] == '-' || !o_package.empty()) {
			throw runtime_error("invalid option " + option);
		} else {
			o_package = option;
		}
	}
	if (o_package.empty())
		throw runtime_error("option missing");
	if (o_info) // Get the package via it's name's md5sum
	{

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		init_file_to_download(PKG_MD5SUM);
		download_file(url + "/" + PKG_MD5SUM + "/" + o_package);

	} else {
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		init_file_to_download(PKG_MD5SUM);
		download_file(url + "/" + PKG_MD5SUM + "/" + o_package);

		// Build up the tar to download we need to replace the symbol "#" with "%23" 

		string tar_file;                                           // Name of the archive file
		set<string> md5sum_value;                                  // Even if its a set they will be only one line
		parameter_value decomp_tar_file;                           // Little trick ... parameter = name, value = 4.7.1-1.pkg.tar.xz  or whatever
		md5sum_value=get_parameter_list(PKG_MD5SUM, "  ");        // get the content of the md5sum is ...
		set<string>::iterator it = md5sum_value.begin();           // ... just one line
		tar_file=get_configuration_value(PKG_MD5SUM, "  ",*it);   // Get the archive name
		decomp_tar_file=split_parameter_value(tar_file,"#");			 // Split it.

		// It time to get the real archive
		string tar_file_to_download = decomp_tar_file.parameter + "%23" + decomp_tar_file.value; // Rebuild de download file
		init_file_to_download(tar_file.c_str());
		// Get the archive with progress pleeeeease
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		download_file(url + "/" + tar_file_to_download);
		cout << endl;	
	}
}
void pkgdwl::init_file_to_download(const char * _file)
{
	destination_file.filename = _file;
	destination_file.filetime = 0;
	destination_file.stream = NULL;
	download_progress.lastruntime = 0;
	download_progress.curl = curl;
}
void pkgdwl::download_file(const string& url_to_download)
{
	download_progress.lastruntime = 0;
	download_progress.curl = curl;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &pkgdwl::handle_write_to_stream);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &destination_file);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &pkgdwl::handle_update_progress);
	curl_easy_setopt(curl, CURLOPT_URL,url_to_download.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR,1L);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &download_progress);
	curl_easy_setopt(curl, CURLOPT_FILETIME,1L);
#ifndef NDEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	result = curl_easy_perform(curl);
	if (result != CURLE_OK)
	{
		cout << result << endl;
		throw runtime_error("TODO: Something is wrong...");
	}
	if (destination_file.stream)
		fclose(destination_file.stream);
	else
		throw runtime_error("TODO: No file created");
}
size_t pkgdwl::write_to_stream(void *buffer, size_t size, size_t nmemb, void *stream)
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
int pkgdwl::update_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	struct DownloadProgress *CurrentProgress = (struct DownloadProgress *)p;
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
int pkgdwl::handle_update_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow)
{
	return static_cast<pkgdwl*>(p)->update_progress(p,dltotal,dlnow,ultotal,ulnow);
}
size_t pkgdwl::handle_write_to_stream(void *buffer, size_t size, size_t nmemb, void *stream)
{
	return static_cast<pkgdwl*>(stream)->write_to_stream(buffer,size,nmemb,stream);
}
void pkgdwl::print_help() const
{
	cout << "usage: " << utilname << " [options] <package>" << endl
	     << "options:" << endl
			 << "  -i, --info          download md5sum info file only" << endl
	     << "  -v, --version       print version and exit" << endl
	     << "  -h, --help          print help and exit" << endl;
}
// vim:set ts=2 :
