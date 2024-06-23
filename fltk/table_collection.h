/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2015 - 2018 Thierry Nuttens <tnut@nutyx.org>
// Copyright 2017 - 2018 Gianni Peschiutta <artemia@nutyx.org>
// Copyright 2018 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

#include "pixmaps/lxde.xpm"
#include "pixmaps/xfce.xpm"
#include "pixmaps/lxqt.xpm"
#include "pixmaps/mate.xpm"
#include "pixmaps/kde.xpm"
#include "pixmaps/gnome.xpm"
#include "pixmaps/E17.xpm"
#include "table_base.h"

/** \class table_collection
 * \brief widget to manage cards package list
 *
 * This class list and manage Card operation by adding , remove or upgrade package
 *
 */
class table_collection : public table_base
{
public:
    /**
     * \brief Constructor
     *
     * Constructor of Tableau class
     *
     */
    table_collection(int x, int y, int w, int h, const char *l=0);

    /**
     * \brief Destructor
     *
     * Destructor of Tableau class
     *
     */
    virtual ~table_collection(){}

    /**
     * \brief Populate the tab with package installed
     *
     * Get installed package from cards and extract a list to be
     * displayed as a list sorted by package name, description, version
     */
    void refresh_table(); // Load the Collection list

    int install_selected();

    int remove_selected();

protected:
    void OnDrawCell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
    void OnEvent(TableContext context, int pCol, int pRow);
};
