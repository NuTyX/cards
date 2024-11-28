/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

#include "cards_log.h"

#include <libcards.h>

namespace cards
{
    // Define cards_wrapper singleton for friendship
    class cards_wrapper;
    class cards_client;

    // Abstract Class to enable events callbacks
    class cards_clientEvents
    {
        friend cards_client;
    protected:
        virtual void OnProgressInfo(int percent){}
    };

    /** \class cards_client
     * \brief Interface Class to access Cards Library Method through inheritence
     *
     * This class ensure interface cards with GUI application need non-blocking operation,
     * This is a single instance (singleton) that ensure only one instance of cards library.
     *
     */
    class cards_client : public pkgadd
    {
        // Only cards_wrapper can own this class
        friend cards_wrapper;

        pkgrepo         m_pkgrepo;
        cards_logger*   m_log;
        std::vector<cards_clientEvents*> m_arrCallback;

    protected:
        /**
         * \brief Constructor
         *
         * Constructor of cards_client class
         */
        cards_client ();

        /**
         * \brief Destructor
         *
         * Destructor of cards_client class
         *
         * \return pointer of the singleton
         */
        virtual ~cards_client (){};

        /**
         * \brief Get list of installed package
         *
         * Return string array content installed package
         *
         * \return string array contain installed package list
         */
        std::set<std::string> ListOfInstalledPackages();

        /**
         * \brief Install a package list
         *
         * Launch Cards Install process
         */
        void InstallPackages(const std::set<std::string>& pPackageList);

        /**
         * \brief Remove a package list
         *
         * Launch Cards Remove process
         */
        void RemovePackages(const std::set<std::string>& pPackageList);
            /**
         * \brief Suscribe to CARDS events
         *
         * Record callback from client class which submit callback from Card lib
         */
        void subscribeToEvents(cards_clientEvents* pCallBack);

        /**
         * \brief Unsuscribe from CARDS Events
         *
         * Unsuscribe client class callbock form Suscribe list
         */
        void unsubscribeFromEvents(cards_clientEvents* pCallBack);

        void progressInfo();

    };
}
