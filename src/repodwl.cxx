//
//  repodwl.cxx
//
//  Copyright (c) 2016-2017 by NuTyX team (http://nutyx.org)
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

#include "repodwl.h"

using namespace std;

Repodwl::Repodwl(const char *fileName)
	: Pkgrepo(fileName)
{
}
void Repodwl::downloadPortsPkgRepo(const string& packageName)
{
	InfoFile downloadFile;
	vector<InfoFile> downloadFilesList;
	bool found = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			if ( m_BasePackageInfo_i->basePackageName == packageName ) {
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
/*
  We should check if the PKGREPO of the port is available
  .PKGREPO file is /var/lib/pkg/saravane/server/alsa-lib/.PKGREPO */
	if ( m_PortsDirectory_i->Url.size() > 0 ) {
		downloadFile.url = m_PortsDirectory_i->Url + "/" + m_BasePackageInfo_i->basePackageName  + "/.PKGREPO";
		downloadFile.dirname = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName;
		downloadFile.filename = "/.PKGREPO";
		downloadFile.md5sum = m_BasePackageInfo_i-> md5SUM;
		downloadFilesList.push_back(downloadFile);
	}
#ifndef NDEBUG
	cerr << m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName  << endl;
#endif
	if ( downloadFilesList.size() > 0 ) {
		FileDownload FD(downloadFilesList,false);
	}
}
void Repodwl::downloadPackageFileName(const string& packageName)
{
	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos) {
		basePackageName=packageName.substr(0,pos);
	}

	bool found = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i != m_portsDirectoryList.end();++m_PortsDirectory_i) {
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin();m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
			if ( m_BasePackageInfo_i->basePackageName == basePackageName) {
				found=true;
				break;
			}
		}
		if (found)
			break;
	}
	if (found) {
#ifndef NDEBUG
		cerr << packageName << endl;
#endif
		if ( ! checkFileExist (m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO"))
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		parseCurrentPackagePkgRepoFile();

		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
			if (m_PortFilesList_i->name == packageName)
				break;
		}

		string url = m_PortsDirectory_i->Url + "/" + m_BasePackageInfo_i->basePackageName + "/" + m_PortFilesList_i->name + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i->arch + m_BasePackageInfo_i->extention;
		string dir = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName;
		string fileName = m_PortFilesList_i->name + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i-> arch + m_BasePackageInfo_i->extention;
#ifndef NDEBUG
		cerr << url << " "
			<< dir << " "
			<< fileName << " "
			<< m_PortFilesList_i->md5SUM << " "
			<< endl;
#endif
		FileDownload FD(url,dir,fileName,m_PortFilesList_i->md5SUM,true);
	}
}
bool Repodwl::checkBinaryExist(const string& packageName)
{
	if (!m_parsePkgRepoCollectionFile)
		parsePkgRepoCollectionFile();

	string basePackageName = packageName;
	string::size_type pos = packageName.find('.');
	if (pos != string::npos)
		basePackageName=packageName.substr(0,pos);
#ifndef NDEBUG
	cerr << "basePackageName: " << basePackageName << endl;
#endif
	bool baseBinaryfound = false;
	bool Binaryfound = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
#ifndef NDEBUG
		cerr << m_PortsDirectory_i->Url << " " <<  m_PortsDirectory_i->Dir << endl;
#endif
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
#ifndef NDEBUG
			cerr << "m_BasePackageInfo_i->basePackageName: " << m_BasePackageInfo_i->basePackageName << ":" << m_BasePackageInfo_i->s_buildDate <<"." <<  endl;
#endif
			if ( m_BasePackageInfo_i->basePackageName == basePackageName ) {
				baseBinaryfound = true;
				break;
			}
		}	
		if (baseBinaryfound)
			break;
	}
	if (baseBinaryfound) {
		if ( ! checkFileExist ( m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO")) {
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		}
		string pkgRepoFile= m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO";
		string pkgReporMD5sum = m_BasePackageInfo_i->md5SUM;
		if ( ! checkMD5sum( pkgRepoFile.c_str(), pkgReporMD5sum.c_str())) {
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		}
		parseCurrentPackagePkgRepoFile();
		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
#ifndef NDEBUG
			cerr << m_PortFilesList_i->name << endl;
#endif
			if (m_PortFilesList_i->name == packageName) {
				Binaryfound = true;
				break;
			}
		}
		if (Binaryfound)
			m_packageFileName = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/" + packageName + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i-> arch + m_BasePackageInfo_i->extention;
#ifndef NDEBUG
		cerr << packageName << " is " << m_packageFileName << endl;
#endif
	}
	return Binaryfound;
}
string Repodwl::getPackageFileName(const string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileName;
}
// vim:set ts=2 :
