/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#pragma once

#include "error_treat.h"

#include <fstream>
#include <map>
#include <set>
#include <vector>

#include <sys/stat.h>

typedef std::map<std::string, std::string> alias_t;

struct keyValue 
{
	std::string parameter;
	std::string value;
};

/**
 * splitKeyValue: Split a string value into a keyValue structure
 *
 */
keyValue splitKeyValue
(std::string s, char delimiter);

std::set<std::string> getKeysList
(std::string file, std::string delimiter);

std::string getValueOfKey
(std::string file, std::string delimiter,std::string parameter);

std::string getValue(const std::string& s, char delimiter);
std::string getValueBefore( const std::string& s, char del );
std::string getValueBeforeLast( const std::string& s, char del );

std::string itos(unsigned int value);
std::string ultos(unsigned long int value);

std::string mtos(mode_t mode);
std::string trimFileName(const std::string& filename);
std::string sizeHumanRead(int value);

/*param s the string to be searched, param delimiter the delimiter char 
return the value after the first occurance of a delimiter */
std::string getFirstValueOfKeyAfterDelim(const std::string& s, char delimiter);

/* strip whitespace in the beginning and end of string, return a stripped string */
std::string stripWhiteSpace(const std::string& s);

/* populate a vector of string with delimited characters */
std::vector<std::string> parseDelimitedVectorList
(const std::string& s, const char *delimiter);

/* populate a vector of string with a delimited character */
const std::vector<std::string> parseDelimitedVectorList
(const std::string& s, const char& c);

/* populate a set of string with delimited characters */
std::set<std::string> parseDelimitedSetList
(const std::string& s, const char *delimiter);

/* make sure s1 starts with s2 */
bool startsWith(const std::string& s, const std::string& with);

/**
 startsWithNoCase
 make sure s1 starts with s2
*/
bool startsWithNoCase(const std::string& s1, const std::string& s2);

std::string convertToLowerCase(const std::string& s);
std::string convertToUpperCase(const std::string& s);

std::string replaceAll
( std::string& in, const std::string& oldString, const std::string& newString );

/**
  Split a string into parts
	param s string to be split
	param del delimter
	param startPos position to start at
	param useEmpty include empty (whitespace only9 results in the result

	return a list of string 
*/
template <class T>
void split( const std::string& s, char del,
            T& target,
            int startPos, bool useEmpty )
{
	std::string line = s;
	std::string ss;
	std::string::size_type pos;
	int offset = startPos;
	while ( ( pos = line.find( del, offset ) ) != std::string::npos ) {
		offset = 0;
		if ( line[pos-1] == '\\'  ) {
			line.erase(pos-1,1);
			ss = ss + line.substr(0,pos);
			line.erase(0,pos);
			continue;
		}
		std::string val = line.substr( 0, pos );
		if ( ( useEmpty || !stripWhiteSpace( val ).empty() ) ||
			( ss.length() > 0 ) ) {
			target.push_back( ss + val );
		}
		line.erase( 0, pos+1 );
	}

	if ( ( line.length() > 0 ) || ( ss.length() > 0 ) ) {
		target.push_back( ss + line );
	}
}
