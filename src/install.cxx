/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "install.h"

namespace cards {

install::install(const CardsArgumentParser& argParser,
		const std::string& configFileName)
	: m_pkgrepo(configFileName)
	, m_argParser(argParser)
	, m_configFileName(configFileName)
{
	m_progress = true;
	parseArguments();
	for(auto i : m_argParser.otherArguments()) {
		if (checkFileExist(i)) {
			continue;
		}
		if ( m_pkgrepo.getListOfPackagesFromCollection(i).empty() &&
			(! m_pkgrepo.checkBinaryExist(i) )  &&
			m_pkgrepo.getListOfPackagesFromSet(i).empty() ){
				m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_FOUND;
				treatErrors(i);
		}
	}

	buildDatabase(true);
	for(auto pkg : m_argParser.otherArguments()) {
		std::set<std::string> SetOfPackages = m_pkgrepo.getListOfPackagesFromSet(pkg);
		if (SetOfPackages.empty())
			SetOfPackages = m_pkgrepo.getListOfPackagesFromCollection(pkg);

		if (!SetOfPackages.empty())  {
			// It's a collection or a set
			for (auto i : SetOfPackages ) {
				if (checkPackageNameExist(i))
					continue;

				m_pkgrepo.generateDependencies(i);
			}
		} else if (checkRegularFile(pkg)) {
			 // It's a regular archive file
			archive packageArchive(pkg);
			std::string p = "";
			if (checkPackageNameExist(packageArchive.name())) {
				p = "Upgrade ";
				m_upgrade=1;
			} else {
				m_upgrade=0;
			}

			m_packageArchiveName = pkg;
			run();

			p += "("
				+ packageArchive.collection()
				+ ") "
				+ packageArchive.name()
				+ "-"
				+ packageArchive.version()
				+ "-"
				+ itos(packageArchive.release());

			syslog(LOG_INFO,"%s",p.c_str());
		} else {
			// It's a normal package
			m_pkgrepo.generateDependencies(pkg);
		}
	}

	getLocalePackagesList();

	for (auto i : m_pkgrepo.getDependenciesList()) {
		m_packageArchiveName = m_pkgrepo.dirName(i.first) + "/" + m_pkgrepo.fileName(i.first);
		archive packageArchive(m_packageArchiveName.c_str());
		std::string p = "";
		if (checkPackageNameExist(packageArchive.name())) {
			m_upgrade=1;
			p = "Upgrade ";
		} else {
			m_upgrade=0;
		}
		if (i.second > 0)
			setDependency();
		p += "("
			+ packageArchive.collection()
			+ ") "
			+ packageArchive.name()
			+ "-"
			+ packageArchive.version()
			+ "-"
			+itos(packageArchive.release());
		run();
		if (!m_argParser.isSet(CardsArgumentParser::OPT_NOLOGENTRY))
			syslog(LOG_INFO,"%s",p.c_str());

	}
}
void install::parseArguments()
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root = m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT)
			+ m_root;

	if (getuid()) {
		m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	if (m_argParser.isSet(CardsArgumentParser::OPT_FORCE))
		m_force=true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_UPDATE))
		m_upgrade=true;
	if (m_argParser.isSet(CardsArgumentParser::OPT_DISABLE))
		m_runPrePost = false;
}
void install::getLocalePackagesList()
{
	std::string packageFileName;
	cards::conf config(m_configFileName);

	if (config.groups().empty())
		return;

	std::set<std::string> tmpList;
	for (auto i : config.groups()) {
		for (auto j : m_pkgrepo.getDependenciesList()) {
			std::string name = j.first + "." + i;
			if (m_pkgrepo.checkBinaryExist(name)) {
				std::string packageName  = m_pkgrepo.dirName(name) + "/" + m_pkgrepo.fileName(name);
				packageFileName = m_pkgrepo.fileName(name);
				if ( ! checkFileExist(packageName) )
					m_pkgrepo.downloadPackageFileName(name);
				tmpList.insert(name);
			}
		}
	}
	if (tmpList.size() > 0 ) {
		for (auto i : tmpList) {
			std::pair<std::string,time_t> PackageTime;
			PackageTime.first=i;
			PackageTime.second=0;

			m_pkgrepo.generateDependencies(i);
		}
	}
}
}
