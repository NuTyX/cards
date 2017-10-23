/*
 * mainWindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Global Includes
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Button.H>
#include <string>
#include <vector>

//Project Includes
#include "pixmaps/flcards.xpm"
#include "tableau.h"
#include "cards_wrapper.h"

using namespace std;

class mainWindow : public Fl_Double_Window, public Cards_event_handler
{
    public:
        mainWindow(int W, int H, string Title);
        virtual ~mainWindow();

    protected:
static  void Quitter_CB(Fl_Widget*,void* instance);
static  void Available_Packages_CB(Fl_Widget*,void* instance);
static  void Installed_Packages_CB(Fl_Widget*,void* instance);
static	void SyncButton_CB(Fl_Widget*,void* instance);
        void OnLogMessage (const string& Message);
        void OnSyncFinished(const CEH_RC rc);
    private:
        //vector<Fl_Menu_Item> Menu;
        Tableau* m_Tab;
        Fl_Pixmap* p_win;
        Fl_RGB_Image* p_im;
        Fl_Input* Recherche;
        Fl_Menu_Bar* BarMenu;
        Fl_Multiline_Output* Console;
        Fl_Button* BtnSync;
        Cards_wrapper* Cards;
};

#endif // MAINWINDOW_H
