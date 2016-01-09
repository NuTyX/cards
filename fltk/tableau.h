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

#define MARGIN 20
#define COLHEADER { "Nom:", "Version:", "Description:", "", "", "", "", "", ""}
#define CMD "pkg-get-list"
// A single row of columns
class Row {
public:
	std::vector<char*> cols;
};
// Class for handling the sorting column using std::sort
class SortColumn 
{

	int _col, _reverse;
public:
	SortColumn (int col, int reverse)
	{
		_col = col;
		_reverse= reverse;
	}
	bool operator() (const Row &a, const Row &b)
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
};
// Derive a custom class from Fl_Table_Row
class Tableau : public Fl_Table_Row 
{
private:
	std::vector<Row> _rowdata;
	int _sort_reverse;
	int _sort_lastcol;

	static void event_callback(Fl_Widget*,void*);
	void event_callback2();
protected:
	// table cell drawing
	void draw_cell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
	// sort the table by a column
	void sort_column(int col, int reverse=0);
	void draw_sort_arrow(int X, int Y, int W, int H);
public:
	//Constructor
	Tableau(int x, int y, int w, int h, const char *l=0) : Fl_Table_Row(x,y,w,h,l) 
	{
		_sort_reverse = 0;
		_sort_lastcol = -1;
		end();
		callback(event_callback, (void*)this);
	}
	~Tableau(){}
	void load_command(const char *packages); // Load the packages list
	void autowidth(int pad); // Automatically set the columns widths to the longuest string
	void resize_window();	// Resize the parent window to size of table
};


#endif
