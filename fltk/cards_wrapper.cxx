/*
 * cards_wrapper.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artmia@nutyx.org>
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
#include <sstream>

#include "version.h"

namespace cards
{
    ///
    /// Singleton Management
    ///

    /** Init static pointer to null */
    CWrapper* CWrapper::_ptCWrapper = nullptr;

    /** Constructor */
    CWrapper::CWrapper()
    {
        _job_running = false;
        _job=nullptr;
        _log=CLogger::instance();
    }

    /** Destructor */
    CWrapper::~CWrapper()
    {
        m_ClearPackagesList();
    }

    /** Return or create singleton instance */
    CWrapper* CWrapper::instance()
    {
        if (_ptCWrapper==nullptr)
            _ptCWrapper=new CWrapper();
        return _ptCWrapper;
    }

    /** Destruction of the singleton , only if instance exist */
    void CWrapper::kill()
    {
        if (_ptCWrapper!=nullptr)
            delete _ptCWrapper;
    }

    // Add a nex suscriber to the callback event list
    void CWrapper::subscribeToEvents(CEventHandler* pCallBack)
    {
        // Todo : Check if the new susciber is already in the container
        _arrEventHandler.push_back(pCallBack);
    }

    /// Remove an event suscriber from event callback list
    void CWrapper::unsubscribeFromEvents(CEventHandler* pCallBack)
    {
        vector<CEventHandler*>::iterator it;
        it=find(_arrEventHandler.begin(),_arrEventHandler.end(),pCallBack);
        if (it!=_arrEventHandler.end())
        {
            _arrEventHandler.erase(it);
        }
    }

    const vector<CPackage*>& CWrapper::getPackageList()
    {
        return _arrPackages;
    }

    CPackage* CWrapper::getPackage(const string& pName)
    {
        CPackage* ptr=nullptr;
        for (CPackage* it : _arrPackages)
        {
            if (it->getName()==pName)
            {
                ptr=it;
                break;
            }
        }
        return ptr;
    }

    /// Update the job list

    void CWrapper::refreshJobList()
    {
        _arrJobList.clear();
        for (CPackage* it:_arrPackages)
        {
            if (it->isToBeInstalled() || it->isToBeRemoved())
            {
                _arrJobList.push_back(it);
    #ifndef NDEBUG
                cerr << "Add Job to list for package " << it->getName() << " Size of job list=" << _arrJobList.size() << endl;
    #endif // _NDEBUG
            }
        }
        m_OnJobListChanged_Callback(OK);
    }

    const vector<CPackage*>& CWrapper::getJobList()
    {
        return _arrJobList;
    }

    ///
    /// Thread Callers
    ///

    /** Create a new thread for Cards Sync operation*/
    void CWrapper::sync()
    {
        if (m_IsThreadFree()) _job = new thread(&CWrapper::m_Sync_Thread, CWrapper::_ptCWrapper);
        _job->detach();
    }

    /** Create a new thread for Cards Sync operation*/
    void CWrapper::doJobList()
    {
        if (m_IsThreadFree()) _job = new thread(&CWrapper::m_DoJobList_Thread, CWrapper::_ptCWrapper);
        _job->detach();
    }

    /** Create a new thread for Cards Sync operation*/
    void CWrapper::refreshPackageList()
    {
        if (m_IsThreadFree()) _job = new thread(&CWrapper::m_RefreshPackageList_Thread, CWrapper::_ptCWrapper);
        _job->detach();
    }

    ///
    /// Threaded Tasks
    ///

    /** Launch a Cards Sync operation*/
    void CWrapper::m_Sync_Thread()
    {
        _job_running =true;
        CEH_RC rc=CEH_RC::OK;
        if (m_checkRootAccess())
        {
            try
            {
                Pkgsync Sync;
                Sync.run();
            }
            catch (exception& e)
            {
                _log->log(_("Exception occured during Sync thread : ") + string(e.what()), LEVEL_ERROR);
            }
        }
        else
        {
            rc= CEH_RC::NO_ROOT;
        }
        _job_running = false;
        m_OnSyncFinished_Callback(rc);
    }

    /** Launch a Cards Sync operation*/
    void CWrapper::m_DoJobList_Thread()
    {
        _job_running =true;
        CEH_RC rc=CEH_RC::OK;
        if (m_checkRootAccess())
        {
            CClient Cards;
            try
            {
                _log->log(_("Determine Packages Install and Remove List..."));
                set<string> Removelist;
                set<string> InstallList;
                for (CPackage* it:_arrJobList)
                {
                    if (it->isToBeInstalled()) InstallList.insert(it->getName());
                    if (it->isToBeRemoved()) Removelist.insert(it->getName());
                }
                _log->log(_("Ok, ") + to_string(Removelist.size()) + _(" Package(s) will be removed and ") +
                    to_string(InstallList.size()) + _(" Package(s) will be installed... "));
                if (Removelist.size() > 0)
                {
                    CClient Cards;
                    Cards.RemovePackages(Removelist);
                }
                if (InstallList.size() > 0)
                {
                    CClient Cards;
                    Cards.InstallPackages(InstallList);
                }
                rc= CEH_RC::OK;
            }
            catch (exception& e)
            {
                _log->log("Exception occured during processing job list Thread : " + string(e.what()), LEVEL_ERROR);
                rc= CEH_RC::EXCEPTION;
            }
            m_RefreshPackageList_Thread();
        }
        else
        {
            rc= CEH_RC::NO_ROOT;
        }
        _arrJobList.clear();
        refreshJobList();
        _job_running = false;
        m_OnDoJobListFinished_Callback(rc);
    }


    /** Threaded task to refresh the package image container*/
    void CWrapper::m_RefreshPackageList_Thread()
    {
        _job_running =true;
        CClient Cards;
        // First pass get all package available
        m_ClearPackagesList();
        set<string> AvailablePackages = Cards.getBinaryPackageList();
        set<string> InstalledPackages = Cards.ListOfInstalledPackages();
        for (auto it : AvailablePackages)
        {
            CPackage* Pack=new CPackage();
            string token;
            istringstream tokenStream(it);
            int i=0;
            while (getline(tokenStream, token, '\t'))
            {
                switch (i)
                {
                    case 0: //Base
                    {
                        Pack->_collection = token;
                        break;
                    }
                    case 1: //Name
                    {
                        Pack->_name = token;
                        break;
                    }
                    case 2: //Version
                    {
                        Pack->_version = token;
                        break;
                    }
                    case 3: //Packager
                    {
                        Pack->_packager = token;
                        break;
                    }
                    case 4: //Description
                    {
                        Pack->_description = token;
                        break;
                    }
                    default:
                        break;
                }
                i++;
            }
            if (InstalledPackages.find(Pack->_name)!=InstalledPackages.end()) Pack->setStatus(INSTALLED);
            _arrPackages.push_back(Pack);
        }
        m_OnRefreshPackageFinished_Callback(CEH_RC::OK);
        _job_running =false;
    }


    ///
    /// Callbacks events
    ///

    /** Broadcast to all suscribers the Sync Finished callback*/
    void CWrapper::m_OnSyncFinished_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : _arrEventHandler)
        {
            it->OnSyncFinished(rc);
        }
    }

    /** Broadcast to all suscribers the Install Finished callback*/
    void CWrapper::m_OnDoJobListFinished_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : _arrEventHandler)
        {
            it->OnDoJobListFinished(rc);
        }
    }


    /** Broadcast the end of the thread to get list of packages to all event suscribers*/
    void CWrapper::m_OnRefreshPackageFinished_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : _arrEventHandler)
        {
            it->OnRefreshPackageFinished(rc);
        }
    }

    /** Broadcast the end of the thread ot get list of package to all event suscribers*/
    void CWrapper::m_OnJobListChanged_Callback(const CEH_RC rc=CEH_RC::OK)
    {
        for (auto* it : _arrEventHandler)
        {
            it->OnJobListChange(rc);
        }
    }

    ///
    /// Misc
    ///

    /** Get and print the libcards Version*/

    string CWrapper::getCardsVersion()
    {
        return string(APP_NAME_VERSION_STR);
    }

    /** Check if the application is curently running as root
      * If it is the case, it return true.*/
    bool CWrapper::m_checkRootAccess()
    {
        if (getuid() != 0)
        {
            return false;
        }
        return true;
    }

    /** Ensure the thread instance is free and ready to launch a new task */
    bool CWrapper::m_IsThreadFree()
    {
        if (!_job_running)
        {
            if (_job != nullptr)
            {
                delete _job;
                _job=nullptr;
            }
            if (_job==nullptr) return true;
        }
        return false;
    }

    void CWrapper::m_ClearPackagesList()
    {
        for (auto it : _arrPackages)
        {
            if (it!=nullptr)
            {
                delete it;
            }
        }
        _arrPackages.clear();
    }

    bool CWrapper::IsJobRunning()
    {
        return _job_running;
    }

    /** Broadcast progress info event */
    void CWrapper::OnProgressInfo(int percent)
    {
        for (auto* it : _arrEventHandler)
        {
            it->OnProgressInfo(percent);
        }
    }
}
