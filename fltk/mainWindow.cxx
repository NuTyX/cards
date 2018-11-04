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
mainWindow::mainWindow(bool pInstaller) :
    Fl_Double_Window(900,900,APP_NAME_VERSION_STR)
{
    _log = CLogger::instance();
    _log->subscribe(this);
    icon(new Fl_RGB_Image(new Fl_Pixmap(flcards_xpm)));
    size_range(400,500,0,0);
    _config = new Fl_Preferences(Fl_Preferences::USER,"nutyx","flcards");
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

    //Default Tab size and position
    int TabLeftCoord = MARGIN;
    int TabWidth = w()-MARGIN*2;
    if (pInstaller)
    {
        // Gui Package Button for easy install of a desktop manager
        _packList = new PackList(MARGIN,MARGIN +50 , 120,h()-400);
        _packList->resizable(0);
        TabLeftCoord += 130;
        TabWidth -= 130;
    }

    _tab = new Tableau(TabLeftCoord, MARGIN+50, TabWidth, h()-400);
    resizable(_tab);
    _cards = CWrapper::instance();
    _cards->subscribeToEvents(this);
    //_log->log(_("FlCards use ") + _cards->getCardsVersion());
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
    if (_config != nullptr)
    {
        int X,Y,H,W;
        _config->get("MainWindowX",X,100);
        _config->get("MainWindowY",Y,100);
        _config->get("MainWindowH",H,700);
        _config->get("MainWindowW",W,700);
        resize(X,Y,W,H);
    }
}

// Preference File Save and flush
void mainWindow::SaveConfig()
{
    if (_config != nullptr)
    {
        _config->set("MainWindowX",x_root());
        _config->set("MainWindowY",y_root());
        _config->set("MainWindowH",h());
        _config->set("MainWindowW",w());
        _config->flush();
    }
}


// Callback on Sync Button click
void mainWindow::SyncButton_CB(Fl_Widget*, void* pInstance)
{
    ProgressBox* box = new ProgressBox(SYNC);
    box->set_modal();
    CWrapper::instance()->sync();
    box->show();
    while (box->shown()) Fl::wait();
    delete box;
}

// Callback on Apply Button click
void mainWindow::ApplyButton_CB(Fl_Widget*, void* pInstance)
{
    ProgressBox* box = new ProgressBox(DOJOB);
    box->set_modal();
    CWrapper::instance()->doJobList();
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
    cout << "Sync : " << CEventHandler::getReasonCodeString(rc) << endl;
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
    vector<CPackage*> jobList = _cards->getJobList();
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

///
/// \brief mainWindow::OnExit_CB
/// \param pWidget
/// \param pInstance
///
void mainWindow::OnExit_CB(Fl_Widget* pWidget, void* pInstance)
{
    while (CWrapper::instance()->IsJobRunning())
    {
        Fl::wait(1);
    }
    ((mainWindow*)pWidget)->SaveConfig();
    exit(0);
}
