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

ProgressBox::ProgressBox(CARDS_ACTIONS action) :
	Fl_Double_Window(300,150)
{
	begin();
	_progress = new Fl_Progress(50,60,200,30);
	_progress->maximum(100.0);
	_progress->minimum(0.0);
	_progress->value(50.0);
	_cards = Cards_wrapper::instance();
	_cards->subscribeToEvents(this);
	switch (action)
	{
		case SYNC:
		{
			label("Database Synchronization ...");
			break;
		}
		case DOJOB:
		{
			label("Package Install/Remove Job ...");
		}
	}
	end();
}

ProgressBox::~ProgressBox()
{
	_cards->unsubscribeFromEvents(this);
	delete _progress;
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
	Fl::unlock();
}

void ProgressBox::OnProgressInfo(int percent)
{
	Fl::lock();
	_progress->value((float)percent);
	Fl::unlock();
}
