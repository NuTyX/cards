//  strings_utils.cc
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


#include "string_utils.h"

#include <cctype>
#include <fstream>
#include <sys/stat.h>


parameter_value split_parameter_value(string s, string delimiter)
{
  parameter_value pv;
  pv.parameter = s;
  pv.value = s;
  size_t found =  s.find(delimiter);
  if (found != string::npos)
  {
    pv.parameter.erase(s.find(delimiter),s.size());
    pv.value.erase(0,s.find(delimiter)+delimiter.size());
  }
  else
  {
    pv.value = "";
  }
  return pv;
}

set<string> get_parameter_list(string file, string delimiter)
{
  set<string> parameter_list;
  ifstream in(file.c_str());
  string line, property;
  if (in) {
    while (!in.eof()) {
      getline(in, line);
      if ((line[0] != '#' ) && ( line.find(delimiter) > 0) && ( line.size() > 0)) {
        property = line;
        property.erase(property.find(delimiter),property.size());
        parameter_list.insert(property);
      }
    }
  }
  return parameter_list;
}

string get_configuration_value(string file, string delimiter,string parameter)
{
  map<string,string> property_list;
  ifstream in(file.c_str());
  string line;
  parameter_value pv;
  if (in) {
    while (!in.eof()) {
      getline(in, line);
      if ((line[0] != '#' ) && ( line.find(delimiter) > 0) && ( line.size() > 0)) {
        pv = split_parameter_value(line,delimiter);
        property_list[pv.parameter]=pv.value;
      }
    }
  }
  return property_list[parameter];
}

string itos(unsigned int value)
{
  static char buf[20];
  sprintf(buf, "%u", value);
  return buf;
}

string mtos(mode_t mode)
{
  string s;

  // File type
  switch (mode & S_IFMT) {
        case S_IFREG:  s += '-'; break; // Regular
        case S_IFDIR:  s += 'd'; break; // Directory
        case S_IFLNK:  s += 'l'; break; // Symbolic link
        case S_IFCHR:  s += 'c'; break; // Character special
        case S_IFBLK:  s += 'b'; break; // Block special
        case S_IFSOCK: s += 's'; break; // Socket
        case S_IFIFO:  s += 'p'; break; // Fifo
        default:       s += '?'; break; // Unknown
        }

  // User permissions
        s += (mode & S_IRUSR) ? 'r' : '-';
        s += (mode & S_IWUSR) ? 'w' : '-';
        switch (mode & (S_IXUSR | S_ISUID)) {
        case S_IXUSR:           s += 'x'; break;
        case S_ISUID:           s += 'S'; break;
        case S_IXUSR | S_ISUID: s += 's'; break;
        default:                s += '-'; break;
        }

        // Group permissions
  s += (mode & S_IRGRP) ? 'r' : '-';
        s += (mode & S_IWGRP) ? 'w' : '-';
        switch (mode & (S_IXGRP | S_ISGID)) {
        case S_IXGRP:           s += 'x'; break;
        case S_ISGID:           s += 'S'; break;
  case S_IXGRP | S_ISGID: s += 's'; break;
        default:                s += '-'; break;
        }

        // Other permissions
        s += (mode & S_IROTH) ? 'r' : '-';
        s += (mode & S_IWOTH) ? 'w' : '-';
        switch (mode & (S_IXOTH | S_ISVTX)) {
        case S_IXOTH:           s += 'x'; break;
        case S_ISVTX:           s += 'T'; break;
        case S_IXOTH | S_ISVTX: s += 't'; break;
        default:                s += '-'; break;
        }

  return s;
}
string get_value(const string& s, char delimiter)
{
	string::size_type pos = s.find( delimiter );
	if ( pos != string::npos && pos+1 < s.length() )
		return s.substr( pos + 1 );
	return "";
}
string get_value_before(const string& s, char delimiter)
{
	string::size_type pos = s.find( delimiter );
	if ( pos != string::npos )
		return s.substr( 0, pos );
	return s;
}
string strip_white_space(const string& s)
{
	if ( s.empty() )
		return s;
	int pos = 0;
	string line = s;
	string::size_type len = line.length();
	while ( pos < len && isspace( line[pos] ) ) {
		++pos;
	}
	line.erase( 0, pos );
	pos = line.length()-1;
	while ( pos > -1 && isspace( line[pos] ) ) {
		--pos;
	}
	if ( pos != -1 ) {
		line.erase( pos+1 );
	}
	return line;
}
bool starts_with_no_case(const string& s1, const string& s2)
{
	string::const_iterator p1 = s1.begin();
	string::const_iterator p2 = s2.begin();

	while ( p1 != s1.end() && p2 != s2.end() ) {
		if ( toupper( *p1 ) != toupper( *p2 ) ) {
			return false;
		}
		++p1;
		++p2;
	}
	if ( p1 == s1.end() && p2 != s2.end() ) {
		return false;
	}
	return true;
}
string to_lower_case(const string& s)
{
	string result = "";
	for ( string::size_type i = 0; i < s.length(); ++i ) {
		result += tolower( s[i] );
	}
	return result;
}
string to_upper_case(const string& s)
{
	string result = "";
	for ( string::size_type i = 0; i < s.length(); ++i ) {
		result += toupper( s[i] );
	}
	return result;
}
// vim:set ts=2 :
