/*
 * cards_wrapper.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artmia@nutyx.org>
 * Copyright 2017 - 2023 Thierry Nuttens <tnut@nutyx.org>
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

#include "cards_wrapper.h"


namespace cards
{
    ///
    /// Singleton Management
    ///

    /** Init static pointer to null */
    cards_wrapper* cards_wrapper::m_ptcards_wrapper = nullptr;

    /** Constructor */
    cards_wrapper::cards_wrapper()
    {
        m_job_running = false;
        m_job=nullptr;
        m_log=cards_logger::instance();;
    }

    /** Destructor */
    cards_wrapper::~cards_wrapper()
    {
        m_ClearPackagesList();
    }

    /** Return or create singleton instance */
    cards_wrapper* cards_wrapper::instance()
    {
        if (m_ptcards_wrapper==nullptr)
            m_ptcards_wrapper=new cards_wrapper();
        return m_ptcards_wrapper;
    }

    /** Destruction of the singleton , only if instance exist */
    void cards_wrapper::kill()
    {
        if (m_ptcards_wrapper!=nullptr)
            delete m_ptcards_wrapper;
    }

    // Add a nex suscriber to the callback event list
    void cards_wrapper::subscribeToEvents(cards_event_handler* pCallBack)
    {
        // Todo : Check if the new susciber is already in the container
        m_arrEventHandler.push_back(pCallBack);
    }

    /// Remove an event suscriber from event callback list
    void cards_wrapper::unsubscribeFromEvents(cards_event_handler* pCallBack)
    {
        std::vector<cards_event_handler*>::iterator it;
        it=find(m_arrEventHandler.begin(),m_arrEventHandler.end(),pCallBack);
        if (it!=m_arrEventHandler.end())
        {
            m_arrEventHandler.erase(it);
        }
    }

    const std::vector<cards::cache*>& cards_wrapper::getPackageList()
    {
        return m_arrPackages;
    }

    const std::set<std::string>& cards_wrapper::getSetList()
    {
        return m_arrSets;
    }

    ///
    ///
    ///
    cards::cache* cards_wrapper::getPackage(const std::string& pName)
    {
        cards::cache* ptr=nullptr;
        for (cards::cache* it : m_arrPackages)
        {
            if (it->name() == pName)
            {
                ptr=it;
                break;
            }
        }
        return ptr;
    }

    /// Update the job list

    void cards_wrapper::refreshJobList()
    {
        m_arrJobList.clear();
        for (cards::cache* it:m_arrPackages)
        {
            if (it->toinstall() || it->toremove())
            {
                m_arrJobList.push_back(it);
#ifndef NDEBUG
                std::cerr << "Add Job to list for package "
                    << it->name()
                    << " Size of job list="
                    << m_arrJobList.size()
                    << std::endl;
#endif
            }
        }
        m_OnJobListChanged_Callback(OK);
    }
    ///
    /// \brief cards_wrapper::clearJobList
    ///
    void cards_wrapper::clearJobList()
    {
        if (m_job_running)
            return;
        for (cards::cache* it:m_arrPackages)
        {
            if (it->status() == cards::STATUS_ENUM_TO_INSTALL)
            {
                it->unsetStatus(cards::STATUS_ENUM_TO_INSTALL);
            }
            if (it->status() == cards::STATUS_ENUM_TO_REMOVE)
            {
                it->unsetStatus(cards::STATUS_ENUM_TO_REMOVE);
            }
        }
        refreshJobList();
    }

    /** Return the current job list */
    const std::vector<cards::cache*>& cards_wrapper::getJobList()
    {
        return m_arrJobList;
    }

    ///
    /// Thread Callers
    ///

    /** Create a new thread for Cards Sync operation*/
    void cards_wrapper::sync()
    {
        if (m_IsThreadFree())
        {
            m_job = new std::thread(&cards_wrapper::m_Sync_Thread, cards_wrapper::m_ptcards_wrapper);
            m_job->detach();
//            m_log->log(_("Synchronization started ..."));
        }
    }

    /** Create a new thread for Cards Sync operation*/
    void cards_wrapper::doJobList()
    {
        if (m_IsThreadFree())
        {
            m_job = new std::thread(&cards_wrapper::m_DoJobList_Thread, cards_wrapper::m_ptcards_wrapper);
            m_job->detach();
        }
    }

    /** Create a new thread for Cards Sync operation*/
    void cards_wrapper::refreshPackageList()
    {
        if (m_IsThreadFree())
        {
            m_job = new std::thread(&cards_wrapper::m_RefreshPackageList_Thread, cards_wrapper::m_ptcards_wrapper);
            m_job->detach();
        }
    }

    void cards_wrapper::getPackageInfo(const std::string& pName)
    {
        if (m_IsThreadFree())
        {
            m_job = new std::thread(&cards_wrapper::m_GetPackageInfo_Thread, cards_wrapper::m_ptcards_wrapper,pName);
            m_job->detach();
        }
    }

    void cards_wrapper::m_GetPackageInfo_Thread(std::string pName)
    {
        m_job_running =true;
        cards::cache Package;
        cards::cache* Pack = getPackage(pName);
        if (Pack != nullptr) Package = *Pack;
        for (auto it : m_arrEventHandler)
        {
            it->OnPackageInfo(Package);
        }
        m_job_running =false;
    }

    ///
    /// Threaded Tasks
    ///

    /** Launch a Cards Sync operation*/
    void cards_wrapper::m_Sync_Thread()
    {
        m_job_running = true;
        CEH_RC rc=CEH_RC::OK;
        if (m_checkRootAccess())
        {
            try
            {
                pkgsync Sync;
                Sync.run();
            }
            catch (std::exception& e)
            {
                m_log->log(_("Exception occured during Sync thread : ") + std::string(e.what()), LEVEL_ERROR);
            }
        }
        else
        {
            rc= CEH_RC::NO_ROOT;
        }
        m_log->log(_("Synchronization finished"));
        m_job_running = false;

        m_OnSyncFinished_Callback(rc);
    }

    /** Launch a Cards Sync operation*/
    void cards_wrapper::m_DoJobList_Thread()
    {
        m_job_running =true;
        CEH_RC rc=CEH_RC::OK;
        if (m_checkRootAccess())
        {
            try
            {
                m_log->log(_("Determine Packages Install and Remove List..."));
                std::set<std::string> Removelist;
                std::set<std::string> InstallList;
                for (cards::cache* it:m_arrJobList)
                {
                    if (it->toinstall()) InstallList.insert(it->name());
                    if (it->toremove()) Removelist.insert(it->name());
                }
                m_log->log(_("Ok, ") + std::to_string(Removelist.size()) + _(" Package(s) will be removed and ") +
                    std::to_string(InstallList.size()) + _(" Package(s) will be installed... "));
                if (Removelist.size() > 0)
                {
                    cards_client Cards;
                    Cards.subscribeToEvents(this);
                    Cards.RemovePackages(Removelist);
                }
                if (InstallList.size() > 0)
                {
                    cards_client Cards;
                    Cards.subscribeToEvents(this);
                    Cards.InstallPackages(InstallList);
                }
                rc= CEH_RC::OK;
            }
            catch (std::exception& e)
            {
                m_log->log("Exception occured during processing job list Thread : " + std::string(e.what()), LEVEL_ERROR);
                rc= CEH_RC::EXCEPTION;
            }
            m_RefreshPackageList_Thread();
        }
        else
        {
            rc= CEH_RC::NO_ROOT;
        }
        m_arrJobList.clear();
        refreshJobList();
        m_log->log("Job list is finished");
        m_job_running = false;
        m_OnDoJobListFinished_Callback(rc);
    }


    /** Threaded task to refresh the package image container*/
    void cards_wrapper::m_RefreshPackageList_Thread()
    {
        m_job_running =true;
        cards_client Cards;
        // First pass get all package available
        m_ClearPackagesList();
        std::set<cards::cache*> AvailablePackages = Cards.getBinaryPackageSet();
        std::set<std::string> InstalledPackages = Cards.ListOfInstalledPackages();
        for (auto i : AvailablePackages)
        {
           cards::cache* Pack = new cards::cache();
           Pack->collection(i->collection());
           Pack->name(i->name());
           Pack->description(i->description());
           Pack->version(i->version());
           Pack->packager(i->packager());
           if (InstalledPackages.find(Pack->name()) != InstalledPackages.end())
           Pack->setStatus(STATUS_ENUM_INSTALLED);
           m_arrPackages.push_back(Pack);
           if (m_arrSets.find(Pack->collection()) == m_arrSets.end())
             m_arrSets.insert(Pack->collection());
        }
        m_OnRefreshPackageFinished_Callback(CEH_RC::OK);
        m_job_running =false;
    }


    ///
    /// Callbacks events
    ///

    /** Broadcast to all suscribers the Sync Finished callback*/
    void cards_wrapper::m_OnSyncFinished_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        if (rc==OK) refreshPackageList();
        for (auto* it : m_arrEventHandler)
        {
            it->OnSyncFinished(rc);
        }
    }

    /** Broadcast to all suscribers the Install Finished callback*/
    void cards_wrapper::m_OnDoJobListFinished_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : m_arrEventHandler)
        {
            it->OnDoJobListFinished(rc);
        }
    }


    /** Broadcast the end of the thread to get list of packages to all event suscribers*/
    void cards_wrapper::m_OnRefreshPackageFinished_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : m_arrEventHandler)
        {
            it->OnRefreshPackageFinished(rc);
        }
    }

    /** Broadcast the end of the thread ot get list of package to all event suscribers*/
    void cards_wrapper::m_OnJobListChanged_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : m_arrEventHandler)
        {
            it->OnJobListChange(rc);
        }
    }

    ///
    /// Misc
    ///

    /** Get and print the libcards Version*/

    std::string cards_wrapper::getCardsVersion()
    {
        return std::string(VERSION);
    }

    /** Check if the application is curently running as root
      * If it is the case, it return true.*/
    bool cards_wrapper::m_checkRootAccess()
    {
        if (getuid() != 0)
        {
            return false;
        }
        return true;
    }

    /** Ensure the thread instance is free and ready to launch a new task */
    bool cards_wrapper::m_IsThreadFree()
    {
        if (!m_job_running)
        {
            if (m_job != nullptr)
            {
                delete m_job;
                m_job=nullptr;
            }
            if (m_job==nullptr) return true;
        }
        return false;
    }

    void cards_wrapper::m_ClearPackagesList()
    {
        for (auto it : m_arrPackages)
        {
            if (it!=nullptr)
            {
                delete it;
            }
        }
        m_arrPackages.clear();
    }

    bool cards_wrapper::isJobRunning()
    {
        return m_job_running;
    }

    /** Broadcast progress info event */
    void cards_wrapper::OnProgressInfo(int percent)
    {
        for (auto* it : m_arrEventHandler)
        {
            it->OnProgressInfo(percent);
        }
    }
}
