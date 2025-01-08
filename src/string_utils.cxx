/* SPDX-License-Identifier: LGPL-2.1-or-later */
//  Copyright (c) 2002 by Johannes Winkelmann
//  Copyright (c) 2013 - .... by NuTyX team (https://nutyx.org)

#include "string_utils.h"

keyValue splitKeyValue(std::string s, char delimiter)
{
	keyValue pv;
	std::string entry;
  size_t found =  s.find(delimiter);
  if (found != std::string::npos)
  {
		entry = s;
		entry.erase(s.find(delimiter),s.size());
    pv.parameter=stripWhiteSpace(entry);
		entry = s;
    entry.erase(0,s.find(delimiter)+sizeof(char));
		pv.value=stripWhiteSpace(entry);
  }
  else
  {
    pv.value = "";
    pv.parameter = s;
  }
  return pv;
}

std::string sizeHumanRead(unsigned long value)
{
	char buffer[255];
	if ( value < KILO ) {
		snprintf(buffer,255,"%lu",value);
		return buffer;
	}
	if ( value < MEGA ) {
		snprintf(buffer,255,"%5.2f K",value / KILO);
		return buffer;
	}
	if ( value < GIGA ) {
		snprintf(buffer,255,"%5.2f M",value / MEGA);
		return buffer;
	}
	snprintf(buffer,255,"%5.2f G",value / GIGA);
	return buffer;
		
}
std::set<std::string> getKeysList(std::string file, std::string delimiter)
{
  std::set<std::string> parameter_list;
  std::ifstream in(file.c_str());
  std::string line, property;
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

std::string getValueOfKey(std::string file, char delimiter,std::string parameter)
{
  std::map<std::string,std::string> property_list;
  std::ifstream in(file.c_str());
  std::string line;
  keyValue pv;
  if (in) {
    while (!in.eof()) {
      getline(in, line);
			if ((line[0] != '#' ) && ( line.find(delimiter) > 0) && ( line.size() > 0)) {
        pv = splitKeyValue(stripWhiteSpace(line),delimiter);
        property_list[pv.parameter]=pv.value;
      }
    }
  }
  return stripWhiteSpace(property_list[parameter]);
}
std::string getValue( const std::string& s, char delimiter )
{
	std::string::size_type pos = s.find( delimiter );
	if ( pos != std::string::npos && pos+1 < s.length() ) {
		return s.substr( pos + 1 );
	}
	return "";
}
std::string getValueBefore( const std::string& s, char del )
{
	std::string::size_type pos = s.find( del );
	if (pos != std::string::npos)
		return s.substr(0, pos);

	return s;
}
std::string getValueBeforeLast(const std::string& s, char del)
{
	std::string::size_type pos = s.rfind(del);
	if (pos != std::string::npos)
		return s.substr(0,pos);
	return s;
}

std::string itos(unsigned int value)
{
  static char buf[20];
  sprintf(buf, "%u", value);
  return buf;
}

std::string ultos(unsigned long int value)
{
	static char buf[20];
	sprintf(buf, "%lu", value);
	return buf;
}
std::string mtos(mode_t mode)
{
  std::string s;

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
std::string getFirstValueOfKeyAfterDelim(const std::string& s, char delimiter)
{
	std::string::size_type pos = s.find( delimiter );
	if ( pos != std::string::npos && pos+1 < s.length() )
		return s.substr( pos + 1 );
	return "";
}
std::vector<std::string> parseDelimitedVectorList
	(const std::string& s, const char *delimiter)
{
	std::vector<std::string> list;
	if (s.empty())
		return list;

	char * cstr = new char [ s.length() + 1 ];
	std::strcpy (cstr, s.c_str());

	char * pch = std::strtok (cstr,delimiter);
	while (pch !=nullptr)
	{
		list.push_back(pch);
		pch = std::strtok (nullptr, delimiter);
	}
	delete[] cstr;
	return list;
}
std::set<std::string> parseDelimitedSetList
	(const std::string& s, const char *delimiter)
{
	std::set<std::string> list;
	if ( s.empty() )
		return list;

	char * cstr = new char [ s.length() + 1 ];
	std::strcpy (cstr, s.c_str());

	char * pch = std::strtok (cstr,delimiter);
	while (pch !=nullptr)
	{
		list.insert(pch);
		pch = std::strtok (nullptr, delimiter);
	}
	delete[] cstr;
	return list;
}
const std::vector<std::string> parseDelimitedVectorList
	(const std::string& s, const char& c)
{
	std::string b{""};
	std::vector<std::string> v;

	for (auto n:s)
	{
		if(n != c) b+=n; else
		if(n == c && b != "")
		{
			v.push_back(b);
			b="";
		}
	}
	if(b != "")
		v.push_back(b);
	return v;
}
const std::list<std::string> parseDelimitedListList
	(const std::string& s, const char& c)
{
	std::string b{""};
	std::list<std::string> l;

	for (auto n:s)
	{
		if(n != c) b+=n; else
		if(n == c && b != "")
		{
			l.push_back(b);
			b="";
		}
	}
	if(b != "")
		l.push_back(b);
	return l;
}
std::string stripWhiteSpace(const std::string& s)
{
	if (s.empty())
		return s;

	int pos = 0;
	std::string line = s;
	int len = line.length();
	while ( pos < len && isspace(line[pos]) )
		++pos;

	line.erase( 0, pos );
	pos = line.length()-1;
	while ( pos > -1 && isspace(line[pos]) )
		--pos;

	if (pos != -1)
		line.erase( pos+1 );

	return line;
}
bool startsWithNoCase(const std::string& s1, const std::string& s2)
{
	std::string::const_iterator p1 = s1.begin();
	std::string::const_iterator p2 = s2.begin();

	while ( p1 != s1.end() && p2 != s2.end() ) {
		if ( toupper( *p1 ) != toupper( *p2 ) )
			return false;

		++p1;
		++p2;
	}
	if ( p1 == s1.end() && p2 != s2.end() )
		return false;

	return true;
}
std::string convertToLowerCase(const std::string& s)
{
	std::string result = "";
	for (std::string::size_type i = 0; i < s.length(); ++i)
		result += tolower( s[i] );

	return result;
}
std::string convertToUpperCase(const std::string& s)
{
	std::string result = "";
	for ( std::string::size_type i = 0; i < s.length(); ++i )
		result += toupper( s[i] );

	return result;
}
std::string replaceAll( std::string& in, const std::string& oldString, const std::string& newString )
{
	size_t pos;
	while ( (pos = in.find( oldString )) != std::string::npos )
		in = in.replace( pos, oldString.length(), newString );

	return in;
}
