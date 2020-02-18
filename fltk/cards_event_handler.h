/*
 * cards_event_handler.h
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2020 Thierry Nuttens <tnut@nutyx.org>
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

#ifndef  CARDS_EVENT_HANDLER_H
#define  CARDS_EVENT_HANDLER_H

#include <cstddef>
#include <string>
#include <set>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>

#include "cards_package.h"

namespace cards
{
    using namespace std;

    enum CEH_RC
    {
        OK=0,
        NO_ROOT,
        PKG_NOT_EXIST,
        EXCEPTION
    };

    // Define Cards_wrapper singleton for friendship
    class CWrapper;

    // Define Cards CLogger singleton for friednship
    class CLogger;
    /** \class Cards_event_handler
     * \brief Abstract class to handle event from cards_wrapper
     *
     * This class ensure interface cards with GUI application need non-blocking operation,
     * This is a single instance (singleton) that ensure only one instance of cards library.
     *
     */
    class CEventHandler
    {
        // Wrapper and Logger can access those protected methods
        friend CWrapper;
        friend CLogger;
    protected:

        virtual void OnLogMessage (const string& Message){} //NOP method like
        virtual void OnSyncFinished (const CEH_RC rc){}
        virtual void OnDoJobListFinished (const CEH_RC rc){}
        virtual void OnRefreshPackageFinished (const CEH_RC rc){}
        virtual void OnJobListChange (const CEH_RC rc){}
        virtual void OnProgressInfo (int percent){}
        virtual void OnPackageInfo (CPackage& package){}

    public:
        static	const string getReasonCodeString(const CEH_RC rc);
    };
}
#endif
