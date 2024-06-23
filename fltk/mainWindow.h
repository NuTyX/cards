/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

// Project Includes
#include "pixmaps/flcards.xpm"
#include "table_package.h"
#include "cards_wrapper.h"
#include "progressbox.h"
#include "cards_log.h"


// Global Includes
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Check_Button.H>


enum widgetID
{
    BTN_SYNC,
    BTN_APPLY,
    BTN_INSTALL,
    BTN_REMOVE,
    SEARCH_CHANGE,
    CHKB_PACKAGE,
    CHKB_COLLECT,
    EVT_EXIT
};

/** \class mainWindow
 * \brief FTLTK Main window of application
 *
 * This class derivate Fl_Window for FLTK window handle
 * Card_event_handler inheritence is done to enable
 * receiving events callback from cards_wrapper :
 * This is a mechanism to ensure GUI application never blocked
 * by libcard operation. This also ensure libcard is quiet to
 * operate like if it was been launched on a terminal.
 */
class mainWindow : public Fl_Double_Window, public cards::cards_event_handler
{
public:
    /**
     * \brief Constructor
     *
     * Constructor of mainWindow class
     *
     */
    mainWindow();

    virtual ~mainWindow();
    void LoadConfig();

protected:
//Fltk callbacks
    static void onWindowEvent(Fl_Widget* pWidget,long pID);

//Cards Wrapper callbacks
    void OnSyncFinished(const cards::CEH_RC rc);
    void OnJobListChange(const cards::CEH_RC rc);
    void OnLogMessage(const std::string& pMessage);
    void OnPackageInfo(cards::cache& pPackage);

private:
    void SaveConfig();
    table_base* m_tablePackages;
    Fl_Input* m_search;
    Fl_Text_Display* m_console;
    Fl_Text_Display* m_info;
    Fl_Text_Buffer * m_consoleBuff;
    Fl_Text_Buffer * m_infoBuff;
    Fl_Button* m_btnSync;
    Fl_Button* m_btnApply;
    Fl_Button* m_btnInstall;
    Fl_Button* m_btnRemove;
    Fl_Tabs* m_tabs;
    cards::cards_wrapper* m_cards;
    Fl_Preferences* m_config;
    cards::cards_logger* m_log;
};
