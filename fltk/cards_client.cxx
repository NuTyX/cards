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
            run();
        }
    }

    /// Install a package list
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
        for (auto pack:pPackageList)
        {

            if (!checkPackageNameExist(pack))
            {
                m_actualError = PACKAGE_NOT_INSTALL;
                treatErrors(pack);
            }

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
        int i,n;
        n = getNumberOfPackages();
        switch ( m_actualAction )
        {
            case DB_OPEN_START:
                Value=0;
                break;
            case DB_OPEN_RUN:
                if (n >100) {
                    i = j / ( n / 100);
                    Value=i;
                }
                j++;
                break;
            case DB_OPEN_END:
                    Value=100;
                    break;
        }
        for (CClientEvents* it : _arrCallback)
        {
            it->OnProgressInfo(Value);
        }
    }

    CPackage CClient::getPackageInfo(const string& pName)
    {
        CPackage Pack;
        buildCompleteDatabase(true);
        if (checkPackageNameExist(pName))
        {
            Pack._name = m_listOfAlias[pName];
            if (m_listOfInstPackages[Pack._name].alias.size() > 0 )
            {
                for ( auto i : m_listOfInstPackages[Pack._name].alias)
                    Pack._alias +=  i + string(" ");
            }
            Pack._description = m_listOfInstPackages[Pack._name].description;
            Pack._base = m_listOfInstPackages[Pack._name].base;
            Pack._group = m_listOfInstPackages[Pack._name].group;
            Pack._collection = m_listOfInstPackages[Pack._name].collection;
            Pack._url = m_listOfInstPackages[Pack._name].url;
            Pack._contributor = m_listOfInstPackages[Pack._name].contributors;
            Pack._packager = m_listOfInstPackages[Pack._name].packager;
            Pack._version = m_listOfInstPackages[Pack._name].version;
            Pack._release = m_listOfInstPackages[Pack._name].release;
            Pack._build_date = getDateFromEpoch(m_listOfInstPackages[Pack._name].build);
            Pack._size = sizeHumanRead(m_listOfInstPackages[Pack._name].size);
            Pack._number_files = to_string(m_listOfInstPackages[Pack._name].files.size());
            Pack._arch = m_listOfInstPackages[Pack._name].arch;
            if ( m_listOfInstPackages[Pack._name].dependencies.size() > 0 )
            {
                for ( auto i : m_listOfInstPackages[Pack._name].dependencies)
                    Pack._dependencies += i.first + string(" ");
            }
        }
        return Pack;
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
