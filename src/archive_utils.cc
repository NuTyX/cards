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
#ifndef NDEBUG
	cerr << "extractFileContent META" << endl;
#endif
	m_contentMeta = extractFileContent(METAFILE);
#ifndef NDEBUG
	cerr << "extractFileContent MTREE" << endl;
#endif
 	m_contentMtree = extractFileContent(MTREEFILE);
#ifndef NDEBUG
	cerr << "extractFileContent INFO" << endl;
#endif
 	m_contentInfo = extractFileContent(INFOFILE);

	if ( m_contentMeta.size() == 0) {
		m_actualError = CANNOT_FIND_META_FILE;
		treatErrors(m_fileName);
	}
	if ( m_contentMtree.size() == 0) {
		m_actualError = CANNOT_FIND_MTREE_FILE;
		treatErrors(m_fileName);
	}
	m_packageName  = getPackageName();
	for (vector<string>::const_iterator i = m_contentMtree.begin(); i != m_contentMtree.end(); i++) {
		string fileName = *i;
		m_filesList.insert(fileName);
	}		
}
ArchiveUtils::~ArchiveUtils()
{
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
	if (m_contentMtree.size() != 0)
		return m_contentMtree.size();
	else return 0;
}
void ArchiveUtils::list()
{
	if ( m_contentMtree.size() == 0) {
		cout << "Not found" << endl;
	} else {
		for (vector<string>::const_iterator i = m_contentMtree.begin(); i != m_contentMtree.end(); i++) {
			cout << *i << endl;
		}
	}
}
vector<string> ArchiveUtils::extractFileContent(const char * fileName)
{
	vector<string> contentFile;
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
			fC[entry_size-1]='\0';
			string s_contentFile = fC;
			// free(fC); // TODO Find out why no need to free this char pointer...
			contentFile = parseDelimitedList(s_contentFile, '\n');
			break;
		}
		++i;
		if (i > 10 ) {
			archive_read_close(ar);
			FREE_ARCHIVE(ar);
			return contentFile; // no need to go further, it didn't find it
		}
	}
	archive_read_close(ar);
	FREE_ARCHIVE(ar);
	return contentFile;
}
void ArchiveUtils::getRunTimeDependencies()
{
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++) {
		string dependencie = *i;
		if ( dependencie[0] == 'R' ) {
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
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++) {
		cout << *i << endl;
	}
}
void ArchiveUtils::printInfo()
{
	if ( m_contentInfo.size() != 0) {
		for (vector<string>::const_iterator i = m_contentInfo.begin(); i != m_contentInfo.end(); i++) {
			cout << *i << endl;
		}
	}
}
string ArchiveUtils::getPackageName()
{
	string name;
	if  ( m_contentMeta.size() == 0 ) {
		m_actualError = CANNOT_FIND_NAME;
		treatErrors(m_fileName);
		return "";
	}
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++) {
		name = *i;
		if ( name[0] == 'N' ) {
			return name.substr(1);
			break;
		}
	}
	m_actualError = CANNOT_FIND_NAME;
	treatErrors(m_fileName);
	return "";
}
string ArchiveUtils::namebuildn()
{
	return getPackageName() + epochBuildDate();
}
vector<string> ArchiveUtils::listofDependencies()
{
	m_rtDependenciesList.push_back(namebuildn());
	getRunTimeDependencies();

	return m_rtDependenciesList;
}
string ArchiveUtils::name()
{
	return m_packageName;
}
string ArchiveUtils::version()
{
	string version;
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++) {
		version = *i;
		if ( version[0] == 'V' ) {
			return version.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::description()
{
	string description;
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++) {
		description = *i;
		if ( description[0] == 'D' ) {
			return description.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::builddate()
{
	char * c_time_s;
	string bt,buildtime;
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++) {
		bt = *i;
		if ( buildtime[0] == 'B' ) {
			buildtime=bt.substr(1);
			break;
		}
	}
	time_t ct = strtoul(buildtime.c_str(),NULL,0);
	c_time_s = ctime(&ct);
	string build = c_time_s;
	return build;
}
string ArchiveUtils::epochBuildDate()
{
	string epochSVal;
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++ ) {
		epochSVal = *i;
		if ( epochSVal[0] == 'B' ) {
			return epochSVal.substr(1);
			break;
		}
	}
	return "";
}
time_t ArchiveUtils::buildn()
{
	time_t epochVal = 0;
	string epochSVal;
	for (vector<string>::const_iterator i = m_contentMeta.begin(); i != m_contentMeta.end(); i++ ) {
		epochSVal = *i;
		if ( epochSVal[0] == 'B' ) {
			epochVal = strtoul((epochSVal.substr(1)).c_str(),NULL,0);
			return epochVal;
			break;
		}
	}
	return 0;
}
// vim:set ts=2 :
