/*
 * pack_list.h
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

#ifndef PACK_LIST_H
#define PACK_LIST_H

#include <FL/Fl.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include "pixmaps/lxde.xpm"
#include "pixmaps/xfce.xpm"
#include "pixmaps/lxqt.xpm"
#include "pixmaps/mate.xpm"
#include "pixmaps/kde.xpm"
#include "pixmaps/gnome.xpm"

#include <iostream>

#include "cards_wrapper.h"

using namespace std;
using namespace cards;

enum Collections
{
    LXDE,
    XFCE,
    LXQT,
    MATE,
    GNOME,
    KDE
};

class PackList : public Fl_Pack
{
public:
    PackList (int x, int y, int w, int h);
    ~PackList(){}

protected:
    static void OnClickButton (Fl_Widget* widget, long p);
};

#endif
