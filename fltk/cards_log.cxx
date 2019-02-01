/*
 * cards_log.cxx
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

 #include "cards_log.h"

namespace cards
{
    CLogger* CLogger::m_ptLogger=nullptr;

    CLogger::CLogger()
    {
        m_ThreadId = this_thread::get_id();
    }

    ///
    ///
    ///
    CLogger* CLogger::instance()
    {
        if (m_ptLogger==nullptr)
        {
            m_ptLogger=new CLogger;
        }
        return m_ptLogger;
    }

    ///
    ///
    ///
    void CLogger::kill()
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
    void CLogger::loopCallback()
    {
        if (m_ptLogger!=nullptr)
        {
            m_ptLogger->m_ArrMutex.lock();
            for (string str : m_ptLogger->m_ArrMessages)
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
    void CLogger::log(const string& pMessage, CL_DEBUG_LEVEL pLevel)
    {
        string PostMess;
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
        if (m_ThreadId == this_thread::get_id())
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
    void CLogger::subscribe(CEventHandler* pSubscriber)
    {
        if (find(m_ArrSubscribers.begin(), m_ArrSubscribers.end(), pSubscriber)==m_ArrSubscribers.end())
        {
            m_ArrSubscribers.push_back(pSubscriber);
        }
    }

    ///
    ///
    ///
    void CLogger::unSubscribe(CEventHandler* pSubscriber)
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
    void CLogger::sendToSubscribers(const string& pMessage)
    {
        for (CEventHandler* it : m_ArrSubscribers)
        {
            it->OnLogMessage(pMessage);
        }
    }
}
