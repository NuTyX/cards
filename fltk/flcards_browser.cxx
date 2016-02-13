/*
 * flcards_browser.cxx
 * 
 * Copyright 2016 Thierry Nuttens <tnut@nutyx.org>
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

#include <FL/Fl.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

#include <FL/fl_draw.H>
#include "pixmaps/flcards.xpm"
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>


#include "flcards_info.h"

#define MAX_COLS 4
#define MAX_ROWS 20
#define COLONNE_HEADER { "STATUS", "NAME", "VERSION", "DESCRIPTION" }
class Row {
public:
        std::vector<char*> cols;
};

class Flcards_browser : public Fl_Table_Row
{
	std::vector<Row> _rowdata;

	void autowidth(int pad)
	{
		fl_font(FL_COURIER, 16);
		// initialize all column widhths to lower value
		for (int c = 0;c<cols();c++) col_width(c,pad);
		for (int r=0; r<(int)_rowdata.size();r++)
		{
			int w=0;
			int h=0;
			for  ( int c=0; c<(int)_rowdata[r].cols.size();c++)
			{
				if (c==0)
                                col_width(c, 40);
				if (c==1)
				col_width(c, 180);
				if (c==2)
				col_width(c,90);
				if (c==3)
				col_width(c,1100);
			}
		}
		table_resized();
		redraw();
	}
	void DrawHeader(const char *s, int X, int Y, int W, int H)
	{
		fl_push_clip(X,Y,W,H);
		fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
		fl_color(FL_BLACK);
		fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	void draw_cell(TableContext context, int ROW=0, int COL=0, int X=0, int Y=0, int W=0, int H=0)
	{
		static const	char *s ="";
		static char sS[40];
		if ( ROW < (int)_rowdata.size() && COL < (int)_rowdata[ROW].cols.size() )
			s = _rowdata[ROW].cols[COL];
		switch ( context )
		{
			case CONTEXT_COL_HEADER:
				fl_push_clip(X,Y,W,H);
				static const char *head[] = COLONNE_HEADER;
				if ( ROW < 1 )
				{
					fl_font(FL_HELVETICA_BOLD, 12);
					fl_color(FL_WHITE);
					fl_draw(head[COL], X+2,Y,W,H, FL_ALIGN_LEFT, 0, 0);
				}
				fl_pop_clip();
				return;
			case CONTEXT_CELL:
			{
				int fgcol = FL_BLACK;
				int bgcol = FL_WHITE;
				if ( row_selected(ROW) )
				{
					fgcol = FL_WHITE;
					bgcol = 0xaa4444;
				}
				fl_font(FL_HELVETICA, 12);
				fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, bgcol);
				fl_color(fgcol);
				if (COL>0)
					fl_draw(s, X+4,Y,W,H, FL_ALIGN_LEFT);
				else
				{
					if ( ! strcmp(s,"0"))
						fl_draw("", X+4,Y,W,H, FL_ALIGN_LEFT);
					if ( ! strcmp(s,"1"))
						fl_draw("I", X+4,Y,W,H, FL_ALIGN_LEFT);
					if ( ! strcmp(s,"2"))
						fl_draw("U", X+4,Y,W,H, FL_ALIGN_LEFT);
				}
				fl_color(FL_LIGHT2);
				fl_rect(X,Y,W,H);
			}
			default:
				return;
		}
	}
public:
	
	// Constructor
	Flcards_browser(int X, int Y, int W, int H, const char *L=0) : Fl_Table_Row(X,Y,W,H,L)
	{
		// Rows
		rows(MAX_ROWS);             // how many rows
		row_header(1);              // enable row headers (along left)
		row_height_all(20);         // default height of rows
		row_resize(1);              // disable row resizing
		// Colls
		cols(MAX_COLS);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(1000);          // default width of columns
		col_resize(1);              // enable column resizing
		end();
	}
	~Flcards_browser() { } // Do nothing special
        void load_table()
        {
                cols(0);
		Flcards_info flcards_info("/etc/cards.conf");
		std::set<string> RowsColumns = flcards_info.getListOfInstalledPackages();
		int r=0;
		for (auto S : RowsColumns ) {
			char* s = new char[S.size()+1];
			copy(S.begin(),S.end(),s);
			s[S.size()]='\0';
                        // Add a new row
                        Row newrow; _rowdata.push_back(newrow);
                        std::vector<char*> &rc = _rowdata[r].cols;
                        // Break line into separate word 'columns'
                        char *ss;
                        const char *delim = "\t";
                        for(int t=0; (t==0)?(ss=strtok(s,delim)):(ss=strtok(NULL,delim)); t++)
                        {
                                rc.push_back(strdup(ss));
                        }
                        // Keep track of max # columns
                        if ( (int)rc.size() > cols() )
                        {
                                cols((int)rc.size());
                        }
			delete s;
			r++;
                }
                // How many rows we loaded
                rows((int)_rowdata.size());
                // Auto-calculate widths, with 20 pixel padding
                autowidth(20);
        }
	void clearRows()
	{
		_rowdata.clear();
	}
	const char* getCellText(int r, int c)
	{
		return _rowdata[r].cols[c];
	}
};
Fl_Button *button1 = new Fl_Button(770,630,80,30,"Installer");
Fl_Box *box1 = new Fl_Box(770,630,80,30,"OK");
Fl_Input *input1 = new Fl_Input(50,10,800,20,"Input: ");
Flcards_browser *list1 = new Flcards_browser(50,50,800,400,"Liste:");
Fl_Browser *output1 = new Fl_Browser(50,450,800,160);
void cb_input1()
{
	if (strlen(input1->value())>1)
	{	
		list1->clearRows();
			
		char c[]="pkg-get-list|grep -i ";
		void *vf=malloc(255*sizeof(char));
		char *f=(char*)vf;
		f=strcat(c,input1->value());
		printf("%s\n",f);
		list1->load_table();
		free(vf);
	}
}
void cb_output1()
{
	int ROW = list1->callback_row();
	output1->clear();
//TODO Review this code
/*	box1->hide();
	button1->hide();
	button1->deactivate();
	output1->label(list1->getCellText(ROW,1));
	output1->add(list1->getCellText(ROW,3));
	output1->add("");
	output1->add(list1->getCellText(ROW,4));
	output1->add(list1->getCellText(ROW,5));
	output1->add(list1->getCellText(ROW,6));
	output1->add(list1->getCellText(ROW,7));
	output1->add("Dependencies: ");
	output1->add(list1->getCellText(ROW,8));
	if ( ! strcmp (list1->getCellText(ROW,0), "0"))
	{
		button1->activate();
		button1->show();
		button1->label("Install");
	}
	if ( ! strcmp (list1->getCellText(ROW,0), "1"))
	{
		box1->show();
	}
	if ( ! strcmp (list1->getCellText(ROW,0), "2"))
	{
		button1->activate();
		button1->show();
		button1->label("Update");
	} */
}
int main( int Int, char ** ppc)
{
	Fl_Pixmap* p_win;
	Fl_RGB_Image* p_im;
	p_win = new Fl_Pixmap(flcards_xpm);
	p_im = new Fl_RGB_Image(p_win);

	Fl_Window *win = new Fl_Window(100,100,900,680, "Browser");
	win->icon(p_im);
	win->begin();
		win->add(*input1);
		input1->callback((Fl_Callback*)*cb_input1);
		input1->when(FL_WHEN_CHANGED);
		win->add(*list1);
		list1->load_table();
		list1->callback((Fl_Callback*)*cb_output1);
		list1->when(FL_RELEASE);
		win->add(*output1);
		output1->color(FL_BACKGROUND_COLOR,FL_BACKGROUND_COLOR);
		win->add(*button1);
		button1->hide();
		win->add(*box1);
		box1->hide();
	win->end();
	win->resizable(list1);
	win->show(Int,ppc);
	return Fl::run();
}
