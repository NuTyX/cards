//  string_utils.h
// 
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 by NuTyX team (http://nutyx.org)
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

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <set>
#include <map>
#include <sys/types.h>

using namespace std;

struct keyValue {
    string parameter;
    string value;
  };


keyValue split_keyValue(string s, string delimiter);
set<string> getKeysList(string file, string delimiter);
string getValueOfKey(string file, string delimiter,string parameter);

string itos(unsigned int value);
string mtos(mode_t mode);
string trim_filename(const string& filename);

/*param s the string to be searched, param delimiter the delimiter char 
return the value after the first occurance of a delimiter */
string getFirstValueOfKeyAfterDelim(const string& s, char delimiter);

/*param s the string to be searched param delimiter  the delimiter char
return the value before the first occurance of a delimiter */
string getFirstValueOfKeyBeforeDelim(const string& s, char delimiter);

/* strip whitespace in the beginning and end of string, return a stripped string */
string stripWhiteSpace(const string& s);

/* populate a set of string with delimited string */
set<string> parseDelimitedList(const string& s, char delimiter);

/* make sure s1 starts with s2 */
bool startsWith(const string& s, const string& with);

/* make sure s1 starts with s2 */
bool starts_with_no_case(const string& s1, const string& s2);

string convertToLowerCase(const string& s);
string convertToUpperCase(const string& s);

#endif /* STRING_UTILS_H */
// vim:set ts=2 :
