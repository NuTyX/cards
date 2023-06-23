/*
 * table_base.h
 *
 * Copyright 2015 - 2018 Thierry Nuttens <tnut@nutyx.org>
 * Copyright 2017 - 2018 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2018 - 2023  Thierry Nuttens <tnut@nutyx.org>
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

#ifndef TABLE_BASE_H
#define TABLE_BASE_H

#include "cards_wrapper.h"

#include <ctype.h>

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Menu.H>

#include "pixmaps/checked.xpm"
#include "pixmaps/download.xpm"
#include "pixmaps/deleted.xpm"

#define MARGIN 20

using namespace cards;

// A single row of columns
class Row
{
public:
    Row(){data=nullptr;}
    std::vector<std::string> cols;
    void* data;
};

// Class for handling the sorting column using std::sort
class SortColumn
{
private:
    int m_col, m_reverse;

public:
    SortColumn (int col, int reverse);
    bool operator() (const Row &a, const Row &b);
};

/** \class Tableau
 * \brief widget to manage cards package list
 *
 * This class list and manage Card operation by adding , remove or upgrade package
 *
 */
class TableBase : public Fl_Table_Row, public CEventHandler
{
public:
    /**
     * \brief Constructor
     *
     * Constructor of Tableau class
     *
     */
    TableBase(int x, int y, int w, int h, const char *l=0);

    /**
     * \brief Destructor
     *
     * Destructor of Tableau class
     *
     */
    virtual ~TableBase(){}

    /**
     * \brief Populate the tab with package installed
     *
     * Get installed package from cards and extract a list to be
     * displayed as a list sorted by package name, description, version
     */
    virtual void refresh_table() = 0; // Load the packages list
    void autowidth(int pad); // Automatically set the columns widths to the longuest string
    void resize_window();	// Resize the parent window to size of table
    void setFilter(const std::string& pValue);
    int getCntRowSelected();
    virtual int install_selected() = 0; // Install rows selected
    virtual int remove_selected() = 0; // Remove rows selected

protected:
    std::vector<std::string> colTitle;

    // table cell drawing
    void draw_cell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
    // sort the table by a column
    void sort_column(int col, int reverse=0);
    void draw_sort_arrow(int X, int Y, int W, int H);

    void OnDoJobListFinished (const CEH_RC rc);
    void OnRefreshPackageFinished (const CEH_RC rc);
    void OnJobListChange(const CEH_RC rc);
    virtual void OnDrawCell(TableContext, int, int, int, int, int, int){}
    virtual void OnEvent(TableContext, int, int){}

private:
    static void event_callback(Fl_Widget*,void*);
    void event_callback2();

protected:
    std::string m_filter;
    std::vector<Row> m_rowdata;
    int m_sort_reverse;
    int m_sort_lastcol;
    CWrapper* m_cards;
};

#endif

