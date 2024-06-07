/*
 * table_package.h
 *
 * Copyright 2015 - 2017 Thierry Nuttens <tnut@nutyx.org>
 * Copyright 2017 Gianni Peschiutta <artemia@nutyx.org>
 * Copyright 2017 - 2023 Thierry Nuttens <tnut@nutyx.org>
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

#ifndef TABLE_PACKAGE_H
#define TABLE_PACKAGE_H

#include "table_base.h"


/** \class table_package
 * \brief widget to manage cards package list
 *
 * This class list and manage Card operation by adding , remove or upgrade package
 *
 */
class table_package : public table_base
{
public:
    /**
     * \brief Constructor
     *
     * Constructor of Tableau class
     *
     */
    table_package(int x, int y, int w, int h, const char *l=0);

    /**
     * \brief Destructor
     *
     * Destructor of Tableau class
     *
     */
    virtual ~table_package(){}

    /**
     * \brief Populate the tab with package installed
     *
     * Get installed package from cards and extract a list to be
     * displayed as a list sorted by package name, description, version
     */
    void refreshTable(); // Load the packages list

    /**
     *  \brief Mark selected package to be installed
     *  \return The number of packages selected
     */
    int install_selected(); // Install packages selected

    /**
     *  \brief Mark selected package to be removed
     *  \return The number of packages selected
     */
    int remove_selected(); // Removed packages selected

protected:
    void OnDrawCell(TableContext context, int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);
    void OnEvent(TableContext context, int pCol, int pRow);
};

#endif
