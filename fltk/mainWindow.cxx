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
#include "version.h"

/// Constructor of the main window
mainWindow::mainWindow() :
	Fl_Double_Window(900,900,APP_NAME_VERSION_STR)
{
	icon(new Fl_RGB_Image(new Fl_Pixmap(flcards_xpm)));
	Config = new Fl_Preferences(Fl_Preferences::USER,"nutyx","flcards");
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
	//Fl_Scroll* ToolBoxContain = new Fl_Scroll (MARGIN,MARGIN +50 , 64,h()-400);
	//ToolBoxContain->begin();
	_packList = new PackList(MARGIN,MARGIN +50 , 120,h()-400);
	_packList->resizable(0);
	size_range(400,500,0,0);
	//ToolBoxContain->end();
	_tab = new Tableau(MARGIN+130, MARGIN+50, w()-MARGIN*2-130, h()-400);
	resizable(_tab);
	_cards = Cards_wrapper::instance();
	_cards->subscribeToEvents(this);
	cout << "FlCards use ";
	_cards->printCardsVersion();
	this->callback(&OnExit_CB,(void*)this);
}

// Main window destructor
mainWindow::~mainWindow()
{
	_cards->unsubscribeFromEvents(this);
	_cards->kill();
}

// Preference File Loading and apply
void mainWindow::LoadConfig()
{
	if (Config != nullptr)
	{
		int X,Y,H,W;
		Config->get("MainWindowX",X,100);
		Config->get("MainWindowY",Y,100);
		Config->get("MainWindowH",H,700);
		Config->get("MainWindowW",W,700);
		resize(X,Y,W,H);
	}
}

// Preference File Save and flush
void mainWindow::SaveConfig()
{
	if (Config != nullptr)
	{
		Config->set("MainWindowX",x_root());
		Config->set("MainWindowY",y_root());
		Config->set("MainWindowH",h());
		Config->set("MainWindowW",w());
		Config->flush();
	}
}


// Callback on Sync Button click
void mainWindow::SyncButton_CB(Fl_Widget*, void* pInstance)
{
	ProgressBox* box = new ProgressBox(SYNC);
	box->set_modal();
	Cards_wrapper::instance()->sync();
	box->show();
	while (box->shown()) Fl::wait();
	delete box;
}

// Callback on Apply Button click
void mainWindow::ApplyButton_CB(Fl_Widget*, void* pInstance)
{
	ProgressBox* box = new ProgressBox(DOJOB);
	box->set_modal();
	Cards_wrapper::instance()->doJobList();
	box->show();
	while (box->shown()) Fl::wait();
	delete box;
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
	Fl::flush();
	Fl::unlock();
}

// Event Callback when Sync Thread is finished
void mainWindow::OnSyncFinished(const CEH_RC rc)
{
	Fl::lock();
	cout << "Sync : " << Cards_event_handler::getReasonCodeString(rc) << endl;
	if (rc==NO_ROOT)
	{
		fl_alert("Please launch this application with root privileges");
	}
	Fl::flush();
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
	Fl::flush();
	Fl::unlock();
}

void mainWindow::OnExit_CB(Fl_Widget* pWidget, void* pInstance)
{
	while (Cards_wrapper::instance()->IsJobRunning())
	{
		Fl::wait(1);
	}
	((mainWindow*)pWidget)->SaveConfig();
    exit(0);
}
