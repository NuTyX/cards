/*
 * cards_log.h
 *
 * Copyright 2018 Gianni Peschiutta <artemia@nutyx.org>
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

#ifndef  CARDS_LOG_H
#define  CARDS_LOG_H

#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>

#include "cards_event_handler.h"

#ifndef _
    #define _(T) std::string(T)
#endif
#define MAX_BUFFER 50

namespace cards
{
    using namespace std;
    enum CL_DEBUG_LEVEL
    {
        LEVEL_INFO,
        LEVEL_WARNING,
        LEVEL_DEBUG,
        LEVEL_ERROR,
    };

    class CLogger
    {
    private:
        CLogger();
        virtual ~CLogger(){}
        static CLogger* m_ptLogger;
        vector<string> m_ArrMessages;
        vector<CEventHandler*> m_ArrSubscribers;
        mutex m_ArrMutex;
        thread::id m_ThreadId;
        void sendToSubscribers(const string& pMessage);

    public:
        static CLogger* instance();
        static void kill();
        void log ( const string& pMessage,
            CL_DEBUG_LEVEL pLevel=LEVEL_INFO);
        static void loopCallback();
        void subscribe (CEventHandler* pSubscriber);
        void unSubscribe (CEventHandler* pSubscriber);
    };
}

#endif
