/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "mysql.h"
#include "webrepo.h"

#include <ctime>
#include <ratio>
#include <chrono>

#define INSERTPACKAGE(basename, name) listOfPackages.insert("<td>" \
+ i.second.system() + "</td>" \
+ "<td>" + i.second.branch() + "</td>"  \
+ "<td>" + i.second.collection() + "</td>" \
+ "<td> <a href=\"https://github.com/NuTyX/" + i.second.system() \
+ "/tree/" + i.second.branch() + "/" \
+ i.second.collection() + "/" + basename + "\">" + name + "</a></td>" \
+ "<td> <a href=\"https://github.com/NuTyX/" + i.second.system() \
+ "/commits/" + i.second.branch() + "/" \
+ i.second.collection() + "/" + basename + "\">" + i.second.version() + "-" + itos(i.second.release()) + "</a></td>" \
+ "<td>" + i.second.description() + "</td>" \
+ "<td>" + getDateFromEpoch(i.second.build()) + "</td>")

struct contentInfo_t {
	std::string date;
	std::string boardId;
	std::vector<std::string> text;
};
struct arguments_t {
	std::string packageSystem; /* The system on which the package is build for to search */
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
