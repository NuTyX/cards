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

// Init static pointer to null
Cards_wrapper* Cards_wrapper::_ptCards_wrapper = nullptr;

// Constructor
Cards_wrapper::Cards_wrapper()
{
	_ptCards = new Cards_client("/etc/cards.conf");
	//redirect std::cout to the callback function
	redirect = new console_forwarder<>(std::cout, m_LogCallback);
	_job_running = false;
	_job=nullptr;
}

//Destructor
Cards_wrapper::~Cards_wrapper()
{
    if (_job != nullptr)
    {
        if (_job->joinable()) _job->join();
    }
    if (_ptCards != nullptr) delete _ptCards;
}

// Return or create singleton instance
Cards_wrapper* Cards_wrapper::instance()
{
	if (_ptCards_wrapper==nullptr)
		_ptCards_wrapper=new Cards_wrapper();
	return _ptCards_wrapper;
}

// Destruction of the singleton , only if instance exist
void Cards_wrapper::kill()
{
	if (_ptCards_wrapper!=nullptr)
		delete _ptCards_wrapper;
}

// Get list of installed package
void Cards_wrapper::getListOfInstalledPackages()
{
	if (!_job_running)
    {
        if (_job != nullptr)
        {
            _job->detach();
            delete _job;
            _job=nullptr;
        }
        if (_job==nullptr) _job = new thread(&Cards_wrapper::m_ListOfInstalledPackages_Thread, Cards_wrapper::_ptCards_wrapper);
    }
}

// Thread main function to get the list of installed package
void Cards_wrapper::m_ListOfInstalledPackages_Thread()
{
    _job_running =true;
	    m_ListOfInstalledPackagesCallback(_ptCards->ListOfInstalledPackages());

    _job_running =false;
}

// Broadcast the end of the thread ot get list of package to all event suscribers
void Cards_wrapper::m_ListOfInstalledPackagesCallback(const set<string>& ListOfInstalledPackages)
{
	for (auto* it : _arrCardsEventHandler)
	{
		it->ListOfInstalledPackages(ListOfInstalledPackages);
	}
}

// Capture cout events and send it to callback receivers
void Cards_wrapper::m_LogCallback(const char *ptr, std::streamsize count)
{
	if (Cards_wrapper::_ptCards_wrapper!=nullptr)
	{
		string Message(ptr,count);
		Message += '\0';
		for (auto* it : Cards_wrapper::_ptCards_wrapper->_arrCardsEventHandler)
		{
			it->OnLogMessage(Message);
		}
	}
}

// Add a nex suscriber to the callback event list
void Cards_wrapper::suscribeToEvents(Cards_event_handler* pCallBack)
{
	// Todo : Check if the new susciber is already in the container
	_arrCardsEventHandler.push_back(pCallBack);
}

// Remove an event suscriber from event callback list
void Cards_wrapper::unsuscribeToEvents(Cards_event_handler* pCallBack)
{
    vector<Cards_event_handler*>::iterator it;
    it=find(_arrCardsEventHandler.begin(),_arrCardsEventHandler.end(),pCallBack);
    if (it!=_arrCardsEventHandler.end())
    {
        _arrCardsEventHandler.erase(it);
    }
}

// Get and print the libcards Version

void Cards_wrapper::printCardsVersion()
{
    _ptCards->print_version();
}

// Create a new thread for Cards Sync operation

void Cards_wrapper::sync()
{
    if (!_job_running)
    {
        if (_job != nullptr)
        {
            _job->detach();
            delete _job;
            _job=nullptr;
        }
        if (_job==nullptr) _job = new thread(&Cards_wrapper::m_Sync_Thread, Cards_wrapper::_ptCards_wrapper);
    }
}

// Launch a Cards Sync operation

void Cards_wrapper::m_Sync_Thread()
{
    _job_running =true;
    CEH_RC rc=CEH_RC::OK;
    if (m_checkRootAccess())
    {
		Pkgsync Sync;
		Sync.run();
    }
    else
	{
		rc= CEH_RC::NO_ROOT;
	}
	_job_running = false;
	m_SyncFinishedCallback(rc);
}

// Broadcast to all suscribers the Sync Finished callback

void Cards_wrapper::m_SyncFinishedCallback(const CEH_RC rc=CEH_RC::OK)
{
	for (auto* it : _arrCardsEventHandler)
	{
		it->OnSyncFinished(rc);
	}
}

// Check if the application is curently running as root
// If it is the case, it return true.

bool Cards_wrapper::m_checkRootAccess()
{
    if (getuid() != 0)
    {
        return false;
    }
    return true;
}
