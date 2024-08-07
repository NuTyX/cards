/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

 #include "cards_log.h"

namespace cards
{
    cards_logger* cards_logger::m_ptLogger=nullptr;

    cards_logger::cards_logger()
    {
        m_ThreadId = std::this_thread::get_id();
    }

    ///
    ///
    ///
    cards_logger* cards_logger::instance()
    {
        if (m_ptLogger==nullptr)
        {
            m_ptLogger=new cards_logger;
        }
        return m_ptLogger;
    }

    ///
    ///
    ///
    void cards_logger::kill()
    {
        if (m_ptLogger!=nullptr)
        {
            delete m_ptLogger;
            m_ptLogger=nullptr;
        }
    }

    ///
    ///
    ///
    void cards_logger::loopCallback()
    {
        if (m_ptLogger!=nullptr)
        {
            m_ptLogger->m_ArrMutex.lock();
            for (std::string str : m_ptLogger->m_ArrMessages)
            {
                m_ptLogger->sendToSubscribers(str);
            }
            m_ptLogger->m_ArrMessages.clear();
            m_ptLogger->m_ArrMutex.unlock();
        }
    }

    ///
    /// Thread Safe Logging Message with Critical Level
    ///
    void cards_logger::log(const std::string& pMessage, CL_DEBUG_LEVEL pLevel)
    {
        std::string PostMess;
        switch (pLevel)
        {
            case LEVEL_DEBUG:
            {
                PostMess += "Debug : ";
                break;
            }
            case LEVEL_ERROR:
            {
                PostMess += "Error : ";
                break;
            }
            case LEVEL_INFO:
            {
                PostMess += "Info : ";
                break;
            }
            case LEVEL_WARNING:
            {
                PostMess += "Warning : ";
                break;
            }
        }
        PostMess += pMessage + '\n';
        if (m_ThreadId == std::this_thread::get_id())
        {
            sendToSubscribers(PostMess);
        }
        else
        {
            m_ArrMutex.lock();
            m_ArrMessages.push_back(PostMess);
            while(m_ArrMessages.size() > MAX_BUFFER)
            {
                m_ArrMessages.erase(m_ArrMessages.begin());
            }
            m_ArrMutex.unlock();
        }
    }

    ///
    ///
    ///
    void cards_logger::subscribe(cards_event_handler* pSubscriber)
    {
        if (find(m_ArrSubscribers.begin(), m_ArrSubscribers.end(), pSubscriber)==m_ArrSubscribers.end())
        {
            m_ArrSubscribers.push_back(pSubscriber);
        }
    }

    ///
    ///
    ///
    void cards_logger::unSubscribe(cards_event_handler* pSubscriber)
    {
        auto it = find(m_ArrSubscribers.begin(), m_ArrSubscribers.end(), pSubscriber);
        if (it != m_ArrSubscribers.end())
        {
            m_ArrSubscribers.erase(it);
        }
    }

    ///
    ///
    ///
    void cards_logger::sendToSubscribers(const std::string& pMessage)
    {
        for (cards_event_handler* it : m_ArrSubscribers)
        {
            it->OnLogMessage(pMessage);
        }
    }
}
