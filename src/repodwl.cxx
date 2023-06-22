//
//  repodwl.cxx
//
//  Copyright (c) 2016 - 2023 by NuTyX team (http://nutyx.org)
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

Repodwl::Repodwl(const char *fileName)
	: Pkgrepo(fileName)
{
}
void Repodwl::downloadPortsPkgRepo(const std::string& packageName)
{
	InfoFile downloadFile;
	std::vector<InfoFile> downloadFilesList;
	bool found = false;
	for (auto portsDirectory : m_portsDirectoryList) {
		for (auto basePackageInfo : portsDirectory.basePackageList) {
			if ( basePackageInfo.basePackageName == packageName ) {
				/**
				 *
				 * We should check if the PKGREPO of the port is available
				 * .PKGREPO file is /var/lib/pkg/depot/cli/alsa-lib/.PKGREPO
				 *
				 */

				if ( portsDirectory.Url.size() > 0 ) {
					downloadFile.url = portsDirectory.Url
						+ "/"
						+ basePackageInfo.basePackageName
						+ "/.PKGREPO";
					downloadFile.dirname = portsDirectory.Dir
						+ "/"
						+ basePackageInfo.basePackageName;
					downloadFile.filename = "/.PKGREPO";
					downloadFile.md5sum = basePackageInfo.md5SUM;
					downloadFilesList.push_back(downloadFile);
#ifndef NDEBUG
					std::cerr << portsDirectory.Dir
						+ "/"
						+ basePackageInfo.basePackageName
						<< std::endl;
#endif
				}
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	if ( downloadFilesList.size() > 0 ) {
		FileDownload FD(downloadFilesList,false);
	}
}
void Repodwl::downloadPackageFileName(const std::string& packageName)
{
	std::string basePackageName = packageName;
	std::string::size_type pos = packageName.find('.');
	if (pos != std::string::npos) {
		basePackageName=packageName.substr(0,pos);
	}

	bool found = false;
	for (auto portsDirectory : m_portsDirectoryList) {
		for (auto basePackageInfo : portsDirectory.basePackageList) {
			if ( basePackageInfo.basePackageName == basePackageName) {
#ifndef NDEBUG
				std::cerr << packageName << std::endl;
#endif
				if ( ! checkFileExist (portsDirectory.Dir + "/" + basePackageInfo.basePackageName + "/.PKGREPO"))
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				parseCurrentPackagePkgRepoFile();

				for ( auto portFilesList : m_portFilesList) {
					if (portFilesList.name == packageName) {
						std::string url = portsDirectory.Url
							+ "/"
							+ basePackageInfo.basePackageName
							+ "/"
							+ portFilesList.name
							+ basePackageInfo.s_buildDate
							+ portFilesList.arch
							+ basePackageInfo.extention;
						std::string dir = portsDirectory.Dir
							+ "/"
							+ basePackageInfo.basePackageName;
						std::string fileName = portFilesList.name
							+ basePackageInfo.s_buildDate
							+ portFilesList.arch
							+ basePackageInfo.extention;
#ifndef NDEBUG
							std::cerr << url << " "
								<< dir << " "
								<< fileName << " "
								<< portFilesList.md5SUM << " "
								<< std::endl;
#endif
						FileDownload FD(packageName + " " + getBasePackageVersion(getBasePackageName(packageName)) \
							+ "-" + itos(getBasePackageRelease(getBasePackageName(packageName))),
							url,dir,fileName,portFilesList.md5SUM,true);
							break;
					}
				}
				found=true;
				break;
			}
		}
		if (found)
			break;
	}
}
bool Repodwl::checkBinaryExist(const std::string& packageName)
{
	parsePkgRepoCollectionFile();

	std::string basePackageName = packageName;
	std::string::size_type pos = packageName.find('.');
	if (pos != std::string::npos)
		basePackageName=packageName.substr(0,pos);
#ifndef NDEBUG
	std::cerr << "basePackageName: " << basePackageName << std::endl;
#endif
	bool baseBinaryfound = false;
	bool Binaryfound = false;
	for (m_PortsDirectory_i = m_portsDirectoryList.begin();m_PortsDirectory_i !=  m_portsDirectoryList.end();++m_PortsDirectory_i) {
#ifndef NDEBUG
		std::cerr << m_PortsDirectory_i->Url << " " <<  m_PortsDirectory_i->Dir << std::endl;
#endif
		for (m_BasePackageInfo_i = m_PortsDirectory_i->basePackageList.begin(); m_BasePackageInfo_i != m_PortsDirectory_i->basePackageList.end();++m_BasePackageInfo_i) {
#ifndef NDEBUG
			std::cerr << "m_BasePackageInfo_i->basePackageName: " << m_BasePackageInfo_i->basePackageName << ":" << m_BasePackageInfo_i->s_buildDate <<"." <<  std::endl;
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
		std::string pkgRepoFile= m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/.PKGREPO";
		std::string pkgReporMD5sum = m_BasePackageInfo_i->md5SUM;
		if ( ! checkMD5sum( pkgRepoFile.c_str(), pkgReporMD5sum.c_str())) {
			downloadPortsPkgRepo(m_BasePackageInfo_i->basePackageName);
		}
		parseCurrentPackagePkgRepoFile();
		for ( m_PortFilesList_i = m_portFilesList.begin();m_PortFilesList_i != m_portFilesList.end();++m_PortFilesList_i) {
#ifndef NDEBUG
			std::cerr << m_PortFilesList_i->name << std::endl;
#endif
			if (m_PortFilesList_i->name == packageName) {
				Binaryfound = true;
				break;
			}
		}
		if (Binaryfound) {
			m_packageFileName = m_PortsDirectory_i->Dir + "/" + m_BasePackageInfo_i->basePackageName + "/" + packageName + m_BasePackageInfo_i->s_buildDate + m_PortFilesList_i-> arch + m_BasePackageInfo_i->extention;
			m_packageFileNameSignature = m_PortFilesList_i -> md5SUM;
		}
#ifndef NDEBUG
		std::cerr << packageName << " is " << m_packageFileName << std::endl;
#endif
	}
	return Binaryfound;
}
std::string Repodwl::getPackageFileName(const std::string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileName;
}
std::string Repodwl::getPackageFileNameSignature(const std::string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileNameSignature;
}
void Repodwl::setPackageFileName(const std::string& packageFileName)
{
		m_packageFileName=packageFileName;
}
// vim:set ts=2 :
