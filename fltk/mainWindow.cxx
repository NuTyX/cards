/*
 * mainWindow.cxx
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

#include "mainWindow.h"

// Constructor of the main window
mainWindow::mainWindow(int W=900, int H=900, string Title="Default") :
	Fl_Window(W,H,Title.c_str())
{
	icon(new Fl_RGB_Image(new Fl_Pixmap(flcards_xpm)));
	_search = new Fl_Input(MARGIN+450, MARGIN+5, 400, 30, "Search:");
	_search->labelfont(2);
	_search->color(FL_WHITE);
	_search->callback(&SearchInput_CB, (void*)this);
	_search->when(FL_WHEN_CHANGED);

	_console = new Fl_Text_Display(MARGIN, 600, w()-MARGIN*2, 300-MARGIN, "Info about the selected package:");
	_tbuff = new Fl_Text_Buffer();
	_console->buffer(_tbuff);
	_console->color(FL_BLACK);
	_console->textcolor(FL_GRAY);
	_console->labeltype(FL_NO_LABEL );

	//Creation of the Sync Button
	_btnSync = new Fl_Button(MARGIN, MARGIN, 100, 40, "Sync");
	_btnSync->callback(&SyncButton_CB,(void*)this);

	//Creation of the Apply Button
	_btnApply = new Fl_Button(MARGIN+120, MARGIN, 100, 40, "Apply");
	_btnApply->deactivate(); // Disabled by default until a modification is pending
	_btnApply->callback(&ApplyButton_CB,(void*)this);

	_tab = new Tableau(MARGIN, MARGIN+50, w()-MARGIN*2, h()-400);
	_cards = Cards_wrapper::instance();
	_cards->subscribeToEvents(this);
	cout << "FlCards use ";
	_cards->printCardsVersion();
	this->callback(&OnExit_CB,(void*)this);
}

// Main window destructor
mainWindow::~mainWindow()
{
	//todo : wait end of threads
	//Cards->join();
	_cards->unsubscribeFromEvents(this);
	_cards->kill();
}

// Callback on Sync Button click
void mainWindow::SyncButton_CB(Fl_Widget*, void* pInstance)
{
	Cards_wrapper::instance()->sync();
}

// Callback on Apply Button click
void mainWindow::ApplyButton_CB(Fl_Widget*, void* pInstance)
{
	Cards_wrapper::instance()->doJobList();
}

// Callback on Apply Button click
void mainWindow::SearchInput_CB(Fl_Widget*, void* pInstance)
{
	mainWindow* o=(mainWindow*)pInstance;
	o->_tab->setFilter(string(o->_search->value()));
}

// Callback on receive text to log
void mainWindow::OnLogMessage(const string& pMessage)
{
	Fl::lock();
	if (_tbuff!=nullptr) _tbuff->append(pMessage.c_str());
	_console->insert_position(_console->buffer()->length());
	_console->scroll(_console->count_lines(0,_console->buffer()->length(),1),0);
	Fl::unlock();
}

// Event Callback when Sync Thread is finished
void mainWindow::OnSyncFinished(const CEH_RC rc)
{
	Fl::lock();
	cout << "Sync : " << Cards_event_handler::getReasonCodeString(rc) << endl;
	Fl::unlock();
}

void mainWindow::OnJobListChange(const CEH_RC rc)
{
	Fl::lock();
	vector<Cards_package*> jobList = _cards->getJobList();
	if (jobList.size() > 0)
	{
		_btnApply->activate();
	}
	else
	{
		_btnApply->deactivate();
	}
	Fl::unlock();
}

void mainWindow::OnExit_CB(Fl_Widget* pWidget, void* pInstance)
{
	//Cards_wrapper::instance()->JoinThreads();
	((Fl_Window*)pInstance)->hide();
}
