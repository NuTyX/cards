// archive_utils.cc
//
// Copyright (c) 2013-2014 by NuTyX team (http://nutyx.org)
//
// This program is free software; you can redistribute it and/or modify
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

#include "archive_utils.h"

ArchiveUtils::ArchiveUtils(const string& fileName)
	: m_fileName(fileName)
{

/*

	 TODO Find a beter way, I'm affraid not possible

	ar = archive_read_new();

	INIT_ARCHIVE(m_archive);
	if (archive_read_open_filename(m_archive,
		m_fileName.c_str(),
		DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK) {
			m_actualError = CANNOT_OPEN_ARCHIVE;
			treatErrors(m_fileName);
	} 
*/
	
}
ArchiveUtils::~ArchiveUtils()
{

#if ARCHIVE_VERSION_NUMBER >= 3000000
  archive_read_free(ar);
#else
  archive_read_finish(ar);
#endif 

	if ( m_contentFile != NULL ) {
		freeItemList(m_contentFile);
	}
}
void ArchiveUtils::treatErrors(const std::string& message) const
{
	switch (m_actualError)
	{
		case CANNOT_OPEN_ARCHIVE:
			throw runtime_error("could not open the archive " + message);
			break;
		case CANNOT_READ_ARCHIVE:
			throw runtime_error("could not read the archive " + message);
			break;
		case CANNOT_FIND_META_FILE:
			throw runtime_error("could not find the meta file " + message);
			break;
		case EMPTY_ARCHIVE:
			throw RunTimeErrorWithErrno("is not an archive " + message);
	}
}
void ArchiveUtils::getFilesList()
{
	ar = archive_read_new();
	INIT_ARCHIVE(ar);
	if (archive_read_open_filename(ar,
    m_fileName.c_str(),
    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK) {
      m_actualError = CANNOT_OPEN_ARCHIVE;
      treatErrors(m_fileName);
  }
	for (unsigned int i = 0; archive_read_next_header(ar, &en) ==
		ARCHIVE_OK; ++i) {
			m_filesList.insert(archive_entry_pathname(en));
			mode_t mode = archive_entry_mode(en);
			if (S_ISREG(mode) &&
				archive_read_data_skip(ar) != ARCHIVE_OK) {
					m_actualError = CANNOT_READ_ARCHIVE;
					treatErrors(m_fileName);
			}
	}
}
void ArchiveUtils::list()
{
	getFilesList();
	for (set<string>::iterator i = m_filesList.begin();i != m_filesList.end();++i) {
		cout << *i << endl;
	}
}
void ArchiveUtils::extractFileContent(const char * fileName)
{
	ar = archive_read_new();
	INIT_ARCHIVE(ar);
	if (archive_read_open_filename(ar,
    m_fileName.c_str(),
    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK) {
      m_actualError = CANNOT_OPEN_ARCHIVE;
      treatErrors(m_fileName);
  }
	int64_t entry_size;
	char * fC; // file content
	while (archive_read_next_header(ar,&en) == ARCHIVE_OK) {
		const char *currentFile = archive_entry_pathname(en);
		entry_size = archive_entry_size(en);
		fC = (char*)Malloc(entry_size);
		archive_read_data(ar,fC,entry_size);
		if(strcmp(currentFile, fileName) == 0) {
			fC[entry_size]='\0';
			m_contentFile = parseDelimitedItemList(fC, "\n");
			break;
			free(fC);
		}
	}
}
void ArchiveUtils::printMeta()
{
	extractFileContent(METAFILE);
	for (unsigned int i=0; i< m_contentFile->count ; ++i) {
		cout << m_contentFile->items[i] << endl;
	}
}
void ArchiveUtils::printInfo()
{
	extractFileContent(INFOFILE);
	for (unsigned int i=0; i< m_contentFile->count ; ++i) {
		cout << m_contentFile->items[i] << endl;
	}
}
string ArchiveUtils::name()
{
	string name;
	extractFileContent(METAFILE);
	for (unsigned int i=0; i< m_contentFile->count ; ++i) {
		if ( m_contentFile->items[i][0] == 'N' ) {
			name = m_contentFile->items[i];
			break;
		}
	}
	return name.substr(2);
}
string ArchiveUtils::version()
{
	string version;
	extractFileContent(METAFILE);
	for (unsigned int i=0; i< m_contentFile->count ; ++i) {
		if ( m_contentFile->items[i][0] == 'V' ) {
			version = m_contentFile->items[i];
			break;
		}
	}
	return version.substr(2);
}
string ArchiveUtils::builddate()
{
	char * c_time_s;
	string buildtime;
	extractFileContent(METAFILE);
	for (unsigned int i=0; i< m_contentFile->count ; ++i) {
		if ( m_contentFile->items[i][0] == 'B' ) {
			string buildtimel=m_contentFile->items[i];
			buildtime=buildtimel.substr(2);
			break;
		}
	}
	time_t ct = strtoul(buildtime.c_str(),NULL,0);
	c_time_s = ctime(&ct);
	string build = c_time_s;
	return build;
}
// vim:set ts=2 :
