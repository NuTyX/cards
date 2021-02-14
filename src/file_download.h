//
//  file_download.h
//
//  Copyright (c) 2013 - 2021 by NuTyX team (http://nutyx.org)
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

/**
 * FileDownload class.
 */

#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H


#include <curl/curl.h>

#include "string_utils.h"
#include "file_utils.h"
#include "pkgdbh.h"


/**
 * \brief File Download State class
 *
 * This class provide container to provide progress update datas
 * to client who suscribe to the progress event
 *
 */

class FileDownloadState
{
	public:
	double dlnow;
	double dltotal;
	double dlspeed;
	std::string filename;
};

class FileDownload;

/**
 * \brief File Download Event class
 *
 * This abstract class provide inherit definition for a callback
 * suscriber to be callable by FileDownload Progress Update
 *
 */

class FileDownloadEvent
{
	friend FileDownload;
public:
	FileDownloadEvent();
	~FileDownloadEvent();

	protected:

	/**
	 * Callback called when new progress data are available
	 *
	 * \param state Class container datas for current download
	 */
	virtual void OnFileDownloadProgressInfo(FileDownloadState state){};
};

/**
 * \brief File Download class
 *
 * This class takes care of the download process of the binaries and
 * other important files
 *
 */

class FileDownload
{
	public:

	/**
	 * Download a file without the need to check the signature of the file
	 *
	 * \param url Complete downloading address including the file
	 * \param dirName Destination folder
	 * \param fileName file where to store the downloaded datas
	 * \param progress If true, show what's going on
	 */
	FileDownload(std::string url, std::string dirName, std::string fileName, bool progress);

	/**
	 * Download a file with the possibility of checking the signature of the file
	 *
	 * \param fileInfo Destination file
	 * \param url Complete downloading address including the file
	 * \param dirName Destination folder
	 * \param fileName file where to store the downloaded datas
	 * \param MD5Sum Signature to use for checking the download file
	 * \param progress If true, show what's going on
	 */
	FileDownload(std::string fileInfo, std::string url, std::string dirName, std::string fileName, std::string MD5Sum , bool progress);

	/**
	 * Download a list of InfoFile (a vector of InfoFile type)
	 * InfoFile consist of (amount other fields.. ) Name of the file and the signature of the file to download
	 *
	 * \param destinationFiles List of files to download
	 * \param progress If true, show what's going on
	 */
	FileDownload(std::vector<InfoFile> destinationFiles,bool progress);

	/**
	 * Destructor need to cleanup all the created pointers for the internal cook
	 */
	~FileDownload()
	{
		curl_slist_free_all(m_slist);
		curl_global_cleanup();
		curl_easy_cleanup(m_curl);
	}

	static void SuscribeToEvents(FileDownloadEvent* callback);
	static void UnSuscribeFromEvents(FileDownloadEvent* callback);


	private:

	/**
	 * \return check if signature is OK
	 */
	bool checkMD5sum();

	/**
	 * download effectively the request file
	 */
	void downloadFile();

	static int updateProgressHandle(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
	static size_t writeToStreamHandle(void *buffer, size_t size, size_t nmemb, void *stream);
	int updateProgress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
	size_t writeToStream(void *buffer, size_t size, size_t nmemb, void *stream);
	void initFileToDownload(std::string _url, std::string _file);

	/**
	 * \return true if package is uptodate else false
	 */
	bool checkUpToDate();
	void SendProgressEvent(FileDownloadState event);
	struct dwlProgress
	{
		std::string name;
		double lastruntime;
		CURL *curl;
	};
	struct curl_slist *m_slist;

	CURL*		m_curl;
	CURLcode	m_curlCode;
	dwlProgress	m_downloadProgress;
	InfoFile	m_destinationFile;
	std::string	m_url;
	std::string	m_downloadFileName;
	std::string	m_fileInfo;
	bool		m_checkMD5;
	bool		m_progress;
	std::string	m_MD5Sum;
	static std::set<FileDownloadEvent*> m_arrCallBacks;
};

#endif /* FILEDOWNLOAD_H */
// vim:set ts=2 :
