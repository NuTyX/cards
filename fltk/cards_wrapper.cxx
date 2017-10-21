/*
 * cards_wrapper.cxx
 *
 * Copyright 2015-2017 Thierry Nuttens <tnut@nutyx.org>
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

Cards_wrapper::~Cards_wrapper()
{
    if (_job != nullptr)
    {
        if (_job->joinable()) _job->join();
    }
    if (_ptCards != nullptr) delete _ptCards;
}

Cards_wrapper* Cards_wrapper::instance()
{
	if (_ptCards_wrapper==nullptr)
		_ptCards_wrapper=new Cards_wrapper();
	return _ptCards_wrapper;
}

void Cards_wrapper::kill()
{
	if (_ptCards_wrapper!=nullptr)
		delete _ptCards_wrapper;
}

void Cards_wrapper::getListOfInstalledPackages()
{

}

void Cards_wrapper::m_LogCallback(const char *ptr, std::streamsize count)
{
	if (Cards_wrapper::_ptCards_wrapper!=nullptr)
	{
	    vector<Cards_event_handler*> arr = Cards_wrapper::_ptCards_wrapper->_arrCardsEventHandler;
		string Message(ptr,count);
		Message += '\0';
		for (auto* it : arr)
		{
			it->OnLogMessage(Message);
		}
	}
}

void Cards_wrapper::suscribeToEvents(Cards_event_handler* pCallBack)
{
	_arrCardsEventHandler.push_back(pCallBack);
}

void Cards_wrapper::unsuscribeToEvents(Cards_event_handler* pCallBack)
{
    vector<Cards_event_handler*>::iterator it;
    it=find(_arrCardsEventHandler.begin(),_arrCardsEventHandler.end(),pCallBack);
    if (it!=_arrCardsEventHandler.end())
    {
        _arrCardsEventHandler.erase(it);
    }
}
void Cards_wrapper::printCardsVersion()
{
    _ptCards->print_version();
}

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

void Cards_wrapper::m_Sync_Thread()
{
    _job_running =true;
    if (m_checkRootAccess())
    {
		Config config;
		string m_repoFile = ".PKGREPO";
		Pkgrepo::parseConfig("/etc/cards.conf", config);
		for (vector<DirUrl>::iterator i = config.dirUrl.begin();i != config.dirUrl.end(); ++i)
		{
			DirUrl DU = *i ;
			if (DU.Url.size() == 0 )
			{
				continue;
			}
			string categoryDir, url ;
			categoryDir = DU.Dir;
			url = DU.Url;
			string category = basename(const_cast<char*>(categoryDir.c_str()));
			string categoryPKGREPOFile = categoryDir + "/" + m_repoFile;
			FileDownload PKGRepo(url + "/" + m_repoFile,
				categoryDir,
				m_repoFile, false);
		}
    }
	_job_running = false;
	m_SyncFinishedCallback();
}

void Cards_wrapper::m_SyncFinishedCallback()
{
	for (auto* it : _arrCardsEventHandler)
	{
		it->OnSyncFinished();
	}
}


bool Cards_wrapper::m_checkRootAccess()
{
    //setuid(0);
    if (getuid() != 0)
    {
        cout << "Root privileges are needed for this action!" << endl;
        return false;
    }
    return true;
}
