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

#include "webcards.h"
#include "www/config_webcards.h"
#include "www/menu_webcards.h"

using namespace std;

content_t getContent(std::set<string>& list)
{
  content_t content;
  for ( auto i : list) {
   string fullName = "content/" + i;
   contentInfo_t contentInfo;
   contentInfo.date = getModifyTimeFile(fullName);
	 vector<string> contentFile;
	 parseFile(contentInfo.text,fullName.c_str());
	 content[i] = contentInfo;
  }
	return content;
}

int main (int argc, char** argv)
{
	set<string> ArticleNamesList;
	findFile(ArticleNamesList, "content/");
	content_t Content;
	Content = getContent(ArticleNamesList);
	string::size_type pos;
	char * pPwd = getenv ("SCRIPT_NAME");

	if ( !pPwd)
		return 0;

	char * pArgument = getenv ("QUERY_STRING");

	if ( !pArgument)
    return 0;

	string  sPwd =  pPwd;
	pos = sPwd.find_last_of( "/\\" );
	if ( pos == string::npos )
		return 0;

	string sPath = sPwd.substr(0,pos);
	string sArgument = pArgument;

	HEADERTEXT;
	CSSDATA;
	pos = sPath.find_last_of( "/\\" );

	if ( pos == string::npos )
		return 0;

	string sLang = sPath.substr(pos+1);
	if ( sLang == "fr" )
		MENUFR;
	else if ( sLang == "tr" )
		MENUTR;
	else
		MENUEN;

	string docName = "index";
	pos = sArgument.find("page=");
	if ( pos != string::npos )
		docName = sArgument.substr(pos+5);

	if ( Content.find(docName) != Content.end() ){
		cout << "<table border=\"0\" cellpadding=\"15\" cellspacing=\"10\" width=\"100%\">"
		<< "<tr valign=\"top\">"
		<< "<td valign=\"top\" align=\"left\" width=\"100%\">"
		<< "<p class=\"updated\"> "
		<< Content[docName].date
		<< " UTC</p>" << endl;
		for (auto i : Content[docName].text) cout << i << endl;
	} else {
		docName="under-construction";
		for (auto i : Content[docName].text) cout << i << endl;
	}
	FOOTERTEXT;
	return 0;
}
// vim:set ts=2 :
