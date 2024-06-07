/*
 * table_base.cxx
 *
 * Copyright 2015 - 2018 Thierry Nuttens <tnut@nutyx.org>
 * Copyright 2017 - 2018 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2018 - 2023 Thierry Nuttens <tnut@nutyx.org>
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

#include "table_base.h"

using namespace cards;

sort_column::sort_column (int col, int reverse)
{
    m_col = col;
    m_reverse= reverse;
}

bool sort_column::operator() (const Row &a, const Row &b)
{
    const char *ap = ( m_col < (int)a.cols.size() ) ? a.cols[m_col].c_str() : "",
    *bp = ( m_col < (int)b.cols.size() ) ? b.cols[m_col].c_str() : "";
    if ( isdigit(*ap) && isdigit(*bp) )
    {
        int av=0; sscanf(ap, "%d", &av);
        int bv=0; sscanf(bp, "%d", &bv);
        return( m_reverse ? av < bv : bv < av );
    }
    else
    {
        return( m_reverse ? strcmp(ap, bp) > 0 : strcmp(ap, bp) < 0 );
    }
}

/// Constructor

table_base::table_base(int x, int y, int w, int h, const char *l)
    : Fl_Table_Row(x,y,w,h,l)
{
    type(SELECT_MULTI);
    m_sort_reverse = 0;
    m_sort_lastcol = -1;
    end();
    callback(event_callback, reinterpret_cast<void*>(this));
    selection_color(FL_YELLOW);
    when(FL_WHEN_RELEASE|FL_WHEN_CHANGED);
    cols(6);
    col_header(1);
    col_header_height(25);
    col_resize(1);
    col_width_all(20);
    row_height_all(18);
    tooltip("Click on the header of the column to sort it");
    color(FL_WHITE);
    selection_color(FL_YELLOW);
    m_cards=cards_wrapper::instance();
    m_cards->subscribeToEvents(this);
    resizable (this);
}

/// Sort a column up or down
void table_base::sortColumn(int col, int reverse)
{
	 // FIXME find out why std prefix is not needed here
    std::sort(m_rowdata.begin(), m_rowdata.end(), sort_column(col, reverse));
    redraw();
}

/// Draw sort arrow
void table_base::drawSortArrow(int X, int Y, int W, int H)
{
    int xlft = X+(W-6)-8;
    int xctr = X+(W-6)-4;
    int xrit = X+(W-6)-0;
    int ytop = Y+(H/2)-4;
    int ybot = Y+(H/2)+4;
    if ( m_sort_reverse )
    {
        // Engraved down arrow
        fl_color(FL_WHITE);
        fl_line(xrit, ytop, xctr, ybot);
        fl_color(41);
        fl_line(xlft, ytop, xrit, ytop);
        fl_line(xlft, ytop, xctr, ybot);
    }
    else
    {
        // Engraved up arrow
        fl_color(FL_WHITE);
        fl_line(xrit, ybot, xctr, ytop);
        fl_line(xrit, ybot, xlft, ybot);
        fl_color(41);                   // dark gray
        fl_line(xlft, ybot, xctr, ytop);
    }
}

/// Handle drawing all cells in table
void table_base::drawCell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
    switch ( context )
    {
        case CONTEXT_COL_HEADER:
            fl_push_clip(X,Y,W,H);
            {
                fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, FL_BACKGROUND_COLOR);
                if ( C < colTitle.size() )
                {
                    fl_font(FL_HELVETICA_BOLD, 16);
                    fl_color(FL_BLACK);
                    fl_draw(colTitle[C].c_str(), X+2,Y,W,H, FL_ALIGN_CENTER, 0, 0); // +2=pad left
                    if ( C == m_sort_lastcol )
                    {
                        drawSortArrow(X,Y,W,H);
                    }
                }
            }
            fl_pop_clip();
            break;
    }
    OnDrawCell(context, R, C, X, Y, W, H);
}

/// Automatically set columns widths to the longuest string.
void table_base::autowidth(int pad)
{
    fl_font(FL_HELVETICA, 17);
    // initialize all column widths to pad value
    int c;
//    for (c = 0;c<cols();c++)
//		col_width(c,pad);
	int n=0;
	int w1 = 70;
	int w2 = 80;
	int w3 = 100;
	int w4 = 80;
	int w5 = 80;
    for (auto r : m_rowdata)
    {
		n++;
        int w,h;
        for  ( c=1; c<r.cols.size();c++)
        {
			// get pixel width of the text
			w=fl_width(r.cols[c].c_str(),r.cols[c].size() );
//			cout << n << " " << c << " " << w << endl;
			switch (c)
			{
				case 1:  // name
					if ( w > w1 )
						w1=w;
					continue;

				case 2: //  version
					if ( w2 < w )
						w2=w;
					continue;

				case 3: //description
					if ( w3 < w )
						w3=w;
					continue;

				case 4: //set
					if ( w4 < w )
						w4=w;
					continue;

				case 5: //packager
					if ( w5 < w )
						w5=w;
				continue;
			}
        }
    }
    col_width(0, 25);
    col_width(1, w1);
    col_width(2, w2);
    col_width(3, w3);
    col_width(4, w4);
    col_width(5, w5);

 //   table_resized();
}

/// Resize parent widows to size of tableau
void table_base::resizeWindow()
{
    // Determine exact outer width of tableau with all columns visible
    int width = 4;	// width of table borders
    for ( int t=0; t<cols();t++) width += col_width(t);  // width = total of all columns
    width += MARGIN*2;
    if ( width < 200 || width > Fl::w() ) return;
    window()->resize(window()->x(), window()->y(), width, window()->h()); // resize window to fit
}

/// Callback whenever someone clicks on different parts of the table
void table_base::event_callback(Fl_Widget*, void *data)
{
    if (data != nullptr)
        ((table_base*)data)->event_callback2();
}

void table_base::event_callback2()
{
    int COL = callback_col();
    int ROW = callback_row();
    TableContext context = callback_context();
    switch ( context )
    {
        case CONTEXT_COL_HEADER:
        {              // someone clicked on column header
            if ( Fl::event() == FL_RELEASE && Fl::event_button() == 1 )
            {
                        if ( m_sort_lastcol == COL )
                {// Click same column? Toggle sort
                    m_sort_reverse ^= 1;
                        }
                else
                {// Click diff column? Up sort
                    m_sort_reverse = 0;
                }
                sort_column(COL, m_sort_reverse);
                m_sort_lastcol = COL;
            }
            break;
        }
    }
    OnEvent(context,COL, ROW);
}

/// Redefine filter and refresh the tab
void table_base::setFilter(const std::string& pValue)
{
    m_filter=pValue;
    refreshTable();
}

/// Callback

void table_base::OnJobListChange(const CEH_RC rc)
{
    refreshTable();
    redraw();
}

void table_base::OnRefreshPackageFinished (const CEH_RC rc)
{
    Fl::lock();
    if (rc == CEH_RC::OK) refreshTable();
    Fl::unlock();
}

void table_base::OnDoJobListFinished(const CEH_RC rc)
{
    redraw();
}

int table_base::getCntRowSelected()
{
    int cnt=0;
    for (int i=0; i< rows(); i++)
    {
        if (row_selected(i))
        {
            cnt++;
        }
    }
    return cnt;
}
