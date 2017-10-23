/*
 * tableau.h
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

#ifndef TABLEAU_H
#define TABLEAU_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>

#include <ctype.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Table_Row.H>
#include "cards_wrapper.h"

#define MARGIN 20
#define COLHEADER { "", "", "", "", "", "", "", "", ""}

using namespace std;

// A single row of columns
class Row
{
public:
	vector<char*> cols;
};

// Class for handling the sorting column using std::sort
class SortColumn
{
private:
	int _col, _reverse;

public:
	SortColumn (int col, int reverse);
	bool operator() (const Row &a, const Row &b);
};

// Derive a custom class from Fl_Table_Row
class Tableau : public Fl_Table_Row, public Cards_event_handler
{
private:
	vector<Row> _rowdata;
	int _sort_reverse;
	int _sort_lastcol;
	Cards_wrapper* _cards;

	static void event_callback(Fl_Widget*,void*);
	void event_callback2();
protected:
	// table cell drawing
	void draw_cell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
	// sort the table by a column
	void sort_column(int col, int reverse=0);
	void draw_sort_arrow(int X, int Y, int W, int H);
	void ListOfInstalledPackages (const set<string>& RowsColumns );
public:
	//Constructor
	Tableau(int x, int y, int w, int h, const char *l=0);
	~Tableau(){}
	void load_table(); // Load the packages list
	void autowidth(int pad); // Automatically set the columns widths to the longuest string
	void resize_window();	// Resize the parent window to size of table
};

#endif
