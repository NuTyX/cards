/*
 * cards_wrapper.h
 *
 * Copyright 2017 Gianni Peschiutta <artmia@nutyx.org>
 * Copyright 2019 by NuTyX team (http://nutyx.org)
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

#ifndef  CARDS_WRAPPER_H
#define  CARDS_WRAPPER_H

#include <cstddef>
#include <thread>
#include <vector>
#include <algorithm>

#include <libcards.h>
#include "console_forwarder.h"
#include "cards_client.h"
#include "cards_event_handler.h"
#include "cards_package.h"
#include "cards_log.h"

using namespace std;

namespace cards
{
    enum CW_ACTIONS
    {
        SYNC,
        UPG,
        DOJOB,
        REFRESH
    };

    /** \class Cards_wrapper
     * \brief GUI interfacing wrapper for CARDS
     *
     * This class ensure interface cards with GUI application need non-blocking operation,
     * This is a single instance (singleton) that ensure only one instance of cards library.
     *
     */
    class  CWrapper : public CClientEvents
    {
    public:
        /**
         * \brief Return or create the singleton
         *
         * This static method create the singleton if not yet done
         *
         * \return pointer of the singleton
         */
    static CWrapper*  instance();

        /**
         * \brief Kill the singleton
         *
         * if exist, delete instance of the singleton
         */
        void kill();

        /**
         * \brief Suscribe to CARDS events
         *
         * Record callback from client class which submit callback from Card lib
         */
        void subscribeToEvents(CEventHandler* pCallBack);

        /**
         * \brief Unsuscribe from CARDS Events
         *
         * Unsuscribe client class callbock form Suscribe list
         */
        void unsubscribeFromEvents(CEventHandler* pCallBack);

        /**
         * \brief list of installed packages
         *
         * Return list of installed packages
         */
        void refreshPackageList();

        /**
         * \brief Get CARDS Version
         *
         * Return CARDS Library version
         */
        string getCardsVersion();

        /**
         * \brief Sync CARDS database
         *
         * Launch CARD Sync operation
         * Callback is called when operation is done
         */
        void sync();

        void doJobList();

        const vector<CPackage*>& getPackageList();

        CPackage* getPackage(const string& pName);

        void refreshJobList();

        const vector<CPackage*>& getJobList();

        const set<string>& getSetList();

        bool isJobRunning();

        void getPackageInfo(const string& pName);

    protected:
        void OnProgressInfo(int percent);

    private:

    /**
     * \brief Constructor
     * Private constructor
     */
        CWrapper();

    /**
     * \brief Destructor
     * Private destructor
     */
        ~CWrapper();

        static	CWrapper*	_ptCWrapper; //Static pointer of the singleton

        /// Containers
        vector<CEventHandler*> _arrEventHandler; // Std array to store callback event clients
        vector<CPackage*> _arrPackages;
        vector<CPackage*> _arrJobList;
        std::set<std::string> m_arrSets;
//        set<string> _arrCollections;

        /// Threaded Tasks
        void m_Sync_Thread(); // Main Thread for Cards Sync Operation
        void m_DoJobList_Thread(); // Thread to install package
        void m_RefreshPackageList_Thread();
        void m_GetPackageInfo_Thread(string pName);

        /// Utils
        CLogger* _log;

        /// CallBack
        void m_OnSyncFinished_Callback(const CEH_RC rc); // Callback broadcast for Sync Cards operation
        void m_OnDoJobListFinished_Callback(const CEH_RC rc);
        void m_OnRefreshPackageFinished_Callback(const CEH_RC rc);
        void m_OnJobListChanged_Callback(const CEH_RC rc);

        bool _job_running; //Flag to know if a thread is currently running
        thread* _job; // Thread handler pointer

        bool m_checkRootAccess(); // Just check if we have root accessing
        bool m_IsThreadFree();
        void m_ClearPackagesList();

    };
}
#endif //  CARDS_WRAPPER_H
