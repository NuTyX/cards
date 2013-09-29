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
	bool o_force = false;

	for (int i = 1; i < argc; i++) {
		string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assert_argument(argv, argc, i);
			o_root = argv[i + 1];
			i++;
		} else if (option == "-f" || option == "--force") {
			o_force = true;
		} else if (option[0] == '-' || !o_package.empty()) {
			throw runtime_error("invalid option " + option);
		} else {
			o_package = option;
		}
	}
	if (getuid())
    throw runtime_error("only root can download/install/upgrade packages");

	if (o_package.empty())
		throw runtime_error("option missing");
		//Retrieving info about all the packages
		list_pkg(o_root);
		db_open_2();
		if (!o_force)
		{
			if (db_find_pkg(o_package))
				throw runtime_error("package " + o_package + " allready installed");
		}

		const string packagedir = root + PKG_INSTALL_DIR ;
		mkdir(packagedir.c_str(),0755);

		// Get the package via it's name's md5sum and store in /install
		// Initiate file container
		string md5sum_file = root + PKG_INSTALL_DIR + "/" + PKG_MD5SUM;
	
		init_file_to_download(md5sum_file.c_str());
		// Initiate the download
		if (curl)
			{
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
				string url_to_download = url + "/" + PKG_MD5SUM + "/" + o_package;
				result = download_file(url_to_download);
			}
		if (result != CURLE_OK)
			{
				cout << result << endl;
				throw runtime_error("TODO: Something is wrong...");
			}
		else
			if (destination_file.stream)
				fclose(destination_file.stream);
			else 
				throw runtime_error("TODO: No file created");

		// Build up the tar to download we need to replace the symbol "#" with "%23" 
		string tar_file;                                           // Name of the archive file
		set<string> md5sum_value;                                  // Even if its a set they will be only one line
		parameter_value decomp_tar_file;                           // Little trick ... parameter = name, value = 4.7.1-1.pkg.tar.xz  or whatever
		md5sum_value=get_parameter_list(md5sum_file, "  ");        // get the content of the md5sume which
		set<string>::iterator it = md5sum_value.begin();           // just one line
		tar_file=get_configuration_value(md5sum_file, "  ",*it);   // Get the archive name
		decomp_tar_file=split_parameter_value(tar_file,"#");			 // Split

		// It time to get the real archive
		string tar_file_to_download = decomp_tar_file.parameter + "%23" + decomp_tar_file.value;
	
		// Initiate the download
		string final_tar_file = root + PKG_INSTALL_DIR + "/" + tar_file;
		init_file_to_download(final_tar_file.c_str());

		// Get the archive with progress pleeeeease
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			string url_to_download = url + "/" + tar_file_to_download;
			result = download_file(url_to_download);
		}
		if (result != CURLE_OK)
		{
			cout << result << endl;
			throw runtime_error("TODO: Something is wrong...");
		}
		else if (destination_file.stream)
			fclose(destination_file.stream);
		else
			throw runtime_error("TODO: No file created");
		cout << endl;	
		cout << tar_file << " stored in " << final_tar_file << endl
				 << "To Installed it: " << endl
				 << "cd " << root + PKG_INSTALL_DIR << endl
				 << "md5sum -c .md5sum , if ok installed it with pkgadd" << endl
				 << "pkgadd " << tar_file << endl;	
}
void pkgdwl::init_file_to_download(const char *_file)
{
	destination_file.filename =_file;
	destination_file.filetime=0;
	destination_file.stream=NULL;
	download_progress.lastruntime = 0;
	download_progress.curl = curl;
}
CURLcode pkgdwl::download_file(const string url_to_download)
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
	return curl_easy_perform(curl);
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
	     << "  -f, --force         force download, even if allready installed" << endl
	     << "  -r, --root <path>   specify alternative installation root" << endl
	     << "  -v, --version       print version and exit" << endl
	     << "  -i, --install       after successfull download, the package is install" << endl
	     << "  -h, --help          print help and exit" << endl;
}
// vim:set ts=2 :
