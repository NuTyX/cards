/*
 * mainWindow.cxx
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

#include "mainWindow.h"
#include "version.h"

/// Constructor of the main window
mainWindow::mainWindow(bool pInstaller) :
    Fl_Double_Window(900,900,APP_NAME_VERSION_STR)
{
    m_log = CLogger::instance();
    m_log->subscribe(this);
    icon(new Fl_RGB_Image(new Fl_Pixmap(flcards_xpm)));
    size_range(700,500,0,0);
    m_config = new Fl_Preferences(Fl_Preferences::USER,"nutyx","flcards");
    m_search = new Fl_Input(MARGIN+560, MARGIN+5, 300, 30, "Search...");
    m_search->labelfont(2);
    m_search->color(FL_WHITE);
    m_search->callback(&onWindowEvent,SEARCH_CHANGE);
    m_search->when(FL_WHEN_CHANGED);

    m_console = new Fl_Text_Display(MARGIN, 700, w()-MARGIN*2, 200-MARGIN, "Cards operation");
    m_consoleBuff = new Fl_Text_Buffer();
    m_console->buffer(m_consoleBuff);
    m_console->color(FL_GRAY);
    m_console->textcolor(FL_BLACK);
    m_console->labeltype(FL_NO_LABEL );

    //Creation of the Sync Button
    m_btnSync = new Fl_Button(MARGIN, MARGIN, 100, 40, "Sync");
    m_btnSync->callback(&onWindowEvent,BTN_SYNC);

    //Creation of the Apply Button
    m_btnApply = new Fl_Button(MARGIN+120, MARGIN, 100, 40, "Apply");
    m_btnApply->deactivate(); // Disabled by default until a modification is pending
    m_btnApply->callback(&onWindowEvent,BTN_APPLY);


    //Default Tab size and position
    int TabLeftCoord = MARGIN;
    int TabWidth = w()-300-MARGIN*3;

    m_tabs = new Fl_Tabs(TabLeftCoord,MARGIN +50 , TabWidth,h()-325);
    {
        m_grpPackage = new Fl_Group(TabLeftCoord,MARGIN+80,TabWidth,h()-330,"All Packages");
        {
            m_tablePackages = reinterpret_cast<TableBase*>(new TablePackage(TabLeftCoord, MARGIN+80, TabWidth, h()-330));
            resizable(m_tablePackages);
        }
        m_grpPackage->end();
        m_grpCollection = new Fl_Group(TabLeftCoord,MARGIN+80,TabWidth,h()-330,"Set of Packages");
        {
            m_tableCollections = reinterpret_cast<TableBase*>(new TableCollection(TabLeftCoord, MARGIN+80, TabWidth, h()-330));
            resizable (m_tableCollections);
        }
        m_grpCollection->end();
        if (pInstaller) m_tabs->value(m_grpCollection);
    }
    m_tabs->end();
    m_info = new Fl_Text_Display (TabLeftCoord + TabWidth+MARGIN,MARGIN +80 , 300,h()-325,"Package Information");
    m_infoBuff = new Fl_Text_Buffer();
    m_info->buffer(m_infoBuff);
    m_cards = CWrapper::instance();
    m_cards->subscribeToEvents(this);
    //_log->log(_("FlCards use ") + _cards->getCardsVersion());
    this->callback(&onWindowEvent,EVT_EXIT);
}

// Main window destructor
mainWindow::~mainWindow()
{
    m_cards->unsubscribeFromEvents(this);
    m_cards->kill();
}

// Preference File Loading and apply
void mainWindow::LoadConfig()
{
    if (m_config != nullptr)
    {
        int X,Y,H,W;
        m_config->get("MainWindowX",X,100);
        m_config->get("MainWindowY",Y,100);
        m_config->get("MainWindowH",H,700);
        m_config->get("MainWindowW",W,700);
        resize(X,Y,W,H);
    }
}

// Preference File Save and flush
void mainWindow::SaveConfig()
{
    if (m_config != nullptr)
    {
        m_config->set("MainWindowX",x_root());
        m_config->set("MainWindowY",y_root());
        m_config->set("MainWindowH",h());
        m_config->set("MainWindowW",w());
        m_config->flush();
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
            win->m_tablePackages->setFilter(std::string(win->m_search->value()));
            if (win->m_tabs->value() != win->m_grpPackage)
                win->m_tabs->value(win->m_grpPackage);
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
            win->m_log->log(_("Unkown Event : ") + std::to_string(pID));
        }
    }
}

// Callback on receive text to log
void mainWindow::OnLogMessage(const std::string& pMessage)
{
    Fl::lock();
    if (m_consoleBuff!=nullptr) m_consoleBuff->append(pMessage.c_str());
    m_console->insert_position(m_console->buffer()->length());
    m_console->scroll(m_console->count_lines(0,m_console->buffer()->length(),1),0);
    Fl::flush();
    Fl::unlock();
}

// Event Callback when Sync Thread is finished
void mainWindow::OnSyncFinished(const CEH_RC rc)
{
    Fl::lock();
    m_log->log( "Sync : " + CEventHandler::getReasonCodeString(rc));
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
    std::vector<CPackage*> jobList = m_cards->getJobList();
    if (jobList.size() > 0)
    {
        m_btnApply->activate();
    }
    else
    {
        m_btnApply->deactivate();
    }
    Fl::flush();
    Fl::unlock();
}

void mainWindow::OnPackageInfo(CPackage& pPackage)
{
    Fl::lock();
    if (m_infoBuff!=nullptr)
    {
        m_infoBuff->remove(0,m_infoBuff->length());
        if (pPackage.getName()=="")
            m_infoBuff->append("No information available for this package");
        else
        {
            m_infoBuff->append(std::string(pPackage.getName()).c_str());
            m_infoBuff->append(std::string(" " + pPackage.getVersion()+"\n").c_str());
            m_infoBuff->append(std::string(pPackage.getDescription()+"\n").c_str());
            m_infoBuff->append(std::string("build by: " + pPackage.getPackager()+"\n").c_str());
            m_infoBuff->append(std::string("from     : " + pPackage.getCollection()+" collection/set of packages").c_str());
        }

    }
    Fl::flush();
    Fl::unlock();
}
