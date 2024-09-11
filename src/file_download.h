/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

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
	FileDownload(std::string url,
		std::string dirName,
		std::string fileName, bool progress);

	/**
	 * Download a file with the possibility of checking the hashing of the file
	 *
	 * \param fileInfo Destination file
	 * \param url Complete downloading address including the file
	 * \param dirName Destination folder
	 * \param fileName file where to store the downloaded datas
	 * \param hash Hash to use for checking the download file
	 * \param progress If true, show what's going on
	 */
	FileDownload(std::string fileInfo,
		std::string url,
		std::string dirName,
		std::string fileName,
		std::string hash,
		bool progress);

	/**
	 * Download a list of InfoFile (a vector of InfoFile type)
	 * InfoFile consist of (amount other fields.. ) Name of the file and the signature of the file to download
	 *
	 * \param destinationFiles List of files to download
	 * \param progress If true, show what's going on
	 */
	FileDownload(std::vector<InfoFile> destinationFiles,
		bool progress);

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
	 * \return check if hashsum is OK
	 */
	bool checkHash();

	/**
	 * download effectively the request file
	 */
	void downloadFile();

	static int updateProgressHandle(void *p,
		curl_off_t dltotal,
		curl_off_t dlnow,
		curl_off_t ultotal,
		curl_off_t ulnow);

	static size_t writeToStreamHandle(void *buffer,
		size_t size,
		size_t nmemb,
		void *stream);
	int updateProgress(void *p,
		curl_off_t dltotal,
		curl_off_t dlnow,
		curl_off_t ultotal,
		curl_off_t ulnow);

	size_t writeToStream(void *buffer,
		size_t size,
		size_t nmemb,
		void *stream);

	void initFileToDownload(std::string _url,
		std::string _file);

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
	bool		m_checkHash;
	bool		m_progress;
	std::string	m_hash;
	static std::set<FileDownloadEvent*> m_arrCallBacks;
};
