/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

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

    /** \class cards_wrapper
     * \brief GUI interfacing wrapper for CARDS
     *
     * This class ensure interface cards with GUI application need non-blocking operation,
     * This is a single instance (singleton) that ensure only one instance of cards library.
     *
     */
    class  cards_wrapper : public cards_clientEvents
    {
    public:
        /**
         * \brief Return or create the singleton
         *
         * This static method create the singleton if not yet done
         *
         * \return pointer of the singleton
         */
    static cards_wrapper*  instance();

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
        void subscribeToEvents(cards_event_handler* pCallBack);

        /**
         * \brief Unsuscribe from CARDS Events
         *
         * Unsuscribe client class callbock form Suscribe list
         */
        void unsubscribeFromEvents(cards_event_handler* pCallBack);

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

        const std::vector<cards::cache*>& getPackageList();

        cards::cache* getPackage(const std::string& pName);

        void refreshJobList();

        void clearJobList();

        const std::vector<cards::cache*>& getJobList();

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
        cards_wrapper();

    /**
     * \brief Destructor
     * Private destructor
     */
        ~cards_wrapper();

        static	cards_wrapper*	m_ptcards_wrapper; //Static pointer of the singleton

        /// Containers
        std::vector<cards_event_handler*> m_arrEventHandler; // Std array to store callback event clients
        std::vector<cards::cache*> m_arrPackages;
        std::vector<cards::cache*> m_arrJobList;
        std::set<std::string> m_arrSets;

        /// Threaded Tasks
        void m_Sync_Thread(); // Main Thread for Cards Sync Operation
        void m_DoJobList_Thread(); // Thread to install package
        void m_RefreshPackageList_Thread();
        void m_GetPackageInfo_Thread(std::string pName);

        /// Utils
        cards_logger* m_log;

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
