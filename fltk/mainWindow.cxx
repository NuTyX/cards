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
    size_range(700,500,0,0);
    _config = new Fl_Preferences(Fl_Preferences::USER,"nutyx","flcards");
    _search = new Fl_Input(MARGIN+560, MARGIN+5, 300, 30, "Search...");
    _search->labelfont(2);
    _search->color(FL_WHITE);
    _search->callback(&onWindowEvent,SEARCH_CHANGE);
    _search->when(FL_WHEN_CHANGED);

    _console = new Fl_Text_Display(MARGIN, 700, w()-MARGIN*2, 200-MARGIN, "Cards operation");
    _consoleBuff = new Fl_Text_Buffer();
    _console->buffer(_consoleBuff);
    _console->color(FL_GRAY);
    _console->textcolor(FL_BLACK);
    _console->labeltype(FL_NO_LABEL );

    //Creation of the Sync Button
    _btnSync = new Fl_Button(MARGIN, MARGIN, 100, 40, "Sync");
    _btnSync->callback(&onWindowEvent,BTN_SYNC);

    //Creation of the Apply Button
    _btnApply = new Fl_Button(MARGIN+120, MARGIN, 100, 40, "Apply");
    _btnApply->deactivate(); // Disabled by default until a modification is pending
    _btnApply->callback(&onWindowEvent,BTN_APPLY);


    //Default Tab size and position
    int TabLeftCoord = MARGIN;
    int TabWidth = w()-300-MARGIN*3;

    _tabs = new Fl_Tabs(TabLeftCoord,MARGIN +50 , TabWidth,h()-325);
    {
        Fl_Group* _grpPackage = new Fl_Group(TabLeftCoord,MARGIN+80,TabWidth,h()-330,"Packages");
        {
            _table = new Tableau(TabLeftCoord, MARGIN+80, TabWidth, h()-330);
            resizable(_table);
        }
        _grpPackage->end();
        Fl_Group* _grpCollection = new Fl_Group(TabLeftCoord,MARGIN+80,TabWidth,h()-330,"Collections");
        {
            _packList = new PackList(TabLeftCoord,MARGIN +80 , TabWidth,h()-330);
            resizable (_packList);
        }
        _grpCollection->end();
        if (pInstaller) _tabs->value(_grpCollection);
    }
    _tabs->end();
    _info = new Fl_Text_Display (TabLeftCoord + TabWidth+MARGIN,MARGIN +80 , 300,h()-325,"Package Information");
    _infoBuff = new Fl_Text_Buffer();
    _info->buffer(_infoBuff);
    _cards = CWrapper::instance();
    _cards->subscribeToEvents(this);
    //_log->log(_("FlCards use ") + _cards->getCardsVersion());
    this->callback(&onWindowEvent,EVT_EXIT);
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

// Process Fltk Window events
void mainWindow::onWindowEvent(Fl_Widget* pWidget, long pID)
{
    if (pWidget==nullptr) return;
    mainWindow* win = reinterpret_cast<mainWindow*>(pWidget);
    while (win->parent()!=nullptr) win = reinterpret_cast<mainWindow*>(win->parent());
    switch (static_cast<widgetID>(pID))
    {
        case BTN_SYNC:
        {
            ProgressBox* box = new ProgressBox(SYNC);
            box->set_modal();
            CWrapper::instance()->sync();
            box->show();
            while (box->shown()) Fl::wait();
            delete box;
            break;
        }
        case BTN_APPLY:
        {
            ProgressBox* box = new ProgressBox(DOJOB);
            box->set_modal();
            CWrapper::instance()->doJobList();
            box->show();
            while (box->shown()) Fl::wait();
            delete box;
            break;
        }
        case SEARCH_CHANGE:
        {
            win->_table->setFilter(string(win->_search->value()));
            break;
        }
        case EVT_EXIT:
        {
            while (CWrapper::instance()->isJobRunning())
            {
                Fl::wait(1);
            }
            win->SaveConfig();
            exit(0);
            break;
        }
        default:
        {
            win->_log->log(_("Unkown Event : ") + to_string(pID));
        }
    }
}

// Callback on receive text to log
void mainWindow::OnLogMessage(const string& pMessage)
{
    Fl::lock();
    if (_consoleBuff!=nullptr) _consoleBuff->append(pMessage.c_str());
    _console->insert_position(_console->buffer()->length());
    _console->scroll(_console->count_lines(0,_console->buffer()->length(),1),0);
    Fl::flush();
    Fl::unlock();
}

// Event Callback when Sync Thread is finished
void mainWindow::OnSyncFinished(const CEH_RC rc)
{
    Fl::lock();
    _log->log( "Sync : " + CEventHandler::getReasonCodeString(rc));
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

void mainWindow::OnPackageInfo(CPackage& pPackage)
{
    Fl::lock();
    if (_infoBuff!=nullptr)
    {
        _infoBuff->remove(0,_infoBuff->length());
        if (pPackage.getName()=="")
            _infoBuff->append("No information available for this package");
        else
        {
            _infoBuff->append(string("Name : " + pPackage.getName()+"\n").c_str());
            _infoBuff->append(string("Description : " + pPackage.getDescription()+"\n").c_str());
            _infoBuff->append(string("Version : " + pPackage.getVersion()+"\n").c_str());
            _infoBuff->append(string("Packager : " + pPackage.getPackager()+"\n").c_str());
            _infoBuff->append(string("Collection : " + pPackage.getCollection()+"\n").c_str());
        }

    }
    Fl::flush();
    Fl::unlock();
}
