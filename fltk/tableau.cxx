/*
 * tableau.cxx
 *
 * Copyright 2015-2017 Thierry Nuttens <tnut@nutyx.org>
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

#include "tableau.h"

SortColumn::SortColumn (int col, int reverse)
{
	_col = col;
	_reverse= reverse;
}

bool SortColumn::operator() (const Row &a, const Row &b)
{
	const char *ap = ( _col < (int)a.cols.size() ) ? a.cols[_col] : "",
	*bp = ( _col < (int)b.cols.size() ) ? b.cols[_col] : "";
	if ( isdigit(*ap) && isdigit(*bp) )
	{
		int av=0; sscanf(ap, "%d", &av);
		int bv=0; sscanf(bp, "%d", &bv);
		return( _reverse ? av < bv : bv < av );
	}
	else
	{
		return( _reverse ? strcmp(ap, bp) > 0 : strcmp(ap, bp) < 0 );
	}
}


Tableau::Tableau(int x, int y, int w, int h, const char *l)
	: Fl_Table_Row(x,y,w,h,l)
{
	_sort_reverse = 0;
	_sort_lastcol = -1;
	end();
	callback(event_callback, (void*)this);
	selection_color(FL_YELLOW);
	when(FL_WHEN_RELEASE|FL_WHEN_CHANGED);
	cols(4);
	col_header(1);
	col_header_height(25);
	col_resize(1);
	col_width_all(80);
	when(FL_WHEN_RELEASE);
	row_height_all(18);
	tooltip("Click on the header of the column to sort it");
	color(FL_WHITE);
	_cards=Cards_wrapper::instance();
	_cards->suscribeToEvents(this);
	_cards->refreshPackageList();
}

// Sort a column up or down
void Tableau::sort_column(int col, int reverse)
{
	sort(_rowdata.begin(), _rowdata.end(), SortColumn(col, reverse));
	redraw();
}

// Draw sort arrow
void Tableau::draw_sort_arrow(int X, int Y, int W, int H)
{
	int xlft = X+(W-6)-8;
	int xctr = X+(W-6)-4;
	int xrit = X+(W-6)-0;
	int ytop = Y+(H/2)-4;
	int ybot = Y+(H/2)+4;
	if ( _sort_reverse )
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
void Tableau::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
	const char *s = "";
	if ( R < (int)_rowdata.size() && C < (int)_rowdata[R].cols.size() )
		s = _rowdata[R].cols[C];
	switch ( context )
	{
		case CONTEXT_COL_HEADER:
			fl_push_clip(X,Y,W,H);
			{
				static const char *head[] = COLHEADER;
				fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, FL_BACKGROUND_COLOR);
				if ( C < 9 )
				{
					fl_font(FL_HELVETICA_BOLD, 16);
					fl_color(FL_BLACK);
					fl_draw(head[C], X+2,Y,W,H, FL_ALIGN_LEFT, 0, 0); // +2=pad left
					if ( C == _sort_lastcol )
					{
						draw_sort_arrow(X,Y,W,H);
					}
				}
			}
			fl_pop_clip();
			return;
		case CONTEXT_CELL:
		{
			fl_push_clip(X,Y,W,H);
			{
				//Bg color
				Fl_Color bgcolor = row_selected(R) ? selection_color() : FL_WHITE;
				fl_color(bgcolor);
				fl_rectf(X,Y,W,H);
				fl_font(FL_HELVETICA, 16);
				fl_color(FL_BLACK);
				fl_draw(s, X+2,Y,W,H, FL_ALIGN_LEFT);  //  +2= pad left
				// Border
				fl_color(FL_LIGHT2);
				fl_rect(X,Y,W,H);
			}
			fl_pop_clip();
			return;
		}
		default:
			return;
	}
}

/// Automatically set columns widths to the longuest string.
void Tableau::autowidth(int pad)
{
	fl_font(FL_COURIER, 16);
	// initialize all column widhths to lower value
	for (int c = 0;c<cols();c++) col_width(c,pad);
	for (int r=0; r<(int)_rowdata.size();r++)
	{
		int w,h;
		for  ( int c=0; c<(int)_rowdata[r].cols.size();c++)
		{
			fl_measure(_rowdata[r].cols[c], w, h, 0);	// get pixel width of the text
			if ( (w+pad) > col_width(c) ) col_width(c, w + pad);
		}
	}
	table_resized();
	//redraw();
}

// Resze parent widows to size of tableau
void Tableau::resize_window()
{
	// Determine exact outer width of tableau with all columns visible
	int width = 4;	// width of table borders
	for ( int t=0; t<cols();t++) width += col_width(t);  // width = total of all columns
	width += MARGIN*2;
	if ( width < 200 || width > Fl::w() ) return;
	window()->resize(window()->x(), window()->y(), width, window()->h()); // resize window to fit
}

/* TODO
 *  Load table with list of packages
 * to be done
 * Should not be a big deal, we play with a vector<char*>
 */
void Tableau::refresh_table()
{
    //cols(0);
    vector<Cards_package*> pkgList = _cards->getPackageList();
    int r = 0;
    for (auto S : pkgList)
	{
		// Add a new row
		Row newrow;
		char* name = new char[S->getName().length()+1];
		strcpy(name,S->getName().c_str());
		newrow.cols.push_back(name);

		char* description = new char[S->getDescription().length()+1];
		strcpy(description,S->getDescription().c_str());
		newrow.cols.push_back(description);

		char* version = new char[S->getVersion().length()+1];
		strcpy(version,S->getVersion().c_str());
		newrow.cols.push_back(version);

		if (S->isInstalled()) newrow.cols.push_back("Installed");

		// Keep track of max # columns
		if ( (int)newrow.cols.size() > cols() )
		{
			cols((int)newrow.cols.size());
		}
		_rowdata.push_back(newrow);
		r++;
    }
	// How many rows we loaded
	rows((int)_rowdata.size());
	// Auto-calculate widths, with 20 pixel padding
	autowidth(20);
}

void Tableau::OnRefreshPackageFinished (const CEH_RC rc)
{
	Fl::lock();
	if (rc == CEH_RC::OK) refresh_table();
	Fl::unlock();
}

// Callback whenever someone clicks on different parts of the table
void Tableau::event_callback(Fl_Widget*, void *data)
{
	if (data != nullptr)
		((Tableau*)data)->event_callback2();
}

void Tableau::event_callback2()
{
	// int ROW = callback_row();                 // unused

	int COL = callback_col();
	TableContext context = callback_context();
	switch ( context )
	{
        case CONTEXT_COL_HEADER:
		{              // someone clicked on column header
			if ( Fl::event() == FL_RELEASE && Fl::event_button() == 1 )
			{
                		if ( _sort_lastcol == COL )
				{	// Click same column? Toggle sort
					_sort_reverse ^= 1;
                		}
				else
				{	// Click diff column? Up sort
					_sort_reverse = 0;
				}
				sort_column(COL, _sort_reverse);
				_sort_lastcol = COL;
			}
			break;
		}
		default:
			return;
	}

}
