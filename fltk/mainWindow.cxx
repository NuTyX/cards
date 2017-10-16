/*
 * mainWindpow.cxx
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

#include "mainWindow.h"

mainWindow::mainWindow(int W=900, int H=900, string Title="Default") :
    Fl_Double_Window(W,H,Title.c_str())
{
    Menu.push_back((Fl_Menu_Item){"Actions", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){"Synchronize", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){"Quit", 0,  (Fl_Callback*)Quitter_CB, 0, 0, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){"Information", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){"Available Packages", 0,  (Fl_Callback*)Available_Packages_CB, 0, 0, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){"Installed Packages", 0,  (Fl_Callback*)Installed_Packages_CB, 0, 0, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){0,0,0,0,0,0,0,0,0});
    Menu.push_back((Fl_Menu_Item){"About", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0});
    Menu.push_back((Fl_Menu_Item){0,0,0,0,0,0,0,0,0});
    Menu.push_back((Fl_Menu_Item){0,0,0,0,0,0,0,0,0});

	p_win = new Fl_Pixmap(flcards_xpm);
	p_im = new Fl_RGB_Image(p_win);
	icon(p_im);
	Recherche = new Fl_Input(MARGIN+80, MARGIN, 499, 24, "Search:");
	Recherche->labelfont(2);
	BarMenu = new Fl_Menu_Bar(0, 0, 900, 20);
	BarMenu->menu(&Menu[0]);
	TextDisplay = new Fl_Text_Display(MARGIN, 600, w()-MARGIN*2, 300-MARGIN, "Info about the selected package:");
	BtnInstall = new Fl_Button(760, 820, 100, 40, "INSTALL");
	m_Tab = new Tableau(MARGIN, MARGIN+40, w()-MARGIN*2, h()-400);
	m_Tab->selection_color(FL_YELLOW);
	m_Tab->col_header(1);
	m_Tab->col_resize(1);
	m_Tab->when(FL_WHEN_RELEASE);
	m_Tab->load_table();
	m_Tab->row_height_all(18);
	m_Tab->tooltip("Click on the header of the column to sort it");
	m_Tab->color(FL_WHITE);
}

mainWindow::~mainWindow()
{
    //dtor
}

void mainWindow::Quitter_CB(Fl_Widget*,void* instance)
{
  	exit(0);
}

void mainWindow::Available_Packages_CB(Fl_Widget*, void* instance)
{

}

void mainWindow::Installed_Packages_CB(Fl_Widget*, void* instance)
{

}


void Selectionner_CB(Fl_Widget *W, void *v)
{
	 fl_alert("callback receive!");
}
