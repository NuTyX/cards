/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <iostream>
#include "file_download.h"
#include <fstream>
#include <iterator>

FileDownloadEvent::FileDownloadEvent()
{
	FileDownload::SuscribeToEvents(this);
}

FileDownloadEvent::~FileDownloadEvent()
{
	FileDownload::UnSuscribeFromEvents(this);
}

FileDownload::FileDownload(std::vector<InfoFile> downloadFiles,bool progress)
	: m_progress(progress)
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
	if (! m_curl)
		throw std::runtime_error ("Curl error");
	if ( progress ) {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
	} else {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
	}
	m_slist=nullptr;
	m_slist= curl_slist_append(m_slist, "Cache-Control: no-cache");
	for (std::vector<InfoFile>::const_iterator i = downloadFiles.begin(); i != downloadFiles.end();++i)
	{
		m_url = i->url;
		m_destinationFile.filename = i->filename;
		m_destinationFile.dirname = i->dirname;
		m_downloadFileName = i->dirname + i->filename;
		m_downloadProgress.name = i->filename;
		m_hash = i->hash;
		createRecursiveDirs(i->dirname);
		initFileToDownload(m_url,m_downloadFileName);
		downloadFile();
		if ( ! checkHash() )
			throw std::runtime_error (m_downloadFileName
			+ " "
			+ m_hash +": checksum error");
	}
}

FileDownload::FileDownload(std::string url,
	std::string dirName,
	std::string fileName,
	bool progress)
	: m_fileInfo(fileName),
		m_url(url),
		m_downloadFileName(dirName+"/"+fileName),
		m_progress(progress)
{

	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();
	if (! m_curl)
		throw std::runtime_error ("Curl error");
	m_slist=nullptr;
	m_slist= curl_slist_append(m_slist, "Cache-Control: no-cache");
	createRecursiveDirs(dirName);

	initFileToDownload(m_url, m_downloadFileName);
	if ( progress ) {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
	} else {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
	}
	downloadFile();
}

FileDownload::FileDownload(std::string fileInfo,std::string url,
	std::string dirName,
	std::string fileName,
	std::string hash,
	bool progress)
  : m_fileInfo(fileInfo),
		m_url(url),
		m_downloadFileName(dirName+"/"+fileName),
		m_hash(hash),
		m_progress(progress)
{
  curl_global_init(CURL_GLOBAL_ALL);
  m_curl = curl_easy_init();
  if (! m_curl)
    throw std::runtime_error ("Curl error");

	m_slist=nullptr;
	m_slist= curl_slist_append(m_slist, "Cache-Control: no-cache");
	createRecursiveDirs(dirName);

  initFileToDownload(m_url, m_downloadFileName);
	if ( progress ) {
  	curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
	} else {
		curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);
	}
	downloadFile();
	if ( ! checkHash() )
		throw std::runtime_error (m_downloadFileName + " " + m_hash +": checksum error");
}

void FileDownload::downloadFile()
{
	if (! m_progress)
		rotatingCursor();

	m_downloadProgress.lastruntime = 0;
	m_downloadProgress.curl = m_curl;
	m_downloadProgress.name = m_fileInfo;
	/*
	TODO Maybe there is a more efficient way to do this, but
	for the moment we never want to use server side cache
	*/
	curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_slist);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &FileDownload::writeToStreamHandle);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_destinationFile);
	curl_easy_setopt(m_curl, CURLOPT_XFERINFOFUNCTION, &FileDownload::updateProgressHandle);
	curl_easy_setopt(m_curl, CURLOPT_URL,m_url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(m_curl, CURLOPT_FAILONERROR,1L);
	curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, &m_downloadProgress);
	curl_easy_setopt(m_curl, CURLOPT_FILETIME,1L);
#ifdef DEBUG
	curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);
#endif
	m_curlCode = curl_easy_perform(m_curl);
	if (m_progress)
		std::cout << std::endl;
	if ( m_curlCode != CURLE_OK) {
		std::cerr << curl_easy_strerror(m_curlCode) << std::endl;
		throw std::runtime_error ( "\n\nURL   : " +
		m_url + "\nFILE  : " +
		m_downloadFileName + "\nHASH: " +
		m_hash +"\n\n !!! download failed !!! \n");
	}
	if (m_destinationFile.stream) {
		fclose(m_destinationFile.stream);
		long int fT = 0;
		curl_easy_getinfo(m_curl,CURLINFO_FILETIME,&fT);
		m_destinationFile.acmodtime.actime = fT;
		m_destinationFile.acmodtime.modtime = fT;
		utime ( m_destinationFile.filename.c_str(),&m_destinationFile.acmodtime);
	}
}

void FileDownload::initFileToDownload(std::string _url,
	std::string _file)
{
  m_destinationFile.url = _url;
  m_destinationFile.filename = _file;
  m_destinationFile.filetime = 0;
  m_destinationFile.stream = nullptr;
  m_downloadProgress.lastruntime = 0;
  m_downloadProgress.curl = m_curl;
}

size_t FileDownload::writeToStream(void *buffer,
	size_t size,
	size_t nmemb,
	void *stream)
{
	InfoFile *outputf = (InfoFile *)stream;
	if ( outputf && ! outputf->stream)
	{
		outputf->stream=fopen(outputf->filename.c_str(), "w");
		if ( ! outputf->stream )
			return -1;
	}
	return fwrite(buffer,size,nmemb,outputf->stream);
}

int FileDownload::updateProgress(void *p,
	curl_off_t dltotal,
	curl_off_t dlnow,
	curl_off_t ultotal,
	curl_off_t ulnow)
{
	struct dwlProgress *CurrentProgress = (struct dwlProgress *)p;
	CURL *curl = CurrentProgress->curl;
	double TotalTime = 0;
	curl_off_t SpeedDownload = 0;

	curl_easy_getinfo(curl,CURLINFO_SPEED_DOWNLOAD_T,&SpeedDownload);
	curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &TotalTime);

	if ( ( dltotal == 0 ) ||
		( SpeedDownload == 0 ) || ( dlnow == 0 ) )
			return 0;

	fprintf(stderr,"\r  %s   %sB (%sB/s) %d %% - %d s ",
	CurrentProgress->name.c_str(),
	sizeHumanRead(static_cast<int>(dlnow)).c_str(),
	sizeHumanRead(static_cast<int>(SpeedDownload)).c_str(),
	static_cast<int>(static_cast<double>(dlnow)/static_cast<double>(dltotal) * 100 ),
	static_cast<int>((dltotal - dlnow)/SpeedDownload));
	FileDownloadState state;
	state.dlnow = static_cast<double>(dlnow);
	state.dltotal = static_cast<double>(dltotal);
	state.dlspeed = static_cast<double>(SpeedDownload);
	//state.filename = m_packageName;
	SendProgressEvent(state);
	return 0;
}

int FileDownload::updateProgressHandle(void *p,
	curl_off_t dltotal,
	curl_off_t dlnow,
	curl_off_t ultotal,
	curl_off_t ulnow)
{
	return static_cast<FileDownload*>(p)->updateProgress(p,dltotal,dlnow,ultotal,ulnow);
}

size_t FileDownload::writeToStreamHandle(void *buffer,
	size_t size,
	size_t nmemb,
	void *stream)
{
	return static_cast<FileDownload*>(stream)->writeToStream(buffer,size,nmemb,stream);
}

bool FileDownload::checkHash()
{
	return ::checkHash(m_downloadFileName.c_str(),m_hash.c_str());
}

bool FileDownload::checkUpToDate()
{
	return true;
}

std::set<FileDownloadEvent*> FileDownload::m_arrCallBacks;

void FileDownload::SendProgressEvent(FileDownloadState event)
{
	for (auto it : m_arrCallBacks)
	{
		it->OnFileDownloadProgressInfo(event);
	}
}

void FileDownload::SuscribeToEvents(FileDownloadEvent* callback)
{
	if ((FileDownload::m_arrCallBacks.find(callback)==FileDownload::m_arrCallBacks.end())
		&& (callback!=nullptr))
		FileDownload::m_arrCallBacks.insert(callback);
}

void FileDownload::UnSuscribeFromEvents(FileDownloadEvent* callback)
{
	auto it =  FileDownload::m_arrCallBacks.find(callback);
	if (it != FileDownload::m_arrCallBacks.end())
		FileDownload::m_arrCallBacks.erase(it);
}
