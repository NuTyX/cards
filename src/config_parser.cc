//  config_parser.cc
//
//  Copyright (c) 2002-2005 by Johannes Winkelmann jw at tks6 dot net
//  Copyright (c) 2014 by NuTyX team (http://nutyx.org)
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

#include <iostream>
#include <cstdio>
#include <cstring>

#include "config_parser.h"

using namespace std;

int ConfigParser::parseConfig(const string& fileName, Config& config)
{
	FILE* fp = fopen(fileName.c_str(), "r");
	if (!fp) {
		return -1;
	}

	char line[512];
	string s;
	while (fgets(line, 512, fp)) {
		if (line[strlen(line)-1] == '\n' ) {
			line[strlen(line)-1] = '\0';
		}
		s = line;

		// strip comments
		string::size_type pos = s.find('#');
		if (pos != string::npos) {
			s = s.substr(0,pos);
		}

		// whitespace separates
		pos = s.find(' ');
		if (pos == string::npos) {
			// try with a tab
			pos = s.find('\t');
		}
		if (pos != string::npos) {
			string key = s.substr(0, pos);
			string val = stripWhiteSpace(s.substr(pos));

			if (key == "dir") {
				pos = val.find('|');
				if (pos != string::npos) {
					string dir = stripWhiteSpace(val.substr(0,pos));
					string url = stripWhiteSpace(val.substr(pos+1));
					config.dirUrl[dir] = url;
				}
			}
		}
	}
	fclose(fp);
	return 0;
}
string ConfigParser::stripWhiteSpace(const string& input)
{
	string output = input;
	while (isspace(output[0])) {
		output = output.substr(1);
	}
	while (isspace(output[output.length()-1])) {
		output = output.substr(0, output.length()-1);
	}

	return output;
}
// vim:set ts=2 :
