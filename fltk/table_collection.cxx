/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2015 - 2018 Thierry Nuttens <tnut@nutyx.org>
// Copyright 2017 - 2018 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2018 - .... Thierry Nuttens <tnut@nutyx.org>

#include "table_collection.h"

using namespace cards;

/// Constructor

table_collection::table_collection(int x, int y, int w, int h, const char *l)
    : table_base(x,y,w,h,l)
{
    type(SELECT_SINGLE);
    colTitle.push_back("");
    colTitle.push_back("Set");
    colTitle.push_back("Name");
    colTitle.push_back("Description");
}

/// Refresh Table
void table_collection::refresh_table()
{
    clear();
    m_rowdata.clear();
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
        m_rowdata.push_back(newrow);
    }
    // How many rows we loaded
    rows((int)m_rowdata.size());
    row_height_all(50);
    // Auto-calculate widths, with 20 pixel padding
    autowidth(20);
}

int table_collection::install_selected()
{
    return 0;
}

int table_collection::remove_selected()
{
    return 0;
}

void table_collection::OnDrawCell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
    std::string s = "";
    if ( (R < (int)m_rowdata.size()) && (C < (int)m_rowdata[R].cols.size()) )
        s = m_rowdata[R].cols[C];
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
                        if (m_rowdata[R].data!=nullptr)
                        {
                            char* const* Icon = reinterpret_cast<char* const*>(m_rowdata[R].data);
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

void table_collection::OnEvent(TableContext context, int pCol, int pRow)
{
    switch ( context )
    {
        case CONTEXT_CELL:
        {
            if ( Fl::event() == FL_RELEASE && Fl::event_button() == 3 )
            {
                std::string Collection = m_rowdata[pRow].cols[1];
                cards_wrapper* Cards = cards_wrapper::instance();
                std::set<std::string> Collec_List;
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
                    std::vector<cards::cache*> Packages = Cards->getPackageList();
                    for (cards::cache* Package : Packages)
                    {
                        if ((!Package->installed()))
                        {
							if (Collec_List.count(Package->collection()))
								Package->setStatus(STATUS_ENUM_TO_INSTALL);
							if (Collec_List.count(Package->name()))
								Package->setStatus(STATUS_ENUM_TO_INSTALL);
                        }
                    }
                    Cards->refreshJobList();
                }
                else if ( strcmp(m->label(), "Remove") == 0 )
                {
                    std::vector<cards::cache*> Packages = Cards->getPackageList();
                    for (cards::cache* Package : Packages)
                    {
                        if (Package->installed() && (Collec_List.count(Package->collection())))
                        {
                            Package->setStatus(STATUS_ENUM_TO_REMOVE);
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
