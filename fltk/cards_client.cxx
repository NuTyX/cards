/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#include "cards_client.h"

namespace cards
{
    /// Constructor
    cards_client::cards_client()
        : pkginst("","/etc/cards.conf")
    {
        m_root="/";
        m_log = cards_logger::instance();
    }

    /// Get a string list of installed packages
    std::set<std::string> cards_client::ListOfInstalledPackages()
    {
        getListOfPackagesNames (m_root);
        buildCompleteDatabase(true);
        std::set<std::string> ListOfInstalledPackages;
        for (auto i : m_listOfPackages)
        {
            ListOfInstalledPackages.insert(i.first);
        }
        return ListOfInstalledPackages;
    }

    /// Install a package list
    void cards_client::InstallPackages(const std::set<std::string>& pPackageList)
    {
        if (pPackageList.size() == 0) return;
        std::string message = _("sudo cards install");
        for (auto pack:pPackageList)
        {
            message += _(" ") + pack;
        }
        m_log->log(message);
        // Get the list of installed packages
        getListOfPackagesNames(m_root);

        // Retrieve info about all the packages
        buildCompleteDatabase(false);

        m_log->log(_("Resolve package dependencies..."));
        for (auto pack:pPackageList)
        {
            m_packageName = pack;
            generateDependencies();
        }
        getLocalePackagesList();
        for ( auto i : m_dependenciesList )
        {
            m_packageArchiveName = getPackageFileName(i.first);
            archive packageArchive(m_packageArchiveName.c_str());
            std::string name = packageArchive.name();
            if ( checkPackageNameExist(name ))
            {
                m_upgrade=1;
            }
            else
            {
                m_upgrade=0;
            }
            if (i.second > 0)
                setDependency();

            // We don't want to have trouble with conflict files,
            // lets flcards force the installation by default
            m_force=true;
            run();
        }
    }

    /// Remove a package list
    void cards_client::RemovePackages(const std::set<std::string>& pPackageList)
    {
        if (pPackageList.size() == 0) return;
        std::string message = _("sudo cards remove");
        for (auto pack:pPackageList)
        {
            message += _(" ") + pack;
        }
        m_log->log(message);

        cards::lock Lock(m_root, true);
        // Get the list of installed packages
        getListOfPackagesNames(m_root);
        // Retrieve info about all the packages
        buildCompleteDatabase(false);

        cards_client Cards;
        std::set<std::string> basePackagesList;
        cards::conf config(Cards.m_configFileName);

        for (auto it : config.baseDir())
        {
            findDir(basePackagesList,it);
        }

        for (auto pack:pPackageList)
        {

            if (!checkPackageNameExist(pack))
            {
                m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_INSTALL;
                treatErrors(pack);
            }
            if (basePackagesList.find(pack) != basePackagesList.end())
		continue;
            // Remove metadata about the package removed
            removePackageFilesRefsFromDB(pack);

            // Remove the files on hd
            removePackageFiles(pack);
        }
    }

    /// Fill m_dependenciesList with LocalPackageList
    void cards_client::getLocalePackagesList()
    {
        if (m_config.groups().empty()) return;
        std::set<std::string> tmpList;
        for ( auto i :  m_config.groups() )
        {
            for ( auto j :m_dependenciesList )
            {
                std::string packageName  = j.first + "." + i;
                if (checkBinaryExist(packageName))
                {
                    setPackageFileName(packageName);
                    if ( ! checkFileExist(getPackageFileName(packageName)) ) downloadPackageFileName(packageName);
                    tmpList.insert(packageName);
                }
            }
        }
        if (tmpList.size() > 0 ) {
			for (auto i : tmpList) {
               std::pair<std::string,time_t> PackageTime;
               PackageTime.first=i;
               PackageTime.second=0;
               m_dependenciesList.push_back(PackageTime);
            }
        }
    }

    void cards_client::progressInfo()
    {
			int Value=0;
			static unsigned int j = 0;
			int i;
			switch ( m_actualAction )
			{
			case cards::ACTION_ENUM_DB_OPEN_RUN:
			{
				i = j / ( getListOfPackagesNames("") / 100);
				Value = i;
				j++;
				break;
			}
			case cards::ACTION_ENUM_PKG_INSTALL_RUN:
			{
				if (getFilesNumber() > 100)
				{
					i= getInstalledFilesNumber() / ( getFilesNumber() /100);
					Value = i;
				} else {
					Value = j;
				}
				j++;
				break;
			}
			case cards::ACTION_ENUM_RM_PKG_FILES_START:
			{
				j = 0;
				break;
			}
			case cards::ACTION_ENUM_RM_PKG_FILES_RUN:
			{
					unsigned int l = getFilesList().size();
					if ( l > 100 )
					{
						i = j / ( l / 100);
						Value = i;
					} else {
						Value = j;
					}
					j++;
					break;
				}
			}
			for (cards_clientEvents* it : m_arrCallback)
			{
				it->OnProgressInfo(Value);
			}
		}

    // Add a nex suscriber to the callback event list
    void cards_client::subscribeToEvents(cards_clientEvents* pCallBack)
    {
        // Todo : Check if the new susciber is already in the container
        m_arrCallback.push_back(pCallBack);
    }

    /// Remove an event suscriber from event callback list
    void cards_client::unsubscribeFromEvents(cards_clientEvents* pCallBack)
    {
        std::vector<cards_clientEvents*>::iterator it;
        it=find(m_arrCallback.begin(),m_arrCallback.end(),pCallBack);
        if (it != m_arrCallback.end())
        {
            m_arrCallback.erase(it);
        }
    }
}
