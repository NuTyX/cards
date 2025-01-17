/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#include "mainWindow.h"

using namespace cards;

/// Constructor of the main window
mainWindow::mainWindow() :
    Fl_Double_Window(900,900,PACKAGE_VERSION)
{
/*
 *cards_logger is broken
 *    m_log = cards_logger::instance();
 *    m_log->subscribe(this);
*/
    icon(new Fl_RGB_Image(new Fl_Pixmap(flcards_xpm)));
    size_range(700,500,0,0);
    m_config = new Fl_Preferences(Fl_Preferences::USER,"nutyx","flcards");

    //Creation of the Sync Button
    m_btnSync = new Fl_Button(MARGIN, MARGIN, 100, 40, "Sync");
    m_btnSync->callback(&onWindowEvent,BTN_SYNC);

    //Creation of the Apply Button
    m_btnApply = new Fl_Button(MARGIN+120, MARGIN, 100, 40, "Go !");
    m_btnApply->deactivate(); // Disabled by default until a modification is pending
    m_btnApply->callback(&onWindowEvent,BTN_APPLY);

    //Default Tab size and position
    int TabLeftCoord = MARGIN;
    int TabWidth = w()-MARGIN*3;

    m_tablePackages = new table_package(MARGIN,
        MARGIN+80,
        TabWidth,
        h()-330,
        "All available packages");
    resizable(m_tablePackages);

    m_info = new Fl_Text_Display (MARGIN, 700, TabWidth, 190-MARGIN,"Package Information");
    m_infoBuff = new Fl_Text_Buffer();
    m_info->buffer(m_infoBuff);
    m_info->color(FL_GRAY0);
    m_info->textcolor(FL_WHITE);
    m_info->textfont(FL_COURIER);

    m_search = new Fl_Input(MARGIN+560, MARGIN+5, 300, 30, "Search...");
    m_search->labelfont(2);
    m_search->color(FL_WHITE);
    m_search->when(FL_WHEN_CHANGED);
    m_search->callback(&onWindowEvent,SEARCH_CHANGE);

    m_cards = cards_wrapper::instance();
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
	while (win->parent()!=nullptr)
        win = reinterpret_cast<mainWindow*>(win->parent());

	switch (static_cast<widgetID>(pID))
	{
		case BTN_SYNC:
		{
			ProgressBox* box = new ProgressBox(SYNC);
			box->set_modal();
			cards_wrapper::instance()->sync();
			box->show();
			while (box->shown())
                Fl::wait();
			delete box;
			break;
		}
		case BTN_APPLY:
		{
			ProgressBox* box = new ProgressBox(DOJOB);
			box->set_modal();
			cards_wrapper::instance()->doJobList();
			box->show();
			while (box->shown())
                Fl::wait();
			delete box;
		break;
		}
		case BTN_INSTALL:
		{
			if (win->m_tablePackages->install_selected()>0)
			{
				ProgressBox* box = new ProgressBox(DOJOB);
				box->set_modal();
				cards_wrapper::instance()->doJobList();
				 box->show();
				 while (box->shown())
					Fl::wait();
				delete box;
			}
			break;
		}
		case BTN_REMOVE:
		{
			if (win->m_tablePackages->remove_selected()>0)
			{
				ProgressBox* box = new ProgressBox(DOJOB);
				box->set_modal();
				cards_wrapper::instance()->doJobList();
				box->show();
				while (box->shown())
					Fl::wait();
				delete box;
			}
			break;
		}
		case SEARCH_CHANGE:
		{
			win->m_tablePackages->setFilter(std::string(win->m_search->value()));
/*			if (win->m_tabs->value() != win->m_grpPackage)
				win->m_tabs->value(win->m_grpPackage);*/
			break;
		}
		case EVT_EXIT:
		{
			while (cards_wrapper::instance()->isJobRunning())
			{
				Fl::wait(1);
			}
			win->SaveConfig();
			exit(0);
			break;
		}
		default:
		{
			std::cerr << "Unkown Event : "
				<< std::to_string(pID)
				<< std::endl;
			//win->m_log->log(_("Unkown Event : ") + std::to_string(pID));
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
//    m_log->log( "Sync : " + cards_event_handler::getReasonCodeString(rc));
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
    std::vector<cards::cache*> jobList = m_cards->getJobList();
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

void mainWindow::OnPackageInfo(cards::cache& pPackage)
{
    Fl::lock();
    if (m_infoBuff!=nullptr)
    {
        m_infoBuff->remove(0,m_infoBuff->length());
        if (pPackage.name()=="")
            m_infoBuff->append("No information available for this package");
        else
        {
            m_infoBuff->append(std::string(pPackage.name()).c_str());
            m_infoBuff->append(std::string(" " + pPackage.version()+"\n").c_str());
            m_infoBuff->append(std::string(pPackage.description()+"\n").c_str());
            m_infoBuff->append(std::string("build by: " + pPackage.packager()+"\n").c_str());
            m_infoBuff->append(std::string("from    : " + pPackage.collection()+" collection/set of packages").c_str());
        }

    }
    Fl::flush();
    Fl::unlock();
}
