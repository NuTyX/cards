/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2017 - .... Thierry Nuttens <tnut@nutyx.org>

#include <algorithm>
#include <string>
#include <vector>

#include "mainWindow.h"
#include "cards_log.h"

int helpFlag = 0;
bool isInstaller = false;
std::string Theme = "gtk+";

///
/// Argument parser for FlCards application in Fltk context
///
int arg_parser( int argc, char** argv, int &i )
{
    if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0)
    {
        helpFlag = 1;
        i += 1;
        return 1;
    }
    if (strcmp("-ni", argv[i]) == 0 || strcmp("--nutyx-installer", argv[i]) == 0)
    {
        isInstaller = true;
        i += 1;
        return 1;
    }
    if (strcmp("-th", argv[i]) == 0 || strcmp("--theme", argv[i]) == 0)
    {
        if (argv[i+1]==nullptr) return 0;
        Theme = argv[i+1];
        i += 2;
        return 1;
    }
    return 0;
}

///
/// Main event loop
///
void MainLoop(void*)
{
    cards::cards_logger::loopCallback();
    Fl::add_timeout(0.2, MainLoop);
}

///
/// Main Program
///
int main(int argc, char **argv)
{
    Fl::lock();
    int i = 1;
    if (Fl::args(argc, argv, i, arg_parser) < argc)
        // note the concatenated strings to give a single format string!
        Fl::fatal("error: unknown option: %s\n"
              "usage: %s [options]\n"
              " -h | --help             : print extended help message\n"
              " -ni | --nutyx-installer : launch FlCards in Installer mode\n"
              " -th | --theme           : choose Fltk theme.\n"
              "                           Valid entries are :\n"
              "                           none or base - default look-n-feel\n"
              "                           plastic - inspired by Aqua user interface\n"
              "                           gtk+ - inspired by Red Hat Bluecurve theme\n"
              "                           gleam - inspired by Clearlooks Glossy scheme\n"
              " plus standard fltk options\n",
              argv[i], argv[0]);
    if (helpFlag)
        Fl::fatal("usage: %s [options]\n"
              " -h | --help             : print extended help message\n"
              " -ni | --nutyx-installer : launch FlCards in Installer mode\n"
              " -th | --theme           : choose Fltk theme.\n"
              "                           Valid entries are :\n"
              "                           none or base - default look-n-feel\n"
              "                           plastic - inspired by Aqua user interface\n"
              "                           gtk+ - inspired by Red Hat Bluecurve theme\n"
              "                           gleam - inspired by Clearlooks Glossy scheme\n"
              " plus standard fltk options:\n"
              "%s\n", argv[0], Fl::help);

    //Enable Log Manager
    if (Theme.length() > 0) Fl::scheme(Theme.c_str());
    mainWindow win;
    win.resizable(win);
    win.show(argc, argv);
    win.LoadConfig();
    //Start Auto MainLoop
/*
 * cards_logger is broken for the moment
 */
//    MainLoop(0);
    return Fl::run();
}
