/* SPDX-License-Identifier: LGPL-2.1-or-later */
// Copyright 2016 - .... Thierry Nuttens <tnut@nutyx.org>

#pragma once

#include <libcards.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Progress.H>

class Flcards_info : public pkgdbh {
public:
	Flcards_info (const std::string& configFile);
	~Flcards_info() {}
	std::set<std::string> getListOfInstalledPackages();
	std::set<std::string> getListOfAvailablePackages();

protected:
	void progressInfo();
	Fl_Window *m_window;
	Fl_Progress *m_progressBar;
};
