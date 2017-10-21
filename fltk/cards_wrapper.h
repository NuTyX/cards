/*
 * cards_wrapper.h
 *
 * Copyright 2015-2017 Gianni P. <tnut@nutyx.org>
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

#include <cstddef>
#include <thread>
#include <vector>
#include <algorithm>

#include <libcards.h>
#include "console_forwarder.h"
#include "cards_client.h"
#include "cards_event_handler.h"

using namespace std;

/**
 * \brief Cards wrapper class
 *
 * This class ensure interface cards with GUI application need non-blocking operation,
 * This is a single instance (singleton) that ensure only one instance of cards library.
 *
 */

class  Cards_wrapper
{
public:
	/**
	 * return or create once instance of Cards_wrapper
	 */
static	Cards_wrapper*  instance();
	/**
	 * if exist, delete instance of Card_wrapper
	 */
	void				kill();

	void				suscribeToEvents(Cards_event_handler* pCallBack);
	void				unsuscribeToEvents(Cards_event_handler* pCallBack);
	void				getListOfInstalledPackages();
	void				printCardsVersion();
	void				sync();


protected:

private:
	/**
	 * Private constructor
	 */
						Cards_wrapper();
	/**
	 * Private destructor
	 */
						~Cards_wrapper();

static	Cards_wrapper*	_ptCards_wrapper;
	bool				_job_running;
	thread*				_job;
	Cards_client*		_ptCards;
	vector<Cards_event_handler*> _arrCardsEventHandler;
	console_forwarder<>*	redirect;
	static	void 		m_LogCallback(const char *ptr, std::streamsize count);
	void				m_SyncFinishedCallback();
	bool				m_checkRootAccess();
	void				m_Sync_Thread();

};

#endif //  CARDS_WRAPPER_H
