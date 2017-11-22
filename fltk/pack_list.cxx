/*
 * pack_list.cxx
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

#include "pack_list.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include "pixmaps/lxde.xpm"
#include "pixmaps/xfce.xpm"
#include "pixmaps/lxqt.xpm"
#include "pixmaps/mate.xpm"
#include "pixmaps/gnome.xpm"
#include "pixmaps/kde.xpm"

PackList::PackList (int x, int y, int w, int h)
	: Fl_Pack(x,y,w,h)
{
	this->box(FL_DOWN_FRAME);
	this->begin();
	//lxde
	Fl_Button* lxde = new Fl_Button(0,0,72,72,"");
	Fl_Pixmap* lxde_Icon = new Fl_Pixmap(lxde_xpm);
	lxde->tooltip("Lxde : a lightweight desktop environment based on X11");
	lxde->image(lxde_Icon);
	lxde->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
	//Xfce
	Fl_Button* xfce = new Fl_Button(0,0,72,72,"");
	Fl_Pixmap* xfce_Icon = new Fl_Pixmap(xfce_xpm);
	xfce->tooltip("xfce : a lightweight desktop environment based on GTK+");
	xfce->image(xfce_Icon);
	xfce->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	//lxqt
	Fl_Button* lxqt = new Fl_Button(0,0,72,72,"");
	Fl_Pixmap* lxqt_icon = new Fl_Pixmap(lxqt_xpm);
	lxqt->tooltip("lxqt : a lightweight desktop environment based on Qt");
	lxqt->image(lxqt_icon);
	lxqt->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	//Mate
	Fl_Button* Mate = new Fl_Button(0,0,72,72,"");
	Fl_Pixmap* Mate_Icon = new Fl_Pixmap(mate_xpm);
	Mate->tooltip("Mate : a Gnome 2 desktop environment fork, light and functional");
	Mate->image(Mate_Icon);
	Mate->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	//Gnome 3
	Fl_Button* Gnome = new Fl_Button(0,0,72,72,"");
	Fl_Pixmap* Gnome_Icon = new Fl_Pixmap(gnome_xpm);
	Gnome->tooltip("Gnome 3 : The mythic desktop environment");
	Gnome->image(Gnome_Icon);
	Gnome->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);

	//KDE
	Fl_Button* KDE = new Fl_Button(0,0,72,72,"");
	Fl_Pixmap* KDE_Icon = new Fl_Pixmap(kde_xpm);
	KDE->tooltip("KDE : a heavy powerfull Desktop Environment");
	KDE->image(KDE_Icon);
	KDE->align(FL_ALIGN_IMAGE_NEXT_TO_TEXT);
	this->end();
}
