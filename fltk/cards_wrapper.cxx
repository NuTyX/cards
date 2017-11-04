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
///
/// Singleton Management
///

/** Init static pointer to null */
Cards_wrapper* Cards_wrapper::_ptCards_wrapper = nullptr;

/** Constructor */
Cards_wrapper::Cards_wrapper()
{
	_ptCards = new Cards_client("/etc/cards.conf");
	redirect_cout = new console_forwarder<>(std::cout, m_OnLogMessage_Callback); //redirect std::cout to the callback function
#ifndef NDEBUG
	redirect_cerr = new console_forwarder<>(std::cerr, m_OnLogMessage_Callback); //redirect std::cerr to the callback function
#endif // NDEBUG
	_job_running = false;
	_job=nullptr;
}

/** Destructor */
Cards_wrapper::~Cards_wrapper()
{
    if (_job != nullptr)
    {
        if (_job->joinable()) _job->join();
    }
    if (_ptCards != nullptr) delete _ptCards;
}

/** Return or create singleton instance */
Cards_wrapper* Cards_wrapper::instance()
{
	if (_ptCards_wrapper==nullptr)
		_ptCards_wrapper=new Cards_wrapper();
	return _ptCards_wrapper;
}

/** Destruction of the singleton , only if instance exist */
void Cards_wrapper::kill()
{
	if (_ptCards_wrapper!=nullptr)
		delete _ptCards_wrapper;
}



// Add a nex suscriber to the callback event list
void Cards_wrapper::suscribeToEvents(Cards_event_handler* pCallBack)
{
	// Todo : Check if the new susciber is already in the container
	_arrCardsEventHandler.push_back(pCallBack);
}

/// Remove an event suscriber from event callback list
void Cards_wrapper::unsuscribeToEvents(Cards_event_handler* pCallBack)
{
    vector<Cards_event_handler*>::iterator it;
    it=find(_arrCardsEventHandler.begin(),_arrCardsEventHandler.end(),pCallBack);
    if (it!=_arrCardsEventHandler.end())
    {
        _arrCardsEventHandler.erase(it);
    }
}

const vector<Cards_package*>& Cards_wrapper::getPackageList()
{
	return _arrCardsPackages;
}

///
/// Thread Callers
///

/** Create a new thread for Cards Sync operation*/
void Cards_wrapper::sync()
{
	if (m_IsThreadFree()) _job = new thread(&Cards_wrapper::m_Sync_Thread, Cards_wrapper::_ptCards_wrapper);
}

/** Create a new thread for Cards Sync operation*/
void Cards_wrapper::install(const set<string>& pPackageList)
{
	if (m_IsThreadFree()) _job = new thread(&Cards_wrapper::m_Install_Thread, Cards_wrapper::_ptCards_wrapper,pPackageList);
}

/** Create a new thread for Cards Sync operation*/
void Cards_wrapper::refreshPackageList()
{
	if (m_IsThreadFree()) _job = new thread(&Cards_wrapper::m_RefreshPackageList_Thread, Cards_wrapper::_ptCards_wrapper);
}

///
/// Threaded Tasks
///

/** Launch a Cards Sync operation*/
void Cards_wrapper::m_Sync_Thread()
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
			cout << "Exception occured" <<endl;
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
void Cards_wrapper::m_Install_Thread(const set<string>& pPackageList)
{
    _job_running =true;
    CEH_RC rc=CEH_RC::OK;
    if (m_checkRootAccess())
    {
		_ptCards->InstallPackage(pPackageList);
		m_RefreshPackageList_Thread();
    }
    else
	{
		rc= CEH_RC::NO_ROOT;
	}
	_job_running = false;
	m_OnInstallFinished_Callback(rc);
}


/** Threaded task to refresh the package image container*/
void Cards_wrapper::m_RefreshPackageList_Thread()
{
    _job_running =true;
    // First pass get all package available
	m_ClearPackagesList();
	set<string> AvailablePackages = _ptCards->getBinaryPackageList();
	set<string> InstalledPackages = _ptCards->ListOfInstalledPackages();
	for (auto it : AvailablePackages)
	{
		Cards_package* Pack=new Cards_package();
		string token;
		istringstream tokenStream(it);
		int i=0;
		while (std::getline(tokenStream, token, '\t'))
		{
			switch (i)
			{
			case 0: //Base
				{
					Pack->_base = token;
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
		if (InstalledPackages.find(Pack->_name)!=InstalledPackages.end()) Pack->_installed=true;
		_arrCardsPackages.push_back(Pack);
#ifndef NDEBUG
			cerr <<Pack->getBase()<<" : "
				 <<Pack->getName()<<" : "
				 <<Pack->getVersion()<<" : "
				 <<Pack->getPackager()<<" : "
				 <<Pack->getDescription()<<" : "
				 <<Pack->isInstalled()<<endl;
#endif
	}
	m_OnRefreshPackageFinished_Callback(CEH_RC::OK);
    _job_running =false;
}


///
/// Callbacks events
///

/** Capture cout events and send it to callback receivers*/
void Cards_wrapper::m_OnLogMessage_Callback(const char *ptr, std::streamsize count)
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

/** Broadcast to all suscribers the Sync Finished callback*/
void Cards_wrapper::m_OnSyncFinished_Callback(const CEH_RC rc=CEH_RC::OK)
{
	for (auto* it : _arrCardsEventHandler)
	{
		it->OnSyncFinished(rc);
	}
}

/** Broadcast to all suscribers the Install Finished callback*/
void Cards_wrapper::m_OnInstallFinished_Callback(const CEH_RC rc=CEH_RC::OK)
{
	for (auto* it : _arrCardsEventHandler)
	{
		it->OnInstallFinished(rc);
	}
}


/** Broadcast the end of the thread ot get list of package to all event suscribers*/
void Cards_wrapper::m_OnRefreshPackageFinished_Callback(const CEH_RC rc=CEH_RC::OK)
{
	for (auto* it : _arrCardsEventHandler)
	{
		it->OnRefreshPackageFinished(rc);
	}
}

///
/// Misc
///

/** Get and print the libcards Version*/

void Cards_wrapper::printCardsVersion()
{
    _ptCards->print_version();
}

/** Check if the application is curently running as root
  * If it is the case, it return true.*/
bool Cards_wrapper::m_checkRootAccess()
{
    if (getuid() != 0)
    {
        return false;
    }
    return true;
}

/** Ensure the thread instance is free and ready to launch a new task */
bool Cards_wrapper::m_IsThreadFree()
{
	if (!_job_running)
    {
        if (_job != nullptr)
        {
            _job->detach();
            delete _job;
            _job=nullptr;
        }
        if (_job==nullptr) return true;
    }
    return false;
}

void Cards_wrapper::m_ClearPackagesList()
{
	for (auto* it : _arrCardsPackages)
	{
		if (it!=nullptr)
		{
			delete it;
		}
	}
	_arrCardsPackages.clear();
}
