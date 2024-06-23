/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

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
