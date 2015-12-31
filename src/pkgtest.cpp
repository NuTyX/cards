/*
 * pkgtest.cc
 * 
 * Copyright 2016 Thierry Nuttens <tnut@nutyx.org>
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

#include "pkgtest.h"

Pkgtest::Pkgtest()
	:Pkginfo("pkgtest"), Pkgrepo("/etc/cards.conf")
{
}
void Pkgtest::parseArguments(int argc, char** argv)
{
	m_root="/";
	m_epoc=1;
	m_arg="gcc";
}
void Pkgtest::printHelp() const
{
	cout << USAGE << m_utilName << " [options] <package>" << endl
	<< OPTIONS << endl
	<< "  -r, --root <path>   specify alternative installation root" << endl
	<< "  -v, --version       print version and exit" << endl
	<< "  -h, --help          print help and exit" << endl;

}
