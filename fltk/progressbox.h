/*
 * progressbox.h
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

#ifndef PROGRESSBOX_H
#define PROGRESSBOX_H

#include "cards_wrapper.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Progress.H>


class ProgressBox : public Fl_Double_Window, public cards::cards_event_handler, public FileDownloadEvent
{
public:
    ProgressBox (cards::CW_ACTIONS action);
    ~ProgressBox ();
protected:
    void OnDoJobListFinished (const cards::CEH_RC rc);
    void OnSyncFinished(const cards::CEH_RC rc);
    void OnProgressInfo(int percent);
    void OnFileDownloadProgressInfo(FileDownloadState state);
static void Callback(Fl_Widget*,void* pInstance);
private:
    Fl_Progress* m_jobProgress;
    Fl_Progress* m_fileProgress;
    cards::cards_wrapper* m_cards;
};

#endif
