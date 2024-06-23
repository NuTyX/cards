/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2015 - 2018 Thierry Nuttens <tnut@nutyx.org>
// Copyright 2017 - 2018 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2018 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

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

// A single row of columns
class Row
{
public:
    Row(){data=nullptr;}
    std::vector<std::string> cols;
    void* data;
};

// Class for handling the sorting column using std::sort
class sort_column
{
private:
    int m_col, m_reverse;

public:
    sort_column (int col, int reverse);
    bool operator() (const Row &a, const Row &b);
};

/** \class Tableau
 * \brief widget to manage cards package list
 *
 * This class list and manage Card operation by adding , remove or upgrade package
 *
 */
class table_base : public Fl_Table_Row, public cards::cards_event_handler
{
public:
    /**
     * \brief Constructor
     *
     * Constructor of Tableau class
     *
     */
    table_base(int x, int y, int w, int h, const char *l=0);

    /**
     * \brief Destructor
     *
     * Destructor of Tableau class
     *
     */
    virtual ~table_base(){}

    /**
     * \brief Populate the tab with package installed
     *
     * Get installed package from cards and extract a list to be
     * displayed as a list sorted by package name, description, version
     */
    virtual void refreshTable() = 0; // Load the packages list
    void autowidth(int pad); // Automatically set the columns widths to the longuest string
    void resizeWindow();	// Resize the parent window to size of table
    void setFilter(const std::string& pValue);
    int getCntRowSelected();
    virtual int install_selected() = 0; // Install rows selected
    virtual int remove_selected() = 0; // Remove rows selected

protected:
    std::vector<std::string> colTitle;

    // table cell drawing
    void drawCell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
    // sort the table by a column
    void sortColumn(int col, int reverse=0);
    void drawSortArrow(int X, int Y, int W, int H);

    void OnDoJobListFinished (const cards::CEH_RC rc);
    void OnRefreshPackageFinished (const cards::CEH_RC rc);
    void OnJobListChange(const cards::CEH_RC rc);
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
    cards::cards_wrapper* m_cards;
};
