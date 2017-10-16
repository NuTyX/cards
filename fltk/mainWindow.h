/*
 * mainWindpow.h
 *
 * Copyright 2017 Thierry Nuttens <tnut@nutyx.org>
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
#include <FL/Fl_Button.H>
#include <string>
#include <vector>

//Project Includes
#include "pixmaps/flcards.xpm"
#include "tableau.h"

using namespace std;




class mainWindow : public Fl_Double_Window
{
    public:
        mainWindow(int W, int H, string Title);
        virtual ~mainWindow();

    protected:
static  void Quitter_CB(Fl_Widget*,void* instance);
static  void Available_Packages_CB(Fl_Widget*,void* instance);
static  void Installed_Packages_CB(Fl_Widget*,void* instance);

    private:
        vector<Fl_Menu_Item> Menu;
        Tableau* m_Tab;
        Fl_Pixmap* p_win;
        Fl_RGB_Image* p_im;
        Fl_Input* Recherche;
        Fl_Menu_Bar* BarMenu;
        Fl_Text_Display* TextDisplay;
        Fl_Button* BtnInstall;
};

#endif // MAINWINDOW_H
