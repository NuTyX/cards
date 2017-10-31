/*
 * cards_package.cxx
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

#include "cards_package.h"

Cards_package::Cards_package()
{
	_installed = false;
}

Cards_package::~Cards_package()
{

}

string Cards_package::getBase()
{
	return _base;
}

string Cards_package::getName()
{
	return _name;
}

string Cards_package::getVersion()
{
	return _version;
}

string Cards_package::getPackager()
{
	return _packager;
}

string Cards_package::getDescription()
{
	return _description;
}

bool Cards_package::isInstalled()
{
	return _installed;
}
