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
	m_archive = archive_read_new();
	INIT_ARCHIVE(m_archive);
	if (archive_read_open_filename(m_archive,
		m_fileName.c_str(),
		DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK) {
			m_actualError = CANNOT_OPEN_ARCHIVE;
			treatErrors(m_fileName);
	}
}
ArchiveUtils::~ArchiveUtils()
{
#if ARCHIVE_VERSION_NUMBER >= 3000000
  archive_read_free(m_archive);
#else
  archive_read_finish(m_archive);
#endif
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
std::set<string> ArchiveUtils::getFilesList()
{
	for (unsigned int i = 0; archive_read_next_header(m_archive, &m_entry) ==
		ARCHIVE_OK; ++i) {
			m_filesList.insert(archive_entry_pathname(m_entry));
			mode_t mode = archive_entry_mode(m_entry);
			if (S_ISREG(mode) &&
				archive_read_data_skip(m_archive) != ARCHIVE_OK) {
					m_actualError = CANNOT_READ_ARCHIVE;
					treatErrors(m_fileName);
			}
	}
	return m_filesList;
}
void ArchiveUtils::printMetaInfo()
{
	int64_t entry_size;
	char *fileContents;
	while (archive_read_next_header(m_archive,&m_entry) == ARCHIVE_OK) {
		const char *currentFile = archive_entry_pathname(m_entry);
		entry_size = archive_entry_size(m_entry);
		fileContents = (char*)malloc(entry_size);
		archive_read_data(m_archive,fileContents,entry_size);
		if(strcmp(currentFile, ".META") == 0) {
			fileContents[entry_size]='\0';
			printf("%s",fileContents);
			break;
		}
		free(fileContents);
	}
} 
// vim:set ts=2 :
