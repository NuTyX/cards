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
	m_contentMtree = NULL;
	m_contentMeta = NULL;
	m_contentInfo = NULL;

	m_contentMeta = extractFileContent(METAFILE);
 	m_contentMtree = extractFileContent(MTREEFILE);
 	m_contentInfo = extractFileContent(INFOFILE);

	if ( m_contentMeta == NULL) {
		m_actualError = CANNOT_FIND_META_FILE;
		treatErrors(m_fileName);
	}
	if ( m_contentMtree == NULL) {
		m_actualError = CANNOT_FIND_MTREE_FILE;
		treatErrors(m_fileName);
	}
	m_packageName  = getPackageName();
	for (unsigned int i = 0 ; i < m_contentMtree->count ; ++i) {
		m_filesList.insert(m_contentMtree->items[i]);
	}


}
ArchiveUtils::~ArchiveUtils()
{
	if ( m_contentMeta != NULL ) {
		freeItemList(m_contentMeta);
	}
	if ( m_contentInfo != NULL) {
		freeItemList(m_contentInfo);
	}
	if ( m_contentMtree != NULL) {
		freeItemList(m_contentMtree);
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
		case CANNOT_FIND_MTREE_FILE:
			throw runtime_error("could not find the mtree file " + message);
			break;
		case EMPTY_ARCHIVE:
			throw runtime_error(message + " is not an archive");
			break;
		case CANNOT_FIND_NAME:
			throw runtime_error(message + " is not a CARDS archive");
			break;
	}
}
set<string> ArchiveUtils::setofFiles()
{
	return m_filesList;
}
unsigned int long ArchiveUtils::size()
{
	if (m_contentMtree != NULL)
		return m_contentMtree->count;
	else return 0;
}
void ArchiveUtils::list()
{
	if ( m_contentMtree == NULL) {
		cout << "Not found" << endl;
	} else {
		for (unsigned int  i = 0; i < m_contentMtree -> count ;++i) {
			cout << m_contentMtree->items[i] << endl;
		}
	}
}
itemList * ArchiveUtils::extractFileContent(const char * fileName)
{
	
	itemList * contentFile = NULL;
	struct archive* ar;
  struct archive_entry* ae;

	ar = archive_read_new();
	INIT_ARCHIVE(ar);
	if (archive_read_open_filename(ar,
    m_fileName.c_str(),
    DEFAULT_BYTES_PER_BLOCK) != ARCHIVE_OK) {
      m_actualError = CANNOT_OPEN_ARCHIVE;
      treatErrors(m_fileName);
  } 
	unsigned int i = 0;
	int64_t entry_size;
	while (archive_read_next_header(ar,&ae) == ARCHIVE_OK) {
		const char *currentFile = archive_entry_pathname(ae);
		if(strcmp(currentFile, fileName) == 0) {
			entry_size = archive_entry_size(ae);
			char *fC = (char*)Malloc(entry_size);
			archive_read_data(ar,fC,entry_size);
			fC[entry_size]='\0';
			contentFile = parseDelimitedItemList(fC, "\n");
			break;
		}
		++i;
		if (i > 10 ) {
			archive_read_close(ar);
			FREE_ARCHIVE(ar);
			return NULL; // no need to go further, it didn't find it
		}
	}
	archive_read_close(ar);
	FREE_ARCHIVE(ar);
	return contentFile;
}
void ArchiveUtils::getRunTimeDependencies()
{
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		if ( m_contentMeta->items[i][0] == 'R' ) {
		string dependencie= m_contentMeta->items[i];
		m_rtDependenciesList.push_back(dependencie.substr(1));
		}
	}
}
void ArchiveUtils::printDeps()
{
	getRunTimeDependencies();
	for (vector<string>::const_iterator i = m_rtDependenciesList.begin();i!= m_rtDependenciesList.end();++i) {
		cout << *i << endl;
	}
}

void ArchiveUtils::printMeta()
{
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		cout << m_contentMeta->items[i] << endl;
	}
}
void ArchiveUtils::printInfo()
{
	if ( m_contentInfo != NULL) {
		for (unsigned int i=0; i< m_contentInfo->count ; ++i) {
			cout << m_contentInfo->items[i] << endl;
		}
	}
}
string ArchiveUtils::getPackageName()
{
	string name;
	if  ( m_contentMeta == NULL ) {
		m_actualError = CANNOT_FIND_NAME;
		treatErrors(m_fileName);
		return "";
	}
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		if ( m_contentMeta->items[i][0] == 'N' ) {
			name = m_contentMeta->items[i];
			return name.substr(1);
			break;
		}
	}
	m_actualError = CANNOT_FIND_NAME;
	treatErrors(m_fileName);
	return "";
}
string ArchiveUtils::name()
{
	return m_packageName;
}
string ArchiveUtils::version()
{
	string version;
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		if ( m_contentMeta->items[i][0] == 'V' ) {
			version = m_contentMeta->items[i];
			return version.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::description()
{
	string description;
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		if ( m_contentMeta->items[i][0] == 'D' ) {
			description = m_contentMeta->items[i];
			return description.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::builddate()
{
	char * c_time_s;
	string buildtime;
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		if ( m_contentMeta->items[i][0] == 'B' ) {
			string buildtimel=m_contentMeta->items[i];
			buildtime=buildtimel.substr(1);
			break;
		}
	}
	time_t ct = strtoul(buildtime.c_str(),NULL,0);
	c_time_s = ctime(&ct);
	string build = c_time_s;
	return build;
}
time_t ArchiveUtils::buildn()
{
	time_t epochVal = 0;
	string epochSVal;
	for (unsigned int i=0; i< m_contentMeta->count ; ++i) {
		if ( m_contentMeta->items[i][0] == 'B' ) {
			epochSVal = m_contentMeta->items[i];
			epochVal = strtoul((epochSVal.substr(1)).c_str(),NULL,0);
			return epochVal;
			break;
		}
	}
	return 0;
}
// vim:set ts=2 :
