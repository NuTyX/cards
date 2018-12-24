/*
 * cards_client.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "cards_client.h"

namespace cards
{
    /// Constructor
    CClient::CClient()
        : Pkginst("","/etc/cards.conf")
    {
        m_root="/";
        _log = CLogger::instance();
    }

    /// Get a string list of installed packages
    set<string> CClient::ListOfInstalledPackages()
    {
        getListOfPackageNames (m_root);
        buildCompleteDatabase(true);
        set<string> ListOfInstalledPackages;
        for (auto i : m_listOfInstPackages)
        {
            ListOfInstalledPackages.insert(i.first);
        }
        return ListOfInstalledPackages;
    }

    /// Install a package list
    void CClient::InstallPackages(const set<string>& pPackageList)
    {
        if (pPackageList.size()==0) return;
        string message = _("sudo cards install");
        for (auto pack:pPackageList)
        {
            message += _(" ") + pack;
        }
        _log->log(message);
        // Get the list of installed packages
        getListOfPackageNames(m_root);

        // Retrieve info about all the packages
        buildCompleteDatabase(false);

        _log->log(_("Resolve package dependencies..."));
        for (auto pack:pPackageList)
        {
            m_packageName = pack;
            generateDependencies();
        }
        getLocalePackagesList();
        for ( auto i : m_dependenciesList )
        {
            m_packageArchiveName = getPackageFileName(i);
            ArchiveUtils packageArchive(m_packageArchiveName.c_str());
            string name = packageArchive.name();
            if ( checkPackageNameExist(name ))
            {
                m_upgrade=1;
            }
            else
            {
                m_upgrade=0;
            }
            // We don't want to have trouble with conflict files,
            // lets flcards force the installation by default
            m_force=true;
            run();
        }
    }

    /// Remove a package list
    void CClient::RemovePackages(const set<string>& pPackageList)
    {
        if (pPackageList.size()==0) return;
        string message = _("sudo cards remove");
        for (auto pack:pPackageList)
        {
            message += _(" ") + pack;
        }
        _log->log(message);

        Db_lock lock(m_root, true);
        // Get the list of installed packages
        getListOfPackageNames(m_root);
        // Retrieve info about all the packages
        buildCompleteDatabase(false);

        CClient Cards;
        set<string> basePackagesList;
        Config config;
        Pkgrepo::parseConfig(Cards.m_configFileName.c_str(),config);
        for (auto it : config.baseDir)
        {
            findFile(basePackagesList,it);
        }

        for (auto pack:pPackageList)
        {

            if (!checkPackageNameExist(pack))
            {
                m_actualError = PACKAGE_NOT_INSTALL;
                treatErrors(pack);
            }
            if (basePackagesList.find(pack)!=basePackagesList.end())
		continue;
            // Remove metadata about the package removed
            removePackageFilesRefsFromDB(pack);

            // Remove the files on hd
            removePackageFiles(pack);
        }
    }

    /// Fill m_dependenciesList with LocalPackageList
    void CClient::getLocalePackagesList()
    {
        if (m_config.locale.empty()) return;
        std::set<std::string> tmpList;
        for ( auto i :  m_config.locale )
        {
            for ( auto j :m_dependenciesList )
            {
                std::string packageName  = j + "." + i;
                if (checkBinaryExist(packageName))
                {
                    m_packageFileName = getPackageFileName(packageName);
                    if ( ! checkFileExist(m_packageFileName) ) downloadPackageFileName(packageName);
                    tmpList.insert(packageName);
                }
            }
        }
        if (tmpList.size() > 0 )
            for (auto i : tmpList) m_dependenciesList.push_back(i);
    }

    void CClient::progressInfo()
    {
        int Value=0;
        static int j = 0;
        int i;
        switch ( m_actualAction )
        {
			case DB_OPEN_RUN:
			{
				i = j / ( getNumberOfPackages() / 100);
				Value = i;
				j++;
				break;
			}
            case PKG_INSTALL_RUN:
            {
                if (getFilesNumber() > 100)
                {
                    i= getInstalledFilesNumber() / ( getFilesNumber() /100);
                }
                Value = i;
                j++;
                break;
            }
        }
        for (CClientEvents* it : _arrCallback)
        {
            it->OnProgressInfo(Value);
        }
    }

    // Add a nex suscriber to the callback event list
    void CClient::subscribeToEvents(CClientEvents* pCallBack)
    {
        // Todo : Check if the new susciber is already in the container
        _arrCallback.push_back(pCallBack);
    }

    /// Remove an event suscriber from event callback list
    void CClient::unsubscribeFromEvents(CClientEvents* pCallBack)
    {
        vector<CClientEvents*>::iterator it;
        it=find(_arrCallback.begin(),_arrCallback.end(),pCallBack);
        if (it!=_arrCallback.end())
        {
            _arrCallback.erase(it);
        }
    }
}
