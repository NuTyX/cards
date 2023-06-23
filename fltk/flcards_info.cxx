/*
 * flcards_info.cxx
 *
 * Copyright 2016 - 2020 Thierry Nuttens <tnut@nutyx.org>
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

Flcards_info::Flcards_info(const std::string& configFileName)
	: Pkgdbh(""),Pkgrepo(configFileName)
{
	m_root="/";
}
void Flcards_info::progressInfo()
{

	static int j = 0;
	int i,n;
	n = getListOfPackagesNames("");
	switch ( m_actualAction )
	{
		case DB_OPEN_START:
			m_progressBar->value(0);
			break;
		case DB_OPEN_RUN:
			if (n >100) {
				i = j / ( n / 100);
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
std::set<std::string> Flcards_info::getListOfInstalledPackages()
{
	m_window = new Fl_Window(300,40,"Get List of Installed Packages");
	m_progressBar = new Fl_Progress(10,10,280,20);
	m_window->add(m_progressBar);
	m_window->clear_border();
	m_progressBar->minimum(0);
	m_progressBar->maximum(100);
	m_progressBar->labelcolor(FL_BLACK);
	m_progressBar->color(FL_GRAY);
	m_progressBar->selection_color(FL_GREEN);
	m_progressBar->show();
	m_window->resizable(m_window);
	m_window->show();
	m_progressBar->value(0);

	getListOfPackagesNames (m_root);
	buildDatabase(false,true,false,false,"");

	delete m_progressBar;
	delete m_window;

	std::set<std::string> ListOfInstalledPackages;
	std::string packageDetails;
	for (auto i : m_listOfPackages) {
#ifndef NDEBUG
		std::cerr << i.first
			<< std::endl;
#endif
		packageDetails = i.first + '\t'
			+ i.second.version + '\t'
			+ i.second.description + '\t';

		ListOfInstalledPackages.insert(packageDetails);
	}
return ListOfInstalledPackages;
}

std::set<std::string> Flcards_info::getListOfAvailablePackages()
{
	m_window = new Fl_Window(300,40,"Get List of Available Packages");
	m_progressBar = new Fl_Progress(10,10,280,20);
	m_window->add(m_progressBar);
	m_window->clear_border();
	m_progressBar->minimum(0);
	m_progressBar->maximum(100);
	m_progressBar->labelcolor(FL_BLACK);
	m_progressBar->color(FL_GRAY);
	m_progressBar->selection_color(FL_BLUE);
	m_progressBar->show();
	m_window->resizable(m_window);
	m_window->show();
	m_progressBar->value(0);

	parsePkgRepoCollectionFile();

	std::set<std::string> ListOfAvailablePackages;
	std::string packageNameVersion;
	for (auto i : m_portsDirectoryList) {
		for (auto j : i.basePackageList) {
			packageNameVersion = j.basePackageName + '\t'
				+ j.version + '\t'
				+ j.description + '\t';
			ListOfAvailablePackages.insert(packageNameVersion);
		}
	}
	delete m_progressBar;
	delete m_window;

	return ListOfAvailablePackages;
}
