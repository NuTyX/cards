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

/** \class Cards_wrapper
 * \brief GUI interfacing wrapper for CARDS
 *
 * This class ensure interface cards with GUI application need non-blocking operation,
 * This is a single instance (singleton) that ensure only one instance of cards library.
 *
 */
class  Cards_wrapper
{
public:
	/**
	 * \brief Return or create the singleton
	 *
	 * This static method create the singleton if not yet done
	 *
	 * \return pointer of the singleton
	 */
static Cards_wrapper*  instance();

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
	void suscribeToEvents(Cards_event_handler* pCallBack);

	/**
	 * \brief Unsuscribe from CARDS Events
	 *
	 * Unsuscribe client class callbock form Suscribe list
	 */
	void unsuscribeToEvents(Cards_event_handler* pCallBack);

	/**
	 * \brief list of installed packages
	 *
	 * Return list of installed packages
	 */
	void getListOfInstalledPackages();

	/**
	 * \brief Get CARDS Version
	 *
	 * Return CARDS Library version
	 */
	void printCardsVersion();

	/**
	 * \brief Sync CARDS database
	 *
	 * Launch CARD Sync operation
	 * Callback is called when operation is done
	 */
	void sync();

private:

/**
 * \brief Constructor
 * Private constructor
 */
	Cards_wrapper();

/**
 * \brief Destructor
 * Private destructor
 */
	~Cards_wrapper();

static	Cards_wrapper*	_ptCards_wrapper; //Static pointer of the singleton

	bool _job_running; //Flag to know if a thread is currently running
	thread* _job; // Thread handler pointer
	Cards_client* _ptCards; // Cards Library Handler pointer
	vector<Cards_event_handler*> _arrCardsEventHandler; // Std array to store callback event clients
	console_forwarder<>* redirect; // Forwarding cout message to LogCallback callback
static void m_LogCallback(const char *ptr, std::streamsize count); //Callback for all cout text output from libcards
	void m_SyncFinishedCallback(const CEH_RC rc); // Callback broadcast for Sync Cards operation
	bool m_checkRootAccess(); // Just check if we have root accessing
	void m_Sync_Thread(); // Main Thread for Cards Sync Operation

};

#endif //  CARDS_WRAPPER_H
