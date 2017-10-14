#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Global Include
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>
#include <string>
#include <vector>

//Project Include
#include "pixmaps/flcards.xpm"
#include "tableau.h"

using namespace std;

class mainWindow : public Fl_Double_Window
{
    public:
        mainWindow(int W, int H, string Title);
        virtual ~mainWindow();

    protected:
static  void Quitter_CB(Fl_Widget*,void* instance);
static  void Available_Packages_CB(Fl_Widget*,void* instance);
static  void Installed_Packages_CB(Fl_Widget*,void* instance);

    private:
        vector<Fl_Menu_Item> Menu;
        Tableau* m_Tab;
        Fl_Pixmap* p_win;
        Fl_RGB_Image* p_im;
        Fl_Input* Recherche;
        Fl_Menu_Bar* BarMenu;
        Fl_Text_Display* TextDisplay;
        Fl_Button* BtnInstall;
};

#endif // MAINWINDOW_H
