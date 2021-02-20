/*
 * table_collection.cxx
 *
 * Copyright 2015 - 2019 Thierry Nuttens <tnut@nutyx.org>
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2021 Thierry Nuttens <tnut@nutyx.org>
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

#include "table_collection.h"

/// Constructor

TableCollection::TableCollection(int x, int y, int w, int h, const char *l)
    : TableBase(x,y,w,h,l)
{
    colTitle.push_back("");
    colTitle.push_back("Set");
    colTitle.push_back("Name");
    colTitle.push_back("Description");
}

/// Refresh Table
void TableCollection::refresh_table()
{
    clear();
    _rowdata.clear();
    cols(4);
    for (int i=0; i<6 ; i++)
    {
        Row newrow;
        newrow.cols.push_back("U");
        switch (i)
        {
            case 0:
            {
                newrow.cols.push_back("LXDE");
                newrow.cols.push_back("Lxde");
                newrow.cols.push_back("Lightweight desktop environment based on GTK+");
                newrow.data = &lxde_xpm;
                break;
            }
            case 1:
            {
                newrow.cols.push_back("XFCE");
                newrow.cols.push_back("Xfce");
                newrow.cols.push_back("Another lightweight desktop environment based on GTK+");
                newrow.data = &xfce_xpm;
                break;
            }
            case 2:
            {
                newrow.cols.push_back("E24");
                newrow.cols.push_back("Enlightenment");
                newrow.cols.push_back("Lightweight desktop environment based on EFL");
                newrow.data = &E17_xpm;
                break;
            }
            case 3:
            {
                newrow.cols.push_back("MATE");
                newrow.cols.push_back("Mate");
                newrow.cols.push_back("Gnome 2 fork desktop environment based on GTK+");
                newrow.data = &mate_xpm;
                break;
            }
            case 4:
            {
                newrow.cols.push_back("KDE");
                newrow.cols.push_back("Kde");
                newrow.cols.push_back("Heavy powerfull Desktop Environment based on Qt");
                newrow.data = &kde_xpm;
                break;
            }
            case 5:
            {
                newrow.cols.push_back("GNOME");
                newrow.cols.push_back("Gnome");
                newrow.cols.push_back("Famous linux desktop environment based on GTK+");
                newrow.data = &gnome_xpm;
                break;
            }
        }
        _rowdata.push_back(newrow);
    }
    // How many rows we loaded
    rows((int)_rowdata.size());
    row_height_all(50);
    // Auto-calculate widths, with 20 pixel padding
    autowidth(20);
}

void TableCollection::OnDrawCell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
    string s = "";
    if ( (R < (int)_rowdata.size()) && (C < (int)_rowdata[R].cols.size()) )
        s = _rowdata[R].cols[C];
    switch (context)
    {
        case CONTEXT_CELL:
        {
            fl_push_clip(X,Y,W,H);
            {
                //Bg color
                Fl_Color bgcolor = row_selected(R) ? selection_color() : FL_WHITE;
                fl_color(bgcolor);
                fl_rectf(X,Y,W,H);
                switch (C)
                {
                    case 0:
                    {
                        // Installation icon status
                        break;
                    }
                    case 1:
                    {
                        //Collection Icon
                        if (_rowdata[R].data!=nullptr)
                        {
                            char* const* Icon = reinterpret_cast<char* const*>(_rowdata[R].data);
                            fl_draw_pixmap(Icon,X+5,Y);
                        }
                        break;
                    }
                    default:
                    {
                        fl_font(FL_HELVETICA, 16);
                        fl_color(FL_BLACK);
                        fl_draw(s.c_str(), X+2,Y,W,H, FL_ALIGN_LEFT);  //  +2= pad left
                    }
                }
                // Border
                fl_color(FL_LIGHT2);
                fl_rect(X,Y,W,H);
            }
            fl_pop_clip();
            break;
        }
    }
}

void TableCollection::OnEvent(TableContext context, int pCol, int pRow)
{
    switch ( context )
    {
        case CONTEXT_CELL:
        {
            if ( Fl::event() == FL_RELEASE && Fl::event_button() == 3 )
            {
                string Collection = _rowdata[pRow].cols[1];
                CWrapper* Cards = CWrapper::instance();
                set<string> Collec_List;
                if (Collection=="LXDE")
                {
                    Collec_List.insert("lxde");
                    Collec_List.insert("lxdm");
                }
                else if (Collection=="XFCE")
                {
                    Collec_List.insert("xfce4");
                    Collec_List.insert("xfce4-extra");
                    Collec_List.insert("lxdm");
               }
                else if (Collection=="E24")
                {
                    Collec_List.insert("enlightenment");
                    Collec_List.insert("lxdm");
                }
                else if (Collection=="MATE")
                {
                    Collec_List.insert("mate");
                    Collec_List.insert("lightdm");
                }
                else if (Collection=="KDE")
                {
                    Collec_List.insert("plasma5");
                }
                else if (Collection=="GNOME")
                {
                    Collec_List.insert("gnome");
                }
                else break;
                Fl_Menu_Item rclick_menu[] = {
                    { "Install" },
                    { "Remove" },
                    { 0 }
                };
                const Fl_Menu_Item *m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
                if ( !m )
                {
                    break;
                }
                else if ( strcmp(m->label(), "Install") == 0 )
                {
                    vector<CPackage*> Packages = Cards->getPackageList();
                    for (CPackage* Package : Packages)
                    {
                        if ((!Package->isInstalled()))
                        {
							if (Collec_List.count(Package->getSet()))
								Package->setStatus(TO_INSTALL);
							if (Collec_List.count(Package->getName()))
								Package->setStatus(TO_INSTALL);
                        }
                    }
                    Cards->refreshJobList();
                }
                else if ( strcmp(m->label(), "Remove") == 0 )
                {
                    vector<CPackage*> Packages = Cards->getPackageList();
                    for (CPackage* Package : Packages)
                    {
                        if (Package->isInstalled() && (Collec_List.count(Package->getSet())))
                        {
                            Package->setStatus(TO_REMOVE);
                        }
                    }
                    Cards->refreshJobList();
                }
            }
            select_row(pRow);
            break;
        }
    }
}
