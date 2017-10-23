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

mainWindow::mainWindow(int W=900, int H=900, string Title="Default") :
    Fl_Double_Window(W,H,Title.c_str())
{
	p_win = new Fl_Pixmap(flcards_xpm);
	p_im = new Fl_RGB_Image(p_win);
	icon(p_im);
	Recherche = new Fl_Input(MARGIN+450, MARGIN+5, 400, 30, "Search:");
	Recherche->labelfont(2);
	Recherche->color(FL_WHITE);
	Console = new Fl_Text_Display(MARGIN, 600, w()-MARGIN*2, 300-MARGIN, "Info about the selected package:");
	tbuff = new Fl_Text_Buffer();
	Console->buffer(tbuff);
	Console->color(FL_BLACK);
    Console->textcolor(FL_GRAY);
    Console->labeltype(FL_NO_LABEL );
	BtnSync = new Fl_Button(MARGIN, MARGIN, 100, 40, "SYNC");
	BtnSync->callback((Fl_Callback*)SyncButton_CB);
	m_Tab = new Tableau(MARGIN, MARGIN+50, w()-MARGIN*2, h()-400);
	m_Tab->selection_color(FL_YELLOW);
	m_Tab->col_header(1);
	m_Tab->col_resize(1);
	m_Tab->when(FL_WHEN_RELEASE);
	m_Tab->row_height_all(18);
	m_Tab->tooltip("Click on the header of the column to sort it");
	m_Tab->color(FL_WHITE);
	Cards = Cards_wrapper::instance();
	Cards->suscribeToEvents(this);
    cout << "Carreidas use ";
    Cards->printCardsVersion();
	m_Tab->load_table();
}

mainWindow::~mainWindow()
{
    //todo : wait end of threads
    //Cards->join();
    Cards->unsuscribeToEvents(this);
    Cards->kill();
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

void mainWindow::SyncButton_CB(Fl_Widget*, void* instance)
{
    Cards_wrapper::instance()->sync();
}

void mainWindow::OnLogMessage(const string& Message)
{
	Fl::lock();
	if (tbuff!=nullptr) tbuff->append(Message.c_str());
	Fl::unlock();
}

void mainWindow::OnSyncFinished(const CEH_RC rc)
{
	Fl::lock();
	cout << "Sync : " << Cards_event_handler::getReasonCodeString(rc) << endl;
	Fl::unlock();
}
