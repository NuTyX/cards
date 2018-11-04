/*
 * progressbox.cxx
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
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
    _jobProgress = nullptr;
    _fileProgress = nullptr;
    _cards = CWrapper::instance();
    _cards->subscribeToEvents(this);
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
            /*_jobProgress = new Fl_Progress(50,60,200,30);
            _jobProgress->maximum(100.0);
            _jobProgress->minimum(0.0);
            _jobProgress->value(0.0);
            _jobProgress->label("Job Progression");*/
            _fileProgress = new Fl_Progress(50,35,200,30);
            _fileProgress->maximum(100.0);
            _fileProgress->minimum(0.0);
            _fileProgress->value(0.0);
            _fileProgress->label("Job Progression");
            _fileProgress->align(FL_ALIGN_BOTTOM);
            _fileProgress->color(FL_BLUE);
        }
    }
    this->callback(&Callback,(void*)this);
    end();
}

ProgressBox::~ProgressBox()
{
    _cards->unsubscribeFromEvents(this);
    if (_fileProgress!= nullptr) delete _fileProgress;
    //if (_jobProgress!= nullptr) delete _jobProgress;
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
    _jobProgress->value((float)percent);
    Fl::flush();
    Fl::unlock();
}

void ProgressBox::OnFileDownloadProgressInfo(FileDownloadState state)
{
    Fl::lock();
    if (_fileProgress!=nullptr)
    {
        _fileProgress->value(state.dlnow/state.dltotal*100);
    }
    Fl::flush();
    Fl::unlock();
}

void ProgressBox::Callback(Fl_Widget*,void* pInstance)
{
    return;
}
