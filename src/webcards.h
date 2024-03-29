// webcards.h
//
//  Copyright (c) 2016 - 2024 by NuTyX team (http://nutyx.org)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
//  USA.
//

#ifndef WEBCARDS_H
#define WEBCARD_H

#include "pkgrepo.h"
#include "mysql.h"

#include <ctime>
#include <ratio>
#include <chrono>

#define INSERTPACKAGE(basename, name) listOfPackages.insert("<td>" \
+ i.arch + "</td>" \
+ "<td>" + i.branch + "</td>"  \
+ "<td>" + i.collection + "</td>" \
+ "<td> <a href=\"https://github.com/NuTyX/packages-" + i.arch \
+ "/tree/" + i.branch + "/" \
+ i.collection + "/" + basename + "\">" + name + "</a></td>" \
+ "<td> <a href=\"https://github.com/NuTyX/packages-" + i.arch \
+ "/commits/" + i.branch + "/" \
+ i.collection + "/" + basename + "\">" + j.version + "-" + itos(j.release) + "</a></td>" \
+ "<td>" + j.description + "</td>" \
+ "<td>" + getDateFromEpoch(j.buildDate) + "</td>")

struct contentInfo_t {
	std::string date;
	std::string boardId;
	std::vector<std::string> text;
};
struct arguments_t {
	std::string packageArch;   /* The arch of the package to search */
	std::string packageBranch; /* The branch of the package to search */
	std::string docName; 	   /* The page to show */
	std::string stringSearch;  /* The string to search */
	std::string packageSearch; /* The package to search */
	std::string type; /* Type of search collection, package, etc */
};
typedef std::map<std::string, contentInfo_t> content_t;

/* populate a vector of string with delimed string from an html formated text */
std::vector<std::string> parseHTMLDelimitedList
(const std::vector<std::string>& text, 
const std::string delimiter, const std::string contentTitle);


void searchpkg(contentInfo_t &contentInfo, arguments_t &arguments);
void visitOfPage(char * Argument);

#endif /* WEBCARDS_H */
// vim:set ts=2 :
