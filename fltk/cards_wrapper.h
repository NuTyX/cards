/*
 * cards_wrapper.h
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

#ifndef  CARDS_WRAPPER_H
#define  CARDS_WRAPPER_H

#include "cards_client.h"
#include "cards_event_handler.h"
#include "cards_log.h"

#include <libcards.h>

#include <thread>
#include <algorithm>


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
        std::string getCardsVersion();

        /**
         * \brief Sync CARDS database
         *
         * Launch CARD Sync operation
         * Callback is called when operation is done
         */
        void sync();

        void doJobList();

        const std::vector<cards::Cache*>& getPackageList();

        cards::Cache* getPackage(const std::string& pName);

        void refreshJobList();

        void clearJobList();

        const std::vector<cards::Cache*>& getJobList();

        const std::set<std::string>& getSetList();

        bool isJobRunning();

        void getPackageInfo(const std::string& pName);

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

        static	CWrapper*	m_ptCWrapper; //Static pointer of the singleton

        /// Containers
        std::vector<CEventHandler*> m_arrEventHandler; // Std array to store callback event clients
        std::vector<cards::Cache*> m_arrPackages;
        std::vector<cards::Cache*> m_arrJobList;
        std::set<std::string> m_arrSets;

        /// Threaded Tasks
        void m_Sync_Thread(); // Main Thread for Cards Sync Operation
        void m_DoJobList_Thread(); // Thread to install package
        void m_RefreshPackageList_Thread();
        void m_GetPackageInfo_Thread(std::string pName);

        /// Utils
        CLogger* m_log;

        /// CallBack
        void m_OnSyncFinished_Callback(const CEH_RC rc); // Callback broadcast for Sync Cards operation
        void m_OnDoJobListFinished_Callback(const CEH_RC rc);
        void m_OnRefreshPackageFinished_Callback(const CEH_RC rc);
        void m_OnJobListChanged_Callback(const CEH_RC rc);

        bool m_job_running; //Flag to know if a thread is currently running
        std::thread* m_job; // Thread handler pointer

        bool m_checkRootAccess(); // Just check if we have root accessing
        bool m_IsThreadFree();
        void m_ClearPackagesList();

    };
}
#endif //  CARDS_WRAPPER_H
