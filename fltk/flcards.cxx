/*
 * flcards.cxx
 *
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

#include <algorithm>
#include <string>
#include <vector>

#include "mainWindow.h"

int helpFlag = 0;
bool isInstaller = false;

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
    return 0;
}

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
              " plus standard fltk options\n",
              argv[i], argv[0]);
    if (helpFlag)
        Fl::fatal("usage: %s [options]\n"
              " -h | --help             : print extended help message\n"
              " -ni | --nutyx-installer : launch FlCards in Installer mode\n"
              " plus standard fltk options:\n"
              "%s\n", argv[0], Fl::help);
	mainWindow win(isInstaller);
	win.resizable(win);
	win.show(argc, argv);
	win.LoadConfig();
	return Fl::run();
}
