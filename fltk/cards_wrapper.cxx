/*
 * cards_wrapper.cxx
 *
 * Copyright 2015-2017 Thierry Nuttens <tnut@nutyx.org>
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

#include "cards_wrapper.h"

// Init static pointer to null
Cards_wrapper* Cards_wrapper::_ptCards_wrapper = nullptr;

// Constructor
Cards_wrapper::Cards_wrapper()
{
	_ptCards = new Cards_client("/etc/cards.conf");
}

Cards_wrapper::~Cards_wrapper()
{
	if (_ptCards != nullptr) delete _ptCards;
}

Cards_wrapper* Cards_wrapper::instance()
{
	if (_ptCards_wrapper==nullptr)
		_ptCards_wrapper=new Cards_wrapper();
	return _ptCards_wrapper;
}

void Cards_wrapper::kill()
{
	if (_ptCards_wrapper!=nullptr)
		delete _ptCards_wrapper;
}

void Cards_wrapper::getListOfInstalledPackages()
{

}
