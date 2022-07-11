//
// archive_utils.cxx
//
// Copyright (c) 2013 - 2022 by NuTyX team (http://nutyx.org)
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

using namespace std;

ArchiveUtils::ArchiveUtils(const std::string& fileName)
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
	m_packageArch  = getPackageArch();
	for ( auto fileName : m_contentMtree) {
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
			throw runtime_error(gettext("could not open the archive ") + message);
			break;
		case CANNOT_READ_ARCHIVE:
			throw runtime_error(gettext("could not read the archive ") + message);
			break;
		case CANNOT_FIND_META_FILE:
			throw runtime_error(gettext("Invalid meta data in file ") + message);
			break;
		case CANNOT_FIND_MTREE_FILE:
			throw runtime_error(gettext("Invalid mtree in file ") + message);
			break;
		case EMPTY_ARCHIVE:
			throw runtime_error(message + gettext(" is not an archive"));
			break;
		case CANNOT_FIND_NAME:
			throw runtime_error(message + gettext(" is not a CARDS archive"));
		case CANNOT_FIND_ARCH:
			throw runtime_error(gettext("archive ") + message + gettext(": invalid architecture"));
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
		cout << gettext("Not found") << endl;
	} else {
		for (auto i : m_contentMtree) {
			cout << i << endl;
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
			if ( fC != nullptr ) {
				free(fC);
				fC = nullptr;
			}
			contentFile = parseDelimitedVectorList(s_contentFile, "\n");
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
void ArchiveUtils::getRunTimeDependenciesEpoch()
{
	std::pair<std::string,time_t > NameEpoch;
	for (auto dependency : m_contentMeta) {
		if ( dependency[0] == 'R' ) {
			NameEpoch.first=dependency.substr(1,dependency.size()-11);
			NameEpoch.second=strtoul((dependency.substr(dependency.size()-10)).c_str(),NULL,0);
			m_rtDependenciesEpochList.insert(NameEpoch);
		}
	}
}
void ArchiveUtils::getRunTimeDependencies()
{
	for (auto dependency : m_contentMeta) {
		if ( dependency[0] == 'R' ) {
			m_rtDependenciesList.insert(dependency.substr(1));
		}
	}
}
void ArchiveUtils::getAliasList()
{
	for (auto i : m_contentMeta) {
		if ( i[0] == 'A' ) {
			m_aliasList.insert(i.substr(1));
		}
	}
}
void ArchiveUtils::printDeps()
{
	getRunTimeDependencies();
	for ( auto i : m_rtDependenciesList)
		cout << i << endl;
}

void ArchiveUtils::printMeta()
{
	for ( auto i : m_contentMeta)
		cout << i << endl;
}
void ArchiveUtils::printInfo()
{
	if ( m_contentInfo.size() != 0) {
		for (auto i : m_contentInfo)
			cout << i << endl;
	}
}
string ArchiveUtils::getPackageName()
{
	if  ( m_contentMeta.size() == 0 ) {
		m_actualError = CANNOT_FIND_NAME;
		treatErrors(m_fileName);
		return "";
	}
	for (auto name : m_contentMeta) {
		if ( name[0] == 'N' ) {
			return name.substr(1);
			break;
		}
	}
	m_actualError = CANNOT_FIND_NAME;
	treatErrors(m_fileName);
	return "";
}
string ArchiveUtils::getPackageArch()
{
	if ( m_contentMeta.size() == 0 ) {
		m_actualError = CANNOT_FIND_ARCH;
		treatErrors(m_fileName);
		return "";
	}
	for (auto arch : m_contentMeta) {
		if ( arch[0] == 'a' ) {
			return arch.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::namebuildn()
{
	return getPackageName() + epochBuildDate();
}
set<string> ArchiveUtils::listofDependencies()
{
	m_rtDependenciesList.insert(namebuildn());
	getRunTimeDependencies();

	return m_rtDependenciesList;
}
set<string> ArchiveUtils::listofAlias()
{
	getAliasList();
	return m_aliasList;
}
set< std::pair<std::string,time_t> > ArchiveUtils::listofDependenciesBuildDate()
{
	std::pair<std::string,time_t> NameEpoch;
	NameEpoch.first=getPackageName();
	NameEpoch.second=buildn();
	m_rtDependenciesEpochList.insert(NameEpoch);
	getRunTimeDependenciesEpoch();
	return m_rtDependenciesEpochList;
}
string ArchiveUtils::name()
{
	return m_packageName;
}
string ArchiveUtils::arch()
{
	return m_packageArch;
}
string ArchiveUtils::version()
{
	for (auto version : m_contentMeta) {
		if ( version[0] == 'V' ) {
			return version.substr(1);
			break;
		}
	}
	return "";
}
int ArchiveUtils::release()
{
	for (auto release : m_contentMeta) {
		if ( release[0] == 'r' ) {
			return stoi(release.substr(1));
			break;
		}
	}
	return 0;
}
string ArchiveUtils::url()
{
	for (auto url : m_contentMeta) {
		if ( url[0] == 'U' ) {
			return url.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::description()
{
	for (auto description : m_contentMeta) {
		if ( description[0] == 'D' ) {
			return description.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::group()
{
	for (auto group : m_contentMeta) {
		if ( group[0] == 'g' ) {
			return group.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::maintainer()
{
  for (auto maintainer : m_contentMeta) {
    if ( maintainer[0] == 'M' ) {
      return maintainer.substr(1);
      break;
    }
  }
  return "";
}
string ArchiveUtils::contributors()
{
  for (auto contributors : m_contentMeta) {
    if ( contributors[0] == 'C' ) {
      return contributors.substr(1);
      break;
    }
  }
  return "";
}
string ArchiveUtils::packager()
{
  for (auto maintainer : m_contentMeta) {
    if ( maintainer[0] == 'P' ) {
      return maintainer.substr(1);
      break;
    }
  }
  return "";
}
string ArchiveUtils::collection()
{
	for (auto collection : m_contentMeta) {
		if ( collection[0] == 'c' ) {
			return collection.substr(1);
			break;
		}
	}
	return "";
}
string ArchiveUtils::builddate()
{
	char * c_time_s;
	string buildtime;
	for (auto bt : m_contentMeta) {
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
	for (auto epochSVal : m_contentMeta) {
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
	for (auto epochSVal : m_contentMeta) {
		if ( epochSVal[0] == 'B' ) {
			epochVal = strtoul((epochSVal.substr(1)).c_str(),NULL,0);
			return epochVal;
			break;
		}
	}
	return 0;
}
// vim:set ts=2 :
