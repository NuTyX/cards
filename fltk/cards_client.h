/*
 * cards_client.h
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

#ifndef  CARDS_CLIENT_H
#define  CARDS_CLIENT_H

#include <cstddef>

#include <libcards.h>

#include "cards_log.h"

namespace cards
{
    using namespace std;

    // Define Cards_wrapper singleton for friendship
    class CWrapper;
    class CClient;

    // Abstract Class to enable events callbacks
    class CClientEvents
    {
        friend CClient;
    protected:
        void OnProgressInfo(int percent){}
    };

    /** \class Cards_client
     * \brief Interface Class to access Cards Library Method through inheritence
     *
     * This class ensure interface cards with GUI application need non-blocking operation,
     * This is a single instance (singleton) that ensure only one instance of cards library.
     *
     */
    class CClient : public Pkginst
    {
        // Only Cards_wrapper can own this class
        friend CWrapper;

    protected:
        /**
         * \brief Constructor
         *
         * Constructor of Cards_client class
         */
        CClient ();

        /**
         * \brief Destructor
         *
         * Destructor of Cards_client class
         *
         * \return pointer of the singleton
         */
        virtual ~CClient (){};

        /**
         * \brief Get list of installed package
         *
         * Return string array content installed package
         *
         * \return string array contain installed package list
         */
        set<string> ListOfInstalledPackages();

        /**
         * \brief Install a package list
         *
         * Launch Cards Install process
         */
        void InstallPackages(const set<string>& pPackageList);

        /**
         * \brief Remove a package list
         *
         * Launch Cards Remove process
         */
        void RemovePackages(const set<string>& pPackageList);
            /**
         * \brief Suscribe to CARDS events
         *
         * Record callback from client class which submit callback from Card lib
         */
        void subscribeToEvents(CClientEvents* pCallBack);

        /**
         * \brief Unsuscribe from CARDS Events
         *
         * Unsuscribe client class callbock form Suscribe list
         */
        void unsubscribeFromEvents(CClientEvents* pCallBack);

        CPackage getPackageInfo(const string& pName);
    protected:
        void progressInfo();

    private:
        CLogger* _log;
        void getLocalePackagesList();
        vector<CClientEvents*> _arrCallback;
    };
}
#endif // CARDS_WRAPPER_H
