//  webcards.cc
//
//  Copyright (c) 2016 by NuTyX team (http://nutyx.org)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

#include <sys/stat.h>

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "file_utils.h"

using namespace std;

void printFile(const char* fileName) {
	vector<string> fileContent;
	if  ( parseFile(fileContent,fileName) == 0 )
		for (auto i:fileContent) cout << i <<endl;
}
/* TODO maybe not that clean
	I guess this will endup in a file somewhere
*/
void head(string &title)
{
	cout << "Content-type:text/html\r\n\r\n"
	<< "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"" << endl
	<< "     \"http://www.w3.org/TR/html4/strict.dtd\">" << endl
	<< "<HTML>" << endl
	<< "   <HEAD>" << endl
	<< "      <TITLE>" << title << "</TITLE>" << endl
	<< "      <META charset=\"utf-8\" />" << endl
	<< "      <LINK rel=\"shortcut icon\" href=\"../favicon.icon\">"
	<< endl;
	
}
/* TODO maybe not that clean
	Same same
*/
void footer(void)
{
	cout << "<hr>" << endl
	<< "  <footer >" << endl
	<< "     <p id=\"foot\"><img src=\"../graphics/logo_nutyx_25.png\" "
	<< "alt=\"NuTyX logo\"> ...<br />" << endl
	<< "     &copy; 2007 - 2016 "
	<< "<a href=\"http://nutyx.org\">NuTyX</a>.<br /><br />" << endl
	<< "     <a href=\"http://www.wtfpl.net/\">" << endl
	<< "     <img src=\"http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-4.png\"" << endl
	<< "     width=\"80\" height=\"15\" alt=\"WTFPL\" /></a><br><br>" << endl
	<< "     Powered by cards "
	<< VERSION 
	<< "</p>" << endl
	<< "   </footer>" << endl
        << "  </table>" << endl 
	<< " </body>" << endl
	<< "</html>" << endl;
}
int main (int argc, char** argv)
{
	string argument = basename(argv[0]);
	string::size_type pos = argument.find( '.' );
	if ( pos != string::npos && pos+1 < argument.length() ) {
		string fileName = argument.substr(0,pos);
		fileName += ".html";
		fileName = "." + fileName;
		head(argument);
		printFile ("../css/.nutyx.css");
		printFile (".menu.html");

		cout  << "<table border=\"0\" cellpadding=\"15\" cellspacing=\"10\" width=\"100%\">"
			<< "<tr valign=\"top\">"
			<< "<td valign=\"top\" align=\"left\" width=\"100%\">"
			<< "<p class=\"updated\"> "
			<<  getModifyTimeFile(fileName)
			<< " UTC</p>" << endl;
			printFile (fileName.c_str());
		footer();
	}
	return 0;
}
// vim:set ts=2 :
