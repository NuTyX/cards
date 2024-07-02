/* SPDX-License-Identifier: LGPL-2.1-or-later */

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
				 * We should check if the" + PKG_REPO of the port is available
				 * " + PKG_REPO file is /var/lib/pkg/depot/cli/alsa-lib/" + PKG_REPO
				 *
				 */
				if ( portsDirectory.url.size() > 0 ) {
					downloadFile.url = portsDirectory.url
						+ "/"
						+ basePackageInfo.basePackageName
						+ PKG_REPO;
					downloadFile.dirname = portsDirectory.dir
						+ "/"
						+ basePackageInfo.basePackageName;
					downloadFile.filename = PKG_REPO;
					downloadFile.md5sum = basePackageInfo.md5SUM;
					downloadFilesList.push_back(downloadFile);
#ifdef DEBUG
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
#ifdef DEBUG
				std::cerr << packageName << std::endl;
#endif
				if ( ! checkFileExist (portsDirectory.dir + "/" + basePackageInfo.basePackageName + PKG_REPO))
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				parsePackagePkgRepoFile(portsDirectory.dir + "/" + basePackageInfo.basePackageName + PKG_REPO);

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
#ifdef DEBUG
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
#ifdef DEBUG
	std::cerr << "basePackageName: " << basePackageName << std::endl;
#endif

	for (auto portsDirectory: m_portsDirectoryList) {
#ifdef DEBUG
		std::cerr << portsDirectory.url << " " <<  portsDirectory.dir << std::endl;
#endif
		for (auto basePackageInfo:portsDirectory.basePackageList) {
#ifdef DEBUG
			std::cerr << "basePackageInfo.basePackageName: "
				<< basePackageInfo.basePackageName
				<< ":"
				<< basePackageInfo.s_buildDate
				<<"."
				<<  std::endl;
#endif
			if ( basePackageInfo.basePackageName == basePackageName ) {
				if ( ! checkFileExist ( portsDirectory.dir + "/" + basePackageInfo.basePackageName + PKG_REPO)) {
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				}
				std::string pkgRepoFile= portsDirectory.dir
					+ "/"
					+ basePackageInfo.basePackageName
					+ PKG_REPO;
				std::string pkgReporMD5sum = basePackageInfo.md5SUM;
				if ( ! checkMD5sum( pkgRepoFile.c_str(), pkgReporMD5sum.c_str())) {
					downloadPortsPkgRepo(basePackageInfo.basePackageName);
				}
				parsePackagePkgRepoFile(pkgRepoFile);

				for ( auto portFilesList: m_portFilesList) {
#ifdef DEBUG
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
#ifdef DEBUG
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
