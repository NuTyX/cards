//
//  repodwl.cxx
//
//  Copyright (c) 2016 - 2024 by NuTyX team (http://nutyx.org)
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

repodwl::repodwl(const char *fileName)
	: pkgrepo(fileName)
{
}
void repodwl::downloadPortsPkgRepo(const std::string& packageName)
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
				if ( portsDirectory.url.size() > 0 ) {
					downloadFile.url = portsDirectory.url
						+ "/"
						+ basePackageInfo.basePackageName
						+ "/.PKGREPO";
					downloadFile.dirname = portsDirectory.dir
						+ "/"
						+ basePackageInfo.basePackageName;
					downloadFile.filename = "/.PKGREPO";
					downloadFile.md5sum = basePackageInfo.md5SUM;
					downloadFilesList.push_back(downloadFile);
#ifndef NDEBUG
					std::cerr << portsDirectory.dir
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
void repodwl::downloadPackageFileName(const std::string& packageName)
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
				if ( ! checkFileExist (portsDirectory.dir + "/" + basePackageInfo.basePackageName + "/.PKGREPO"))
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				parsePackagePkgRepoFile(portsDirectory.dir + "/" + basePackageInfo.basePackageName + "/.PKGREPO");

				for ( auto portFilesList : m_portFilesList) {
					if (portFilesList.name == packageName) {
						std::string url = portsDirectory.url
							+ "/"
							+ basePackageInfo.basePackageName
							+ "/"
							+ portFilesList.name
							+ basePackageInfo.s_buildDate
							+ portFilesList.arch
							+ basePackageInfo.extention;
						std::string dir = portsDirectory.dir
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
bool repodwl::checkBinaryExist(const std::string& packageName)
{
	parseCollectionPkgRepoFile();

	std::string basePackageName = packageName;
	std::string::size_type pos = packageName.find('.');
	if (pos != std::string::npos)
		basePackageName=packageName.substr(0,pos);
#ifndef NDEBUG
	std::cerr << "basePackageName: " << basePackageName << std::endl;
#endif

	for (auto portsDirectory: m_portsDirectoryList) {
#ifndef NDEBUG
		std::cerr << portsDirectory.url << " " <<  portsDirectory.dir << std::endl;
#endif
		for (auto basePackageInfo:portsDirectory.basePackageList) {
#ifndef NDEBUG
			std::cerr << "basePackageInfo.basePackageName: "
				<< basePackageInfo.basePackageName
				<< ":"
				<< basePackageInfo.s_buildDate
				<<"."
				<<  std::endl;
#endif
			if ( basePackageInfo.basePackageName == basePackageName ) {
				if ( ! checkFileExist ( portsDirectory.dir + "/" + basePackageInfo.basePackageName + "/.PKGREPO")) {
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				}
				std::string pkgRepoFile= portsDirectory.dir
					+ "/"
					+ basePackageInfo.basePackageName
					+ "/.PKGREPO";
				std::string pkgReporMD5sum = basePackageInfo.md5SUM;
				if ( ! checkMD5sum( pkgRepoFile.c_str(), pkgReporMD5sum.c_str())) {
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				}
				parsePackagePkgRepoFile(pkgRepoFile);

				for ( auto portFilesList: m_portFilesList) {
#ifndef NDEBUG
					std::cerr << portFilesList.name << std::endl;
#endif
					if (portFilesList.name == packageName) {
						m_packageFileName = portsDirectory.dir
						+ "/"
						+ basePackageInfo.basePackageName
						+ "/"
						+ packageName
						+ basePackageInfo.s_buildDate
						+ portFilesList.arch
						+ basePackageInfo.extention;
						m_packageFileNameSignature = portFilesList.md5SUM;
#ifndef NDEBUG
						std::cerr << packageName
							<< " is "
							<< m_packageFileName
							<< std::endl;
#endif
						return true;
					}
				}
			}
		}
	}
	return false;
}
std::string repodwl::getPackageFileName(const std::string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileName;
}
std::string repodwl::getPackageFileNameSignature(const std::string& packageName)
{
	m_packageFileName = packageName;
	checkBinaryExist(packageName);
	return m_packageFileNameSignature;
}
void repodwl::setPackageFileName(const std::string& packageFileName)
{
		m_packageFileName=packageFileName;
}
// vim:set ts=2 :
