/*
 * mainWindow.h
 *
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2020 Thierry Nuttens <tnut@nutyx.org>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Global Includes
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
#include <string>
#include <vector>

//Project Includes
#include "pixmaps/flcards.xpm"
#include "table_package.h"
#include "table_collection.h"
#include "cards_wrapper.h"
#include "progressbox.h"
#include "cards_log.h"

using namespace std;
using namespace cards;


enum widgetID
{
    BTN_SYNC,
    BTN_APPLY,
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
class mainWindow : public Fl_Double_Window, public CEventHandler
{
public:
    /**
     * \brief Constructor
     *
     * Constructor of mainWindow class
     *
     */
    mainWindow(bool pInstaller=false);

    virtual ~mainWindow();
    void LoadConfig();

protected:
//Fltk callbacks
    static void onWindowEvent(Fl_Widget* pWidget,long pID);

//Cards Wrapper callbacks
    void OnSyncFinished(const CEH_RC rc);
    void OnJobListChange(const CEH_RC rc);
    void OnLogMessage(const string& pMessage);
    void OnPackageInfo(CPackage& pPackage);

private:
    void SaveConfig();
    TableBase* _tablePackages;
    TableBase* _tableCollections;
    //PackList* _packList;
    Fl_Input* _search;
    Fl_Text_Display* _console;
    Fl_Text_Display* _info;
    Fl_Text_Buffer * _consoleBuff;
    Fl_Text_Buffer * _infoBuff;
    Fl_Button* _btnSync;
    Fl_Button* _btnApply;
    Fl_Group* _grpPackage;
    Fl_Group* _grpCollection;
    Fl_Tabs* _tabs;
    CWrapper* _cards;
    Fl_Preferences* _config;
    CLogger* _log;
};

#endif // MAINWINDOW_H
