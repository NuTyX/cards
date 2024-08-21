/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "repodwl.h"

repodwl::repodwl(const char *fileName)
	: repo(fileName)
{
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
