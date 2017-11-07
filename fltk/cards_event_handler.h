/*
 * cards_event_handler.h
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

#ifndef  CARDS_EVENT_HANDLER_H
#define  CARDS_EVENT_HANDLER_H

#include <cstddef>
#include <string>
#include <set>

using namespace std;

enum CEH_RC
{
	OK=0,
	NO_ROOT,
	PKG_NOT_EXIST,
	EXCEPTION
};

// Define Cards_wrapper singleton for friendship
class Cards_wrapper;

/** \class Cards_event_handler
 * \brief Abstract class to handle event from cards_wrapper
 *
 * This class ensure interface cards with GUI application need non-blocking operation,
 * This is a single instance (singleton) that ensure only one instance of cards library.
 *
 */
class Cards_event_handler
{
    friend Cards_wrapper;
protected:

    virtual void OnLogMessage (const string& Message){}
    virtual void OnSyncFinished (const CEH_RC rc){}
    virtual void OnDoJobListFinished (const CEH_RC rc){}
    virtual void OnRefreshPackageFinished (const CEH_RC rc){}
    virtual void OnJobListChange (const CEH_RC rc){}
public:
static	const string getReasonCodeString(const CEH_RC rc);
};

#endif
