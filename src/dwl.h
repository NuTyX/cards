/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "file_utils.h"

#include <curl/curl.h>



/**
 * \brief File Download State class
 *
 * This class provide container to provide progress update datas
 * to client who suscribe to the progress event
 *
 */

class dwlState
{
	public:

		double dlnow;
		double dltotal;
		double dlspeed;
		std::string filename;
};

class dwl;

/**
 * \brief File Download Event class
 *
 * This abstract class provide inherit definition for a callback
 * suscriber to be callable by dwl Progress Update
 *
 */

class dwlEvent
{
		friend dwl;

	protected:

		/**
		 * Callback called when new progress data are available
		 *
		 * \param state Class container datas for current download
		 */
		virtual void OndwlProgressInfo(dwlState state){};

	public:

		dwlEvent();
		~dwlEvent();

};

/**
 * \brief File Download class
 *
 * This class takes care of the download process of the binaries and
 * other important files
 *
 */

class dwl
{
		bool			m_progress;
		std::string		m_url;
		std::string		m_downloadFileName;
		std::string		m_fileInfo;

		CURL*			m_curl;
		CURLcode		m_curlCode;
		/**
		* download effectively the request file
		*/
		void get();

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

		void SendProgressEvent(dwlState event);

		struct dwlProgress
		{
			std::string name;
			double lastruntime;
			CURL *curl;
		};

		struct curl_slist *m_slist;

		dwlProgress	m_downloadProgress;
		InfoFile	m_destinationFile;

		static std::set<dwlEvent*> m_arrCallBacks;

	public:

		/**
		* Constructor
		* Download a file
		*
		* \param url Complete downloading address including the file
		* \param dirName Destination folder
		* \param fileName file where to store the downloaded datas
		* \param progress If true, show what's going on
		*/
		dwl(std::string url,
			std::string dirName,
			std::string fileName,
			bool progress);

		/**
		* Constructor
		* Download a file
		*
		* \param fileInfo Destination file
		* \param url Complete downloading address including the file
		* \param dirName Destination folder
		* \param fileName file where to store the downloaded datas
		* \param progress If true, show what's going on
		*/
		dwl(std::string fileInfo,
			std::string url,
			std::string dirName,
			std::string fileName,
			bool progress);

		/**
		* Constructor
		* Download a list of InfoFile (a vector of InfoFile type)
		* InfoFile consist of (amount other fields.. ) Name of the file and the signature of the file to download
		*
		* \param destinationFiles List of files to download
		* \param progress If true, show what's going on
		*/
		dwl(std::vector<InfoFile> destinationFiles,
			bool progress);

		/**
		* Destructor need to cleanup all the created pointers for the internal cook
		*/
		~dwl()
		{
			curl_slist_free_all(m_slist);
			curl_global_cleanup();
			curl_easy_cleanup(m_curl);
		}

		static void SuscribeToEvents(dwlEvent* callback);
		static void UnSuscribeFromEvents(dwlEvent* callback);
};
