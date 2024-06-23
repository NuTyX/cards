/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

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
