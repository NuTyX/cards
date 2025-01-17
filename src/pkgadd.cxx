/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2000 - 2005 Per Liden
//  Copyright (c) 2006 - 2013 by CRUX team (https://crux.nu)
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "pkgadd.h"

pkgadd::pkgadd()
	: pkgdbh("pkgadd"),
	m_runPrePost(true),
	m_upgrade(false),
	m_force(false)
{
}
pkgadd::pkgadd(const std::string& commandName)
	: pkgdbh(commandName),
	m_runPrePost(true),
	m_upgrade(false),
	m_force(false)
{
}
void pkgadd::parseArguments(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		std::string option(argv[i]);
		if (option == "-r" || option == "--root") {
			assertArgument(argv, argc, i);
			m_root = argv[i + 1] + m_root;
			i++;
		} else if (option == "-i" || option == "--ignore") {
			m_runPrePost = false;
		} else if (option == "-u" || option == "--upgrade") {
			m_upgrade = true;
		} else if (option == "-f" || option == "--force") {
			m_force = true;
		} else if (option == "-p" || option == "--progress") {
			m_progress = true;
		} else if (option[0] == '-' || !m_packageArchiveName.empty()) {
			m_actualError = cards::ERROR_ENUM_INVALID_OPTION;
			treatErrors(option);
		} else {
			m_packageArchiveName = option;
		}
	}
	if (m_packageArchiveName.empty()) {
		m_actualError = cards::ERROR_ENUM_OPTION_MISSING;
		treatErrors("");
	}

	// Check UID
	if (getuid())
	{
		m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}

}
void pkgadd::preRun()
{
	// Check UID
	if (getuid())
	{
		m_actualError = cards::ERROR_ENUM_ONLY_ROOT_CAN_INSTALL_UPGRADE_REMOVE;
		treatErrors("");
	}
	extractAndRunPREfromPackage(m_packageArchiveName);
}
void pkgadd::run()
{
	// Retrieving info about all the packages
	buildDatabase(true);

	// Reading the archiving to find a list of files
	std::pair<std::string, cards::db> package = openArchivePackage(m_packageArchiveName);

	readRulesFile();

	bool installed = checkPackageNameExist(package.first);
	if (installed && !m_upgrade)
	{
		m_actualError = cards::ERROR_ENUM_PACKAGE_ALLREADY_INSTALL;
		treatErrors (package.first);
	}
	else if (!installed && m_upgrade)
	{
		m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_PREVIOUSLY_INSTALL;
		treatErrors(package.first);
	}

	std::set<std::string> non_install_files = applyInstallRules(package.first,
		package.second, m_actionRules);

	if (m_runPrePost)
		preRun();

	std::set<std::string> conflicting_files = getConflictsFilesList(package.first,
		package.second);
	if (!conflicting_files.empty()) {
		if (m_force) {
			cards::lock Lock(m_root, true);
			std::set<std::string> keep_list;
			// Don't remove files matching the rules in configuration
			if (m_upgrade)
				keep_list = getKeepFileList(conflicting_files, m_actionRules);
			// Remove unwanted conflicts
			removePackageFilesRefsFromDB(conflicting_files, keep_list);
		} else {
			for (auto f : conflicting_files)
				std::cerr << f << std::endl;
/*			copy(conflicting_files.begin(), conflicting_files.end(),
				std::ostream_iterator<std::string>(std::cerr, "\n")); */
			m_actualError = cards::ERROR_ENUM_LISTED_FILES_ALLREADY_INSTALLED;
			treatErrors(package.first);
		}
	}

	std::set<std::string> keep_list;
	if (m_upgrade) {
		if (checkDependency(package.first))
			setDependency();

		cards::lock Lock(m_root, true);
		// Remove metadata about the package removed
		removePackageFilesRefsFromDB(package.first);
		keep_list = getKeepFileList(package.second.files, m_actionRules);
		removePackageFiles(package.first, keep_list);
	}
	{
		cards::lock Lock(m_root, true);
		// Installation progressInfo of the files on the HD
		installArchivePackage(m_packageArchiveName,
			keep_list, non_install_files);

		// Add the metadata about the package to the DB
		moveMetaFilesPackage(package.first,package.second);

		// Add the info about the files to the DB
		addPackageFilesRefsToDB(package.first, package.second);
	}
	postRun();
}
void pkgadd::postRun()
{
	if (checkFileExist(PKG_PRE_INSTALL))
		removeFile(m_root,PKG_PRE_INSTALL);
	if (checkFileExist(PKG_POST_INSTALL))
	{
		if ( ! m_runPrePost) {
			removeFile(m_root,PKG_POST_INSTALL);
			return;
		}
		progressInfo(cards::ACTION_ENUM_PKG_POSTINSTALL_START);
		process postinstall(SHELL,PKG_POST_INSTALL, 0 );
		if (postinstall.executeShell()) {
			std::cerr << _("WARNING Run post-install FAILED, continue") << std::endl;
		}
		progressInfo(cards::ACTION_ENUM_PKG_POSTINSTALL_END);
		removeFile(m_root,PKG_POST_INSTALL);
	}
}
void pkgadd::printHelp() const
{
	std::cout << HELP_USAGE << m_utilName << " [options] <file>" << std::endl
	<< HELP_OPTIONS << std::endl
	<< "  -i, --ignore        "
	<< _("do not execute pre/post install scripts")
	<< std::endl
	<< "  -u, --upgrade       "
	<< _("upgrade package with the same name")
	<< std::endl
	<< "  -f, --force         "
	<< _("force install, overwrite conflicting files")
	<< std::endl
	<< "  -p, --progress      "
	<< _("shows progress info")
	<< std::endl
	<< "  -r, --root <path>   "
	<< _("specify alternative installation root")
	<< std::endl
	<< "  -v, --version       "
	<< _("print version and exit")
	<< std::endl
	<< "  -h, --help          "
	<< _("print help and exit")
	<< std::endl;
}

std::set<std::string>
pkgadd::getKeepFileList(const std::set< std::string>& files, const std::vector<rule_t>& rules)
{
	std::set<std::string> keep_list;
	std::vector<rule_t> found;

	getInstallRulesList(rules, cards::UPGRADE, found);

	for (auto i : files) {
		for (std::vector<rule_t>::reverse_iterator j = found.rbegin();
		j != found.rend();
		j++) {
			if (checkRuleAppliesToFile(*j, i)) {
				if (!(*j).action)
					keep_list.insert(keep_list.end(), i);

				break;
			}
		}
	}

	return keep_list;
}

std::set<std::string>
pkgadd::applyInstallRules(const std::string& name, cards::db& info,
		const std::vector<rule_t>& rules)
{
	// TODO: better algo(?)
	std::set<std::string> install_set;
	std::set<std::string> non_install_set;

	std::vector<rule_t> files_found;
	getInstallRulesList(rules, cards::INSTALL, files_found);

	std::vector<rule_t> scripts_found;
	getPostInstallRulesList(rules, scripts_found);

	for (auto i : info.files) {

		bool install_file = true;
		for (std::vector<rule_t>::reverse_iterator j = files_found.rbegin();
		j != files_found.rend();
		j++) {
			if (checkRuleAppliesToFile(*j, i)) {
				install_file = (*j).action;
				break;
			}
		}
		if (install_file)
			install_set.insert(install_set.end(), i);
		else
			non_install_set.insert(i);

		std::pair <std::string, int> a;
		for (std::vector<rule_t>::reverse_iterator j = scripts_found.rbegin();
		j != scripts_found.rend();
		j++) {
			a.first=i;
			a.second=(*j).event;

			if (checkRuleAppliesToFile((*j), i)) {
				install_file = (*j).action;
				break;
				}
		}
		if (install_file) {
			if ( a.second == cards::LDCONF ) {
				a.first="";
			} else {
				a.first=i;
			}
			m_postInstallList.insert(a);
		}
	}
	scripts_found.clear();
	info.files.clear();
	info.files = install_set;

#ifdef DEBUG
	std::cerr << "PostInstall set:" << std::endl;
	for (auto i : m_postInstallList)
		std::cerr << "  "
			<< i.first
			<< " "
			<< i.second
			<< std::endl;
	std::cerr << "Install set:" << std::endl;
	for  (auto i : info.files)
		std::cerr << "   " << i << std::endl;
	std::cerr << std::endl;
	std::cerr << "Non-Install set:" << std::endl;
	for (auto i : non_install_set)
		std::cerr << "   " << i << std::endl;
	std::cerr << std::endl;
#endif

	return non_install_set;
}
void
pkgadd::applyPostInstallRules(const std::string& name, cards::db& info,
			const std::vector<rule_t>& rules)
{
	std::vector<rule_t> found;

	getPostInstallRulesList(rules, found);

	for (auto i : info.files) {
		bool install_file = true;
		for (std::vector<rule_t>::reverse_iterator j = found.rbegin();
		j != found.rend();
		j++) {
			std::pair <std::string, int> a;
			a.first=(*j).event;
			a.second=(*j).action;

			if (checkRuleAppliesToFile((*j), i)) {
				install_file = (*j).action;
				if (install_file) {
					if ( a.second == cards::LDCONF ) {
						a.first="";
					} else {
						a.first=i;
					}
					m_postInstallList.insert(a);
				}
				break;
			}
		}
	}
}
void
pkgadd::getInstallRulesList(const std::vector<rule_t>& rules, cards::rule_event_t event,
	std::vector<rule_t>& found) const
{
	for (auto i : rules ) {
		if (i.event == event)
			found.push_back(i);
	}
}
void
pkgadd::getPostInstallRulesList(const std::vector<rule_t>& rules,
	std::vector<rule_t>& found) const
{
	for (auto i : rules ) {
		if ( (i.event != cards::INSTALL) && (i.event != cards::UPGRADE))
			found.push_back(i);
	}
}
