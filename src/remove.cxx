/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "remove.h"

namespace cards {

remove::remove(const std::string& commandName,
	const CardsArgumentParser& argParser,
	const std::string& configFileName)
	: pkgrm(commandName),m_argParser(argParser)
{
	if (m_argParser.isSet(CardsArgumentParser::OPT_ROOT))
		m_root = m_argParser.getOptionValue(CardsArgumentParser::OPT_ROOT)
			+ m_root;

	m_progress = true;

	cards::conf config(configFileName);

	// Retrieve info about all the packages
	buildDatabase(true);

	std::set< std::pair<std::string,std::string> > listOfPackagesToRemove;
	std::pair<std::string,std::string> PackageToRemove;

	for ( auto i : m_argParser.otherArguments() ) {
		if ( i == "base" ) {
			m_actualError = cards::ERROR_ENUM_PACKAGE_IN_USE;
			std::cout << std::endl
				<< _("The collection ")
				<< i
				<< _(" is part of the minimal system and cannot be removed !!!")
				<< std::endl
				<< std::endl;
			treatErrors(i);
		}

		for (auto j : m_listOfPackages) {
			for (auto k : j.second.dependencies() ){
				if ( i == k.first) {
					m_actualError = cards::ERROR_ENUM_PACKAGE_IN_USE;
					std::cout << std::endl << i << _(" is a runtime dependency for ") << j.first << " !!!" << std::endl << std::endl;
					treatErrors(i);
				}
			}
			if  ((j.second.collection() == i) ||
				(j.second.group() == i)) {
				PackageToRemove.first=j.first;
				PackageToRemove.second=j.second.collection();
				listOfPackagesToRemove.insert(PackageToRemove);
			}
		}
		if (listOfPackagesToRemove.empty()) {
			for (auto j : m_listOfPackages) {
				for (auto k : j.second.sets()) {
					if ( i == k ) {
						PackageToRemove.first = j.first;
						PackageToRemove.second=j.second.collection();
						listOfPackagesToRemove.insert(PackageToRemove);
					}
				}
			}
		}
		{
			// if it's an alias get the real name
			std::string a = m_listOfAlias [i];
			PackageToRemove.first = a ;
			PackageToRemove.second = getCollection(i);
			listOfPackagesToRemove.insert(PackageToRemove);
		}
	}
	std::set< std::pair<std::string,std::string> > groupSetPackagesToRemove;
	for ( auto i :  config.groups() ) {
		if ( i == "lib" )
			continue;
		for ( auto j : listOfPackagesToRemove ) {
			PackageToRemove.first = j.first  + "." + i;
			PackageToRemove.second = j.second;
			groupSetPackagesToRemove.insert(PackageToRemove);
		}
	}
	for ( auto i : groupSetPackagesToRemove ) {
		if (checkPackageNameExist(i.first))
			listOfPackagesToRemove.insert(i);
	}
	for ( auto i : listOfPackagesToRemove ) {
		if (i.first.size() == 0 )
			continue;
		m_packageName = i.first;
		if ((i.second == "base") &&
			(!m_argParser.isSet(CardsArgumentParser::OPT_FORCE))) {
				m_actualError = cards::ERROR_ENUM_PACKAGE_IN_BASESYSTEM;
				treatErrors(i.first);
		}
		run();
		std::string name = "(" +  m_packageCollection + ") ";
		name += i.first;
		syslog(LOG_INFO,"%s",name.c_str());
	}

	// Lets get read of obsolets dependencies
	getListOfManInstalledPackages ();
	bool found;
	std::set<std::string> obsoletsDeps;
	std::set<std::string> obsoletsGroups;
	for ( auto i : m_listOfPackages  ) {
		found = false;
		for (  auto j : m_listofDependencies ) {
			if ( i.first == j ) {
				found = true;
				break;
			}
		}
		if (!found)
			obsoletsDeps.insert(i.first);
	}
	for ( auto i :  config.groups() ) {
		if ( i == "lib" )
			continue;
		for ( auto j : obsoletsDeps ) {
			obsoletsGroups.insert(j+"."+i);
		}
	}
	for ( auto i : obsoletsGroups ) {
		if (checkPackageNameExist(i))
			obsoletsDeps.insert(i);
	}
	for ( auto i : obsoletsDeps ) {
		m_packageName = i;
		run();
		std::string name = "(" +  m_packageCollection + ") ";
		name += i;
		syslog(LOG_INFO,"%s",name.c_str());
	}
}
}
