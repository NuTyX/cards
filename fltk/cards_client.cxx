/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#include "cards_client.h"

namespace cards
{
    /// Constructor
    cards_client::cards_client()
        : pkgadd("")
        , m_pkgrepo("/etc/cards.conf")
    {
        m_log = cards_logger::instance();
    }

    /// Get a string list of installed packages
    std::set<std::string> cards_client::ListOfInstalledPackages()
    {
        return getSetOfPackagesNames();
    }

    /// Install a package list
    void cards_client::InstallPackages(const std::set<std::string>& pPackageList)
    {
        if (pPackageList.size() == 0)
            return;
        std::string message = _("sudo cards install");
        for (auto pack : pPackageList)
        {
            message += _(" ") + pack;
        }
        m_log->log(message);

        // Retrieve info about all the packages
        buildDatabase(true);

        m_log->log(_("Resolve package dependencies..."));
        for (auto pack : pPackageList)
        {
            m_pkgrepo.generateDependencies(pack);
        }
        for (auto i : m_pkgrepo.getDependenciesList() )
        {
            m_packageArchiveName = m_pkgrepo.dirName(i.first)
                + "/"
                + m_pkgrepo.fileName(i.first);
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

        // Retrieve info about all the packages
        buildDatabase(true);

        cards_client Cards;
        std::set<std::string> basePackagesList;

        for (auto pack:pPackageList)
        {

            if (!checkPackageNameExist(pack))
            {
                m_actualError = cards::ERROR_ENUM_PACKAGE_NOT_INSTALL;
                treatErrors(pack);
            }

            // Remove metadata about the package removed
            removePackageFilesRefsFromDB(pack);

            // Remove the files on hd
            removePackageFiles(pack);
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
				i = j / ( getNumberOfPackages() / 100);
				Value = i;
				j++;
				break;
			}
			case cards::ACTION_ENUM_PKG_INSTALL_RUN:
			{
				if (getNumberOfFiles() > 100)
				{
					i= getInstalledFilesNumber() / ( getNumberOfFiles() /100);
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
					unsigned int l = getNumberOfFiles();
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
