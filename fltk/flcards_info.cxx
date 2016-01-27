/*
 * flcards_info.cxx
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

#include "flcards_info.h"


Flcards_info::Flcards_info(const string& configFileName)
	: Pkgdbh(""),Pkgrepo(configFileName)
{
	m_root="/";	
}
void Flcards_info::progressInfo() const
{
	
	static int j = 0;
	int i;
	switch ( m_actualAction )
	{
		case DB_OPEN_START:
			m_progressBar->value(0);
			break;
		case DB_OPEN_RUN:
			if ( m_packageNamesList.size()>100) {
				i = j / ( m_packageNamesList.size() / 100);
				m_progressBar->value(i);
			}
			j++;
			break;
		case DB_OPEN_END:
				m_progressBar->value(100);
				break;
	}
	Fl::check();
}
vector<string> Flcards_info::getListOfInstalledPackages()
{
	m_window = new Fl_Window(250,40);
	m_progressBar = new Fl_Progress(10,10,200,20);
	m_window->add(m_progressBar);
	m_progressBar->minimum(0);
	m_progressBar->maximum(100);
	m_progressBar->labelcolor(FL_BLACK);
	m_progressBar->color(FL_GRAY);
	m_progressBar->selection_color(FL_BLUE);
	m_progressBar->show();
	m_window->resizable(m_window);
	m_window->show();
	m_progressBar->value(0);
	m_progressBar->label("Get List of Installed Packages");

	getListOfPackageNames (m_root);
	buildDatabaseWithDetailsInfos(false);

	delete m_progressBar;
	delete m_window;

	vector<string> ListOfInstalledPackages;
	string packageDetails;
	for (auto i : m_listOfInstPackages) {
#ifndef NDEBUG
		cerr << i.first << endl;
#endif
		packageDetails = i.first + '\t'
			+ i.second.version + '\t'
			+ i.second.description + '\t';

		ListOfInstalledPackages.push_back(packageDetails);
	}
return ListOfInstalledPackages;
}
