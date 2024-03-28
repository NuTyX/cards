//
//  string_utils.h
// 
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - 2023 by NuTyX team (http://nutyx.org)
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

#include "error_treat.h"

#include <fstream>
#include <map>
#include <set>
#include <vector>

#include <sys/stat.h>

#define GIGA 1e9
#define MEGA 1e6
#define KILO 1e3
#define PACKAGE_LOCALE_DIR "/usr/share/locale"
#define GETTEXT_PACKAGE "cards"

typedef std::map<std::string, std::string> alias_t;

void *Malloc(size_t s);

struct keyValue 
{
	std::string parameter;
	std::string value;
};

/**
 * itemList is a list of char* dynamically allocated
 */
typedef struct
{
	char **items;
	unsigned int count;
} itemList;

/**
 * initItemList: Create an item List
 *
 */
itemList *initItemList(void);

/**
 * addItemToItemList: Add an item to an already created itemList
 *
 */
void addItemToItemList(itemList *list, const char *item);

/**
 * freeItemList: Destroy the itemList created and free the allocated memory
 *
 */
 
void freeItemList(itemList *list);

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

#endif /* STRING_UTILS_H */
// vim:set ts=2 :
