/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2016 - .... Thierry Nuttens <tnut@nutyx.org>

#include "flcards_info.h"

Flcards_info::Flcards_info(const std::string& configFileName)
	: pkgdbh("")
{
}
void Flcards_info::progressInfo()
{

	static int j = 0;
	unsigned int i, n;
	n = getNumberOfPackages();
	switch (m_actualAction)
	{
		case cards::ACTION_ENUM_DB_OPEN_START:
			m_progressBar->value(0);
			break;
		case cards::ACTION_ENUM_DB_OPEN_RUN:
			if (n >100) {
				i = j /(n / 100);
				m_progressBar->value(i);
			}
			j++;
			break;
		case cards::ACTION_ENUM_DB_OPEN_END:
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

	buildDatabase(false);

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
			+ i.second.version() + '\t'
			+ i.second.description() + '\t';

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

	cards::pkgrepo pkgrepo("/etc/cards");

	std::set<std::string> ListOfAvailablePackages;
	std::string packageNameVersion;
	for (auto i : pkgrepo.getListOfPackages()) {
		packageNameVersion = i.first + '\t'
			+ i.second.version() + '\t'
			+ i.second.description() + '\t';
		ListOfAvailablePackages.insert(packageNameVersion);
	}
	delete m_progressBar;
	delete m_window;

	return ListOfAvailablePackages;
}
