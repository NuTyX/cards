/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

#include <libcards.h>

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>


namespace cards
{

    enum CEH_RC
    {
        OK=0,
        NO_ROOT,
        PKG_NOT_EXIST,
        EXCEPTION
    };

    // Define cards_wrapper singleton for friendship
    class cards_wrapper;

    // Define Cards cards_logger singleton for friednship
    class cards_logger;
    /** \class cards_event_handler
     * \brief Abstract class to handle event from cards_wrapper
     *
     * This class ensure interface cards with GUI application need non-blocking operation,
     * This is a single instance (singleton) that ensure only one instance of cards library.
     *
     */
    class cards_event_handler
    {
        // Wrapper and Logger can access those protected methods
        friend cards_wrapper;
        friend cards_logger;
    protected:

        virtual void OnLogMessage (const std::string& Message){} //NOP method like
        virtual void OnSyncFinished (const CEH_RC rc){}
        virtual void OnDoJobListFinished (const CEH_RC rc){}
        virtual void OnRefreshPackageFinished (const CEH_RC rc){}
        virtual void OnJobListChange (const CEH_RC rc){}
        virtual void OnProgressInfo (int percent){}
        virtual void OnPackageInfo (cards::cache& package){}

    public:
        static	const std::string getReasonCodeString(const CEH_RC rc);
    };
}
