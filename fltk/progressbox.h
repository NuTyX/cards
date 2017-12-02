/*
 * progressbox.h
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

#ifndef PROGRESSBOX_H
#define PROGRESSBOX_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Progress.H>
#include "cards_wrapper.h"

class ProgressBox : public Fl_Double_Window, public Cards_event_handler
{
public:
	ProgressBox (CARDS_ACTIONS action);
	~ProgressBox ();
protected:
	void OnDoJobListFinished (const CEH_RC rc);
	void OnSyncFinished(const CEH_RC rc);
	void OnProgressInfo(int percent);
private:
	Fl_Progress* _progress;
	Cards_wrapper* _cards;
};

#endif
