/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "repodwl.h"

repodwl::repodwl(const char *fileName)
	: repo(fileName)
{
}
void repodwl::downloadPortsPkgRepo(const std::string& packageName)
{
	InfoFile downloadFile;
	std::vector<InfoFile> downloadFilesList;
	bool found = false;
	for (auto portsDirectory : m_portsDirectoryList) {
		for (auto packageInfo : portsDirectory.packagesList) {
			if ( packageInfo.baseName() == packageName ) {
				/**
				 *
				 * We should check if the" + PKG_REPO of the port is available
				 * " + PKG_REPO file is /var/lib/pkg/depot/cli/alsa-lib/" + PKG_REPO
				 *
				 */
				if ( portsDirectory.url.size() > 0 ) {
					downloadFile.url = portsDirectory.url
						+ "/"
						+ packageInfo.baseName()
						+ PKG_REPO;
					downloadFile.dirname = portsDirectory.dir
						+ "/"
						+ packageInfo.baseName();
					downloadFile.filename = PKG_REPO;
					downloadFile.sha256sum = packageInfo.sha256SUM();
					downloadFilesList.push_back(downloadFile);
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
}
bool repodwl::checkBinaryExist(const std::string& packageName)
{
	parseCollectionPkgRepoFile();

	std::string::size_type pos = packageName.find('.');
	if (pos == std::string::npos) 
		return false;

	return true;
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
