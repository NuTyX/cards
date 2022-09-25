/*
 * progressbox.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2022 Thierry Nuttens <tnut@nutyx.org>
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

#include "progressbox.h"

#include <FL/Fl_Output.H>

ProgressBox::ProgressBox(CW_ACTIONS action) :
    Fl_Double_Window(300,100)
{
    begin();
    m_jobProgress = nullptr;
    m_fileProgress = nullptr;
    m_cards = CWrapper::instance();
    m_cards->subscribeToEvents(this);
    switch (action)
    {
        case SYNC:
        {
            label("Database Synchronization ...");
            Fl_Output* Message = new Fl_Output(50,60,200,30);
            Message->value("Synchronizing, please wait...");

            break;
        }
        case DOJOB:
        {
            label("Package Install/Remove Job ...");
            /*m_jobProgress = new Fl_Progress(50,60,200,30);
            m_jobProgress->maximum(100.0);
            m_jobProgress->minimum(0.0);
            m_jobProgress->value(0.0);
            m_jobProgress->label("Job Progression");*/
            m_fileProgress = new Fl_Progress(50,35,200,30);
            m_fileProgress->maximum(100.0);
            m_fileProgress->minimum(0.0);
            m_fileProgress->value(0.0);
            m_fileProgress->label("Job Progression");
            m_fileProgress->align(FL_ALIGN_BOTTOM);
            m_fileProgress->color(FL_BLUE);
        }
    }
    this->callback(&Callback,(void*)this);
    end();
}

ProgressBox::~ProgressBox()
{
    m_cards->unsubscribeFromEvents(this);
    if (m_fileProgress!= nullptr) delete m_fileProgress;
    //if (m_jobProgress!= nullptr) delete m_jobProgress;
}

void ProgressBox::OnSyncFinished(const CEH_RC rc)
{
    Fl::lock();
    this->hide();
    Fl::unlock();
}

void ProgressBox::OnDoJobListFinished(const CEH_RC rc)
{
    Fl::lock();
    this->hide();
    Fl::flush();
    Fl::unlock();
}

void ProgressBox::OnProgressInfo(int percent)
{
    Fl::lock();
    m_fileProgress->value((float)percent);
    Fl::flush();
    Fl::unlock();
}

void ProgressBox::OnFileDownloadProgressInfo(FileDownloadState state)
{
    Fl::lock();
    if (m_fileProgress!=nullptr)
    {
        m_fileProgress->value(state.dlnow/state.dltotal*100);
    }
    Fl::flush();
    Fl::unlock();
}

void ProgressBox::Callback(Fl_Widget*,void* pInstance)
{
    return;
}
