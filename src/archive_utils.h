//  archive_utils.h
// 
//  Copyright (c) 2002 by Johannes Winkelmann
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


#ifndef ARCHIVEUTILS_H
#define ARCHIVEUTILS_H

#include <time.h>
#include <archive.h>
#include <archive_entry.h>

#include "string_utils.h"
#include "error_treat.h"

#include <iostream>
#include <iterator>
#include <stdlib.h>

#include <set>
#include <vector>


#if ARCHIVE_VERSION_NUMBER >= 3000000
#define INIT_ARCHIVE(ar) \
	archive_read_support_filter_gzip((ar)); \
	archive_read_support_filter_bzip2((ar)); \
	archive_read_support_filter_xz((ar)); \
	archive_read_support_format_tar((ar))
#else
#define INIT_ARCHIVE(ar) \
	archive_read_support_compression_gzip((ar)); \
	archive_read_support_compression_bzip2((ar)); \
	archive_read_support_compression_xz((ar)); \
	archive_read_support_format_tar((ar))
#endif

#define DEFAULT_BYTES_PER_BLOCK (20 * 512)
#define METAFILE ".META"
#define INFOFILE ".INFO"

enum archive_error
{
CANNOT_OPEN_ARCHIVE,
CANNOT_READ_ARCHIVE,
CANNOT_FIND_META_FILE,
EMPTY_ARCHIVE
};


class ArchiveUtils
{
	public:


	ArchiveUtils(const std::string& fileName);
	virtual ~ArchiveUtils();

	virtual void treatErrors(const std::string& s) const;

	void extractFiles();     // Installed files from the archive

	void printMeta();	       // Print Out the .META file
	void printInfo();        // the .INFO file
	void printPre();         // the .PRE file
	void printPost();        // the .POST file
	void printReadMe();      // and the .README file
	void list();             // list the files to stdio
	unsigned int size();              // Numbers of files in the archive
	std::set<std::string> setofFiles(); // return a set of string 
	std::string name();       // return the name
	std::string version();    // return the version
	std::string description();// return the description	
	std::string builddate();  // return the date like Mon Mar 24 10:16:00 2014

	time_t buildn();    // return the epoch value

	private:

	void getFilesList();
	void extractFileContent(const char * fileName);

	struct archive* ar;
	struct archive_entry* en;

	unsigned int m_size;
	itemList * m_contentFile;

	std::string m_fileName;
	std::set<string> m_filesList;

	archive_error m_actualError;
};

int openArchive(const char *fileName);

#endif /* ARCHIVEUTILS_H */
// vim:set ts=2 :
