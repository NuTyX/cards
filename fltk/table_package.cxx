/*
 * table_package.cxx
 *
 * Copyright 2015 - 2017 Thierry Nuttens <tnut@nutyx.org>
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

#include "table_package.h"

/// Constructor

TablePackage::TablePackage(int x, int y, int w, int h, const char *l)
    : TableBase(x,y,w,h,l)
{
    colTitle.push_back("");
    colTitle.push_back("Collection");
    colTitle.push_back("Name");
    colTitle.push_back("Description");
    _cards->refreshPackageList();
}

/// Refresh Table
void TablePackage::refresh_table()
{
    clear();
    _rowdata.clear();
    cols(4);
    vector<CPackage*> pkgList = _cards->getPackageList();
    for (auto S : pkgList)
    {
        if (_filter.length()>0)
            if ((S->getName().find(_filter)==string::npos) &&
                (S->getSetList().find(_filter)==string::npos) &&
                (S->getDescription().find(_filter)==string::npos) ) continue;
        // Add a new row
        Row newrow;
        newrow.data=S;
        if(S->isInstalled())
        {
            newrow.cols.push_back("I");
        }
        else newrow.cols.push_back("U");
        newrow.cols.push_back(S->getSetList());
        newrow.cols.push_back(S->getName());
        newrow.cols.push_back(S->getDescription());
        _rowdata.push_back(newrow);
    }
    // How many rows we loaded
    rows((int)_rowdata.size());
    // Auto-calculate widths, with 20 pixel padding
    autowidth(40);
}

void TablePackage::OnDrawCell(TableContext context, int R, int C, int X, int Y, int W, int H)
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
                if (C !=0 )
                {
                    fl_font(FL_HELVETICA, 16);
                    fl_color(FL_BLACK);
                    fl_draw(s.c_str(), X+2,Y,W,H, FL_ALIGN_LEFT);  //  +2= pad left
                }
                else if (_rowdata[R].data!=nullptr)
                {
                    CPackage* pack=reinterpret_cast<CPackage*>(_rowdata[R].data);
                    if (pack->isToBeInstalled())
                    {
                        fl_draw_pixmap(download_xpm,X+5,Y);
                    }
                    else if (pack->isToBeRemoved())
                    {
                        fl_draw_pixmap(deleted_xpm,X+5,Y);
                    }
                    else if (pack->isInstalled())
                    {
                        fl_draw_pixmap(checked_xpm,X+5,Y);
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

void TablePackage::OnEvent(TableContext context, int pCol, int pRow)
{
    switch ( context )
    {
        case CONTEXT_CELL:
        {
            if (_rowdata[pRow].data!=nullptr)
            {
                CPackage* pack = reinterpret_cast<CPackage*>(_rowdata[pRow].data);
                if ( Fl::event() == FL_RELEASE && Fl::event_button() == 3 )
                {
                    Fl_Menu_Item rclick_menu[] = {
                        { "Install" },
                        { "Remove" },
                        { "Cancel" },
                        { 0 }
                    };
#ifndef NDEBUG
                    cout << pack->getName() << " : " << (pack->isToBeInstalled() ? "To install" : "Nothing" ) << endl;
                    cout << pack->getName() << " : " << (pack->isToBeRemoved() ? "To remove" : "Nothing" ) << endl;
#endif // NDEBUG
                    if (pack->isInstalled())
                    {
                        rclick_menu[0].deactivate();
                        if (pack->isToBeRemoved())
                        {
                            rclick_menu[1].deactivate();
                        }
                        else rclick_menu[2].deactivate();
                    }
                    if (!pack->isInstalled())
                    {
                        rclick_menu[1].deactivate();
                        if (pack->isToBeInstalled())
                        {
                            rclick_menu[0].deactivate();
                        }
                        else rclick_menu[2].deactivate();
                    }
                    const Fl_Menu_Item *m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
                    if ( !m )
                    {
                        return;
                    }
                    else if ( strcmp(m->label(), "Install") == 0 )
                    {
                        pack->setStatus(TO_INSTALL);
                        _cards->refreshJobList();
#ifndef NDEBUG
                        cout << pack->getName() << " : " << (pack->isToBeInstalled() ? "To install" : "Nothing" ) << endl;
#endif // NDEBUG
                    }
                    else if ( strcmp(m->label(), "Remove") == 0 )
                    {
                        if (pack->isInstalled())
                        {
                            pack->setStatus(TO_REMOVE);
                            _cards->refreshJobList();
#ifndef NDEBUG
                            cout << pack->getName() << " : " << (pack->isToBeRemoved() ? "To remove" : "Nothing" ) << endl;
#endif // NDEBUG
                        }
                    }
                    else if ( strcmp(m->label(), "Cancel") == 0 )
                    {
                        if (pack->isToBeInstalled()) pack->unSetStatus(TO_INSTALL);
                        if (pack->isToBeRemoved()) pack->unSetStatus(TO_REMOVE);
                        _cards->refreshJobList();
                    }
                }
                _cards->getPackageInfo(pack->getName());
            }
            select_row(pRow);
            break;
        }
    }
}

