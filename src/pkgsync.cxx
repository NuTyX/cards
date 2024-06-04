//
// pkgsync.cxx
//
//  Copyright (c) 2017 - 2023 by NuTyX team (http://nutyx.org)
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

#include "pkgsync.h"

const std::string pkgsync::DEFAULT_REPOFILE = ".PKGREPO";

pkgsync::pkgsync ()
{
	m_repoFile = DEFAULT_REPOFILE;
	if (m_root.empty())
		m_root = "/";
	if (m_configFile.empty())
		m_configFile = "etc/cards.conf";
}

pkgsync::pkgsync ( const std::string& url,
	const std::string& baseDirectory,
	const std::string& repoFile)
	: m_baseDirectory(baseDirectory),
	m_remoteUrl(url)
{
	if (repoFile != "") {
		m_repoFile = repoFile;
	} else {
		m_repoFile = DEFAULT_REPOFILE;
	}
	if (m_root.empty())
		m_root = "/";
	if (m_configFile.empty())
		m_configFile = "etc/cards.conf";
}
void pkgsync::treatErrors(const std::string& s) const
{
	switch ( m_actualError )
	{
		case cards::ERROR_ENUM_CANNOT_FIND_FILE:
		case cards::ERROR_ENUM_CANNOT_PARSE_FILE:
		case cards::ERROR_ENUM_CANNOT_WRITE_FILE:
		case cards::ERROR_ENUM_CANNOT_SYNCHRONIZE:
		case cards::ERROR_ENUM_CANNOT_COPY_FILE:
		case cards::ERROR_ENUM_CANNOT_RENAME_FILE:
		case cards::ERROR_ENUM_CANNOT_DETERMINE_NAME_BUILDNR:
		case cards::ERROR_ENUM_WRONG_ARCHITECTURE:
		case cards::ERROR_ENUM_EMPTY_PACKAGE:
		case cards::ERROR_ENUM_CANNOT_FORK:
		case cards::ERROR_ENUM_WAIT_PID_FAILED:
		case cards::ERROR_ENUM_DATABASE_LOCKED:
		case cards::ERROR_ENUM_CANNOT_LOCK_DIRECTORY:
		case cards::ERROR_ENUM_CANNOT_REMOVE_FILE:
		case cards::ERROR_ENUM_CANNOT_RENAME_DIRECTORY:
		case cards::ERROR_ENUM_OPTION_ONE_ARGUMENT:
		case cards::ERROR_ENUM_INVALID_OPTION:
		case cards::ERROR_ENUM_OPTION_MISSING:
		case cards::ERROR_ENUM_TOO_MANY_OPTIONS:
		case cards::ERROR_ENUM_LISTED_FILES_ALLREADY_INSTALLED:
		case cards::ERROR_ENUM_PKGADD_CONFIG_LINE_TOO_LONG:
		case cards::ERROR_ENUM_PKGADD_CONFIG_WRONG_NUMBER_ARGUMENTS:
		case cards::ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_ACTION:
		case cards::ERROR_ENUM_PKGADD_CONFIG_UNKNOWN_EVENT:
		case cards::ERROR_ENUM_CANNOT_COMPILE_REGULAR_EXPRESSION:
		case cards::ERROR_ENUM_NOT_INSTALL_PACKAGE_NEITHER_PACKAGE_FILE:
		case cards::ERROR_ENUM_PACKAGE_NOT_FOUND:
		case cards::ERROR_ENUM_PACKAGE_ALLREADY_INSTALL:
		case cards::ERROR_ENUM_PACKAGE_NOT_INSTALL:
		case cards::ERROR_ENUM_PACKAGE_NOT_PREVIOUSLY_INSTALL:
		case cards::ERROR_ENUM_CANNOT_GENERATE_LEVEL:
		case cards::ERROR_ENUM_CANNOT_CREATE_FILE:
			break;
		case cards::ERROR_ENUM_CANNOT_DOWNLOAD_FILE:
			throw std::runtime_error("could not download " + s);
			break;
		case cards::ERROR_ENUM_CANNOT_OPEN_FILE:
			throw RunTimeErrorWithErrno("could not open " + s);
			break;
		case cards::ERROR_ENUM_CANNOT_READ_FILE:
			throw std::runtime_error("could not read " + s);
			break;
		case cards::ERROR_ENUM_CANNOT_READ_DIRECTORY:
			throw RunTimeErrorWithErrno("could not read directory " + s);
			break;
		case cards::ERROR_ENUM_CANNOT_CREATE_DIRECTORY:
			throw RunTimeErrorWithErrno("could not create directory " + s);
			break;
		case cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE:
			throw std::runtime_error(s + _(" only root can install / sync / purge / upgrade / remove packages"));
			break;
	}
}
unsigned int pkgsync::getLocalPackages(const std::string& path)
{
	m_localPackagesList.clear();
	if ( findDir( m_localPackagesList, path) != 0 ) {
		m_actualError = cards::ERROR_ENUM_CANNOT_READ_DIRECTORY;
		treatErrors(path);
	}
	return m_localPackagesList.size();	
}
std::set<std::string> pkgsync::getListOfLocalPackages(const std::string& path) {
	if (m_localPackagesList.size() == 0 ) {
		getLocalPackages(path);
		return m_localPackagesList;
	}
	return m_localPackagesList;
}
std::set<std::string> pkgsync::getListOfRemotePackages(const std::string& pkgrepoFile) {
	if (m_remotePackagesList.size() == 0 ) {
		getRemotePackages(pkgrepoFile);
		return m_remotePackagesList;
	}
	return m_remotePackagesList;
}
unsigned int pkgsync::getRemotePackages(const std::string& pkgrepoFile)
{
	m_remotePackagesList.clear();
	if ( parseFile(m_remotePackagesList,pkgrepoFile.c_str()) != 0) {
		m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
		treatErrors(pkgrepoFile);
	}
	return m_remotePackagesList.size();
}
void pkgsync::run()
{
/*	if (getuid()) {
		m_actualError = ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	} */
	std::string configFile = m_root + m_configFile;
	cards::conf config(configFile);
	for (auto DU : config.dirUrl()) {
		if (DU.url.size() == 0 ) {
			continue;
		}
		FileDownload PKGRepo(DU.url + "/" + m_repoFile,
			DU.dir,
			m_repoFile, false);
	}
}
void pkgsync::purge()
{
	if (getuid()) {
			m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
			treatErrors("");
	}
	std::string configFile = m_root + m_configFile;
	cards::conf config(configFile);
	for (auto i : config.dirUrl()) {
		if ( i.url.size() == 0)
			continue;
		if ( getLocalPackages(i.dir) == 0 )
			continue;
		for (auto dir : m_localPackagesList) {
			deleteFolder(i.dir + "/" + dir);
		}
	}
}
void pkgsync::deleteFolder(const std::string& folderName)
{
	std::set<std::string> filesToDelete;
	if ( findDir(filesToDelete, folderName) != 0 ){
			m_actualError = cards::ERROR_ENUM_CANNOT_READ_FILE;
			treatErrors(folderName);
	}
	for (auto f : filesToDelete) {
		std::string fileName = folderName + "/" + f;
#ifndef NDEBUG
		if (f->size() > 0)
			cerr << "Deleting " << fileName << endl;
#endif
		removeFile("/",fileName);
	}	
	removeFile("/",folderName);
}
void pkgsync::setRootPath(const std::string& path)
{
	m_root=path;
}
void pkgsync::setConfigFile(const std::string& file)
{
	m_configFile=file;
}
// vim:set ts=2 :
