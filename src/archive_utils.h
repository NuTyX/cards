//
//  archive_utils.h
// 
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - 2020 by NuTyX team (http://nutyx.org)
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

#include "string_utils.h"

#include <archive.h>
#include <archive_entry.h>


#if ARCHIVE_VERSION_NUMBER >= 3000000
#define INIT_ARCHIVE(ar) \
	archive_read_support_filter_gzip((ar)); \
	archive_read_support_filter_bzip2((ar)); \
	archive_read_support_filter_xz((ar)); \
	archive_read_support_filter_zstd((ar)); \
	archive_read_support_format_tar((ar))
#define FREE_ARCHIVE(ar) \
	archive_read_free((ar))
#else
#define INIT_ARCHIVE(ar) \
	archive_read_support_compression_gzip((ar)); \
	archive_read_support_compression_bzip2((ar)); \
	archive_read_support_compression_xz((ar)); \
	archive_read_support_format_tar((ar))
#define FREE_ARCHIVE(ar) \
	archive_read_finish((ar))
#endif

#define DEFAULT_BYTES_PER_BLOCK (20 * 512)
#define METAFILE ".META"
#define INFOFILE ".INFO"
#define MTREEFILE ".MTREE"

enum archive_error
{
CANNOT_OPEN_ARCHIVE,
CANNOT_READ_ARCHIVE,
CANNOT_FIND_META_FILE,
CANNOT_FIND_MTREE_FILE,
CANNOT_FIND_NAME,
CANNOT_FIND_ARCH,
EMPTY_ARCHIVE
};


class ArchiveUtils
{
	public:


	ArchiveUtils(const std::string& fileName);
	virtual ~ArchiveUtils();

	virtual void treatErrors(const std::string& s) const;

	void printDeps();        // Print Out the dependencies
	void printMeta();        // Print Out the .META file
	void printInfo();        // the .INFO file
	void list();             // list the files to stdio
	

	unsigned int long size();              	// Numbers of files in the archive
	std::set<std::string> setofFiles(); 		// return a order set of string
	std::set<std::string> listofDependencies(); // return an order set of dependencies
	std::set<std::string> listofAlias(); // return the alias list
	std::set<std::pair<std::string,time_t> > listofDependenciesBuildDate(); // return an order set of dependencies,BuildDate
	std::string arch();       // return the arch of the package
	std::string version();    // return the version of the package
	int  release();		// return the release of the package
	std::string description();// return the description	of the package
	std::string url();        // return the url of the package
	std::string contributors(); // return the Contributor(s) of the package
	std::string maintainer(); // return the Maintainer(s) of the package
	std::string collection(); // return the collection of the package
	std::string packager();   // return the Packager(s) of the package
	std::string builddate();  // return the date like Mon Mar 24 10:16:00 2014
	std::string name();				// return the name of the package
	std::string group();  // return the group of the package
	std::string namebuildn();	// return the name + epochvalue
	std::string epochBuildDate();		// return the epochvalue in string format
	time_t buildn();    // return the epoch value

	private:
	
	std::string getPackageName();
	std::string getPackageArch();
	std::vector<std::string>  extractFileContent(const char * fileName);
	void getRunTimeDependencies();
	void getRunTimeDependenciesEpoch();
	void getAliasList();

	unsigned int long m_size;

	std::vector<std::string> m_contentMtree;
	std::vector<std::string> m_contentMeta;
	std::vector<std::string> m_contentInfo;

	std::set<std::string> m_rtDependenciesList;
	std::set<std::pair<std::string,time_t> > m_rtDependenciesEpochList;
	std::string m_fileName;
	std::string m_packageName;
	std::string m_packageArch;
	std::set<std::string> m_filesList;
	std::set<std::string> m_aliasList;

	archive_error m_actualError;
};

int openArchive(const char *fileName);

#endif /* ARCHIVEUTILS_H */
// vim:set ts=2 :
