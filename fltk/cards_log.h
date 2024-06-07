/*
 * cards_log.h
 *
 * Copyright 2018 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2018 - 2022 Thierry Nuttens <tnut@nutyx.org>
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

#include "cards_event_handler.h"

#include <algorithm>
#include <sstream>
#include <mutex>
#include <thread>

namespace cards
{
    enum CL_DEBUG_LEVEL
    {
        LEVEL_INFO,
        LEVEL_WARNING,
        LEVEL_DEBUG,
        LEVEL_ERROR,
    };

    class cards_logger
    {
    private:
        cards_logger();
        virtual ~cards_logger(){}
        static cards_logger* m_ptLogger;
        std::vector<std::string> m_ArrMessages;
        std::vector<cards_event_handler*> m_ArrSubscribers;
        std::mutex m_ArrMutex;
        std::thread::id m_ThreadId;
        void sendToSubscribers(const std::string& pMessage);

    public:
        static cards_logger* instance();
        static void kill();
        void log ( const std::string& pMessage,
            CL_DEBUG_LEVEL pLevel=LEVEL_INFO);
        static void loopCallback();
        void subscribe (cards_event_handler* pSubscriber);
        void unSubscribe (cards_event_handler* pSubscriber);
    };
}

#endif
