/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2015 - 2018 Thierry Nuttens <tnut@nutyx.org>
// Copyright 2017 - 2018 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2018 - .... Thierry Nuttens <tnut@nutyx.org>


#include "table_package.h"

/// Constructor

table_package::table_package(int x, int y, int w, int h, const char *l)
    : table_base(x,y,w,h,l)
{
    colTitle.push_back("");
    colTitle.push_back("Name");
    colTitle.push_back("Version");
    colTitle.push_back("Description");
    colTitle.push_back("Set");
    colTitle.push_back("Packager");
    m_cards->refreshPackageList();
}

/// Refresh Table
void table_package::refreshTable()
{
    clear();
    m_rowdata.clear();
    cols(4);
    std::vector<cards::cache*> pkgList = m_cards->getPackageList();
    for (auto S : pkgList)
    {
        if (m_filter.length()>0)
            if ((S->name().find(m_filter) == std::string::npos) &&
                (S->collection().find(m_filter) == std::string::npos) &&
                (S->description().find(m_filter) == std::string::npos) &&
                (S->version().find(m_filter) == std::string::npos) ) continue;
        // Add a new row
        Row newrow;
        newrow.data=S;
        if (S->toinstall())
            newrow.cols.push_back("A");
        else if (S->toremove())
            newrow.cols.push_back("B");
        else if(S->installed())
            newrow.cols.push_back("C");
        else newrow.cols.push_back("D");
        newrow.cols.push_back(S->name());
        newrow.cols.push_back(S->version());
        newrow.cols.push_back(S->description());
        newrow.cols.push_back(S->collection());
        newrow.cols.push_back(S->packager());

        m_rowdata.push_back(newrow);
    }
    // How many rows we loaded
    rows((int)m_rowdata.size());
    // Auto-calculate widths, with 50 pixel as a start
    autowidth(50);
}

int table_package::install_selected()
{
    int cnt=0;
    m_cards->clearJobList();
    for (int i =0; i < rows(); i++)
    {
        if (row_selected(i))
            {
                if (m_rowdata[i].data!=nullptr)
                {
                    cards::cache* pack = reinterpret_cast<cards::cache*>(m_rowdata[i].data);
                    if (!pack->status()==cards::STATUS_ENUM_INSTALLED)
                    {
                        pack->setStatus(cards::STATUS_ENUM_TO_INSTALL);
                        cnt++;
                    }
                }
        }
    }
    m_cards->refreshJobList();
    return cnt;
}
int table_package::remove_selected()
{
        int cnt=0;
        m_cards->clearJobList();
        for (int i =0; i < rows(); i++)
        {
            if (row_selected(i))
            {
                if (m_rowdata[i].data!=nullptr)
                {
                    cards::cache* pack = reinterpret_cast<cards::cache*>(m_rowdata[i].data);
                    if (!pack->status()!=cards::STATUS_ENUM_INSTALLED)
                    {
                        pack->setStatus(cards::STATUS_ENUM_TO_REMOVE);
                        cnt++;
                    }
                }
            }
        }
        m_cards->refreshJobList();
        return cnt;
}
void table_package::OnDrawCell(TableContext context, int R, int C, int X, int Y, int W, int H)
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
                if (C !=0 )
                {
                    fl_font(FL_HELVETICA, 16);
                    fl_color(FL_BLACK);
                    fl_draw(s.c_str(), X+2,Y,W,H, FL_ALIGN_LEFT);  //  +2= pad left
                }
                else if (m_rowdata[R].data!=nullptr)
                {
                    cards::cache* pack=reinterpret_cast<cards::cache*>(m_rowdata[R].data);
                    if (pack->installed())
                    {
                        fl_draw_pixmap(download_xpm,X+5,Y+5);
                    }
                    if (pack->toremove())
                    {
                        fl_draw_pixmap(deleted_xpm,X+5,Y+5);
                    }
                    if (pack->toinstall())
                    {
                        fl_draw_pixmap(checked_xpm,X+5,Y+5);
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

void table_package::OnEvent(TableContext context, int pCol, int pRow)
{
    switch ( context )
    {
        case CONTEXT_CELL:
        {
            if (m_rowdata[pRow].data!=nullptr)
            {
                cards::cache* pack = reinterpret_cast<cards::cache*>(m_rowdata[pRow].data);
                if ( Fl::event() == FL_RELEASE && Fl::event_button() == 1 )
                {
#ifndef NDEBUG
                    std::cout << pack->name()
                        << " : "
                        << (!pack->installed() ? "To install" : "Nothing" )
                        << std::endl;
                    std::cout << pack->name()
                        << " : "
                        << (pack->installed() ? "To remove" : "Nothing" )
                        << std::endl;
#endif // NDEBUG
                    const Fl_Menu_Item *m;
					if (pack->installed() && !pack->toremove()) {
						Fl_Menu_Item rclick_menu[] = {
							{ "Remove" },
							{ 0 }
						};
						m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
					}
					if (!pack->installed() && !pack->toinstall()) {
						Fl_Menu_Item rclick_menu[] = {
							{ "Install" },
							{ 0 }
						};
						m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
					}
					if (pack->toremove()) {
						Fl_Menu_Item rclick_menu[] = {
							{ "Cancel Remove" },
							{ 0 }
						};
						m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
					}
					if (pack->toinstall()) {
						Fl_Menu_Item rclick_menu[] = {
							{ "Cancel Install" },
							{ 0 }
						};
						m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
					}

                    if ( !m )
                    {
                        return;
                    }
                    else if ( strcmp(m->label(), "Install") == 0 )
                    {
                        pack->setStatus(cards::STATUS_ENUM_TO_INSTALL);
                        m_cards->refreshJobList();
#ifndef NDEBUG
                        std::cout << pack->name()
                            << " : "
                            << (pack->toinstall() ? "To install" : "Nothing" )
                            << std::endl;
#endif // NDEBUG
                    }
                    else if ( strcmp(m->label(), "Remove") == 0 )
                    {
                        if (pack->installed())
                        {
                            pack->setStatus(cards::STATUS_ENUM_TO_REMOVE);
                            m_cards->refreshJobList();
#ifndef NDEBUG
                            std::cout << pack->name()
                                << " : "
                                << (pack->toremove() ? "To remove" : "Nothing" )
                                << std::endl;
#endif // NDEBUG
                        }
                    }
                    else if ( ( strcmp(m->label(), "Cancel Install") == 0 ) ||
								( strcmp(m->label(), "Cancel Remove") == 0 ) )
                    {
                        if (pack->toinstall()) pack->unsetStatus(cards::STATUS_ENUM_TO_INSTALL);
                        if (pack->toremove()) pack->unsetStatus(cards::STATUS_ENUM_TO_REMOVE);
                        m_cards->refreshJobList();
                    }
                }
                m_cards->getPackageInfo(pack->name());
            }
            select_row(pRow);
            break;
        }
    }
}
