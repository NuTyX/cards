//  webcards.cpp
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

void sideBar( const char *forum)
{
	cout << "  <td class=\"sidebar\" width=\"20%\">" << endl
		<< "   <h4>Forum</h4>" << endl
		<< "    <div>" << endl;
	mysql forumDB("content/.mysql.conf");
	forumDB.lastPosts(forum,10);
	cout << "    </div>" << endl
		<< "<hr>";

}
void lastUpdate(std::string& date)
{
	cout << "    <p class=\"updated\"> "
	<< date
	<< " UTC</p>" << endl;
}
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
contentInfo_t getFormatedBinaryPackageList(string& search)
{
	time_t timer;
	time(&timer);
	string row = "odd";
	contentInfo_t contentInfo;
	contentInfo.date = getDateFromEpoch(timer);
	vector<RepoInfo> List;
	Pkgrepo repoList(".webcards.conf");
	set<string> listOfPackages;
	List = repoList.getRepoInfo();
	for (auto i : List) {
		for (auto j : i.basePackageList) {
			if ( search.size() == 0 ) {
				INSERTPACKAGE;
			} else {
				string::size_type pos;
				pos = i.collection.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE;
					continue;
				}
				if ( convertToLowerCase(search) == j.basePackageName ) {
					INSERTPACKAGE;
					continue;
				}
				pos = j.description.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE;
					continue;
				}
			}
		}
	}
	contentInfo.text.push_back("<h1>NuTyX Packages 8.2</h1>");
	if (listOfPackages.size() == 0 )
		contentInfo.text.push_back(" <h2>no matching package found</h2>");
	if (listOfPackages.size() == 1 ) {
		contentInfo.text.push_back(" <h2>" + itos(listOfPackages.size()) \
+ " package found</h2>");
	}
	if (listOfPackages.size() >1) {
		contentInfo.text.push_back(" <h2>" + itos(listOfPackages.size()) \
+ " packages founds</h2>");
	}
	SEARCHPKG;
  contentInfo.text.push_back("<table>");
	contentInfo.text.push_back("  <tr class=\"header\">");
	contentInfo.text.push_back("  <td>ARCH</td><td>BRANCH</td>\
<td>COLLECTION</td>\
<td>NAME</td>\
<td>VERSION</td>\
<td>DESCRIPTION</td>\
<td>UPDATE</td>");
	contentInfo.text.push_back("<tbody id=\"fbody\">");
	if (search.size() > 0) {
		for (auto i : listOfPackages ){
			contentInfo.text.push_back("<tr class=\"" +  row + "\">");
			contentInfo.text.push_back(i);
			contentInfo.text.push_back("</tr>");
			if ( row=="odd")
				row="even";
			else
				row="odd";
		}
	}
	contentInfo.text.push_back(" </tbody>");
	contentInfo.text.push_back(" </table>");

	return contentInfo;
}
int main (int argc, char** argv)
{
	set<string> ArticleNamesList;
	findFile(ArticleNamesList, "content/");
	content_t Content;
	Content = getContent(ArticleNamesList);
	string::size_type pos;
	char * pArgument = getenv ("QUERY_STRING");
	if ( !pArgument)
		return 0;
	FILE *pVisits = fopen("content/.visits","r+");
	if (pVisits != NULL) {
		time_t timer;
		time(&timer);
		char * pIP = getenv ("REMOTE_ADDR");
		char * pRA = getenv ("HTTP_USER_AGENT");
		fseek(pVisits,0,SEEK_END);
		if (pIP)
			fprintf(pVisits,"%d,%s,%s,%s\n",timer,pIP,pRA,pArgument);
		else
			fprintf(pVisits,"%d,%s,%s,%s\n",timer,"NotAvailable",pRA,pArgument);
		fclose(pVisits);
	}

	char * pPwd = getenv ("SCRIPT_NAME");
	if ( !pPwd)
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
	/* The main table */
	cout << "<table border=\"0\" cellpadding=\"15\" \
cellspacing=\"10\" width=\"100%\">" << endl
  << " <tr valign=\"top\">" << endl;
	const char * forumAdress;
	const char * search;
	if ( sLang == "fr" )
	{
		forumAdress = "http://forum.nutyx.org";
		search = "Recherche ...";
	} else{
		forumAdress = "http://forums.nutyx.org";
		search = "Search ...";
	}
	sideBar(forumAdress);
	cout << "  <td valign=\"top\" align=\"left\" width=\"100%\">" << endl;
	string docName = "index";
	if  ( sArgument.size() < 1 ) {
		SEARCH;
		lastUpdate(Content[docName].date);
		for (auto i : Content[docName].text) cout << i << endl;
	}
	pos = sArgument.find("page=");
	if ( pos != string::npos )	{
		docName = sArgument.substr(pos+5);
		if ( docName == "packages" ) {
			string search = "";
			Content[docName] = getFormatedBinaryPackageList(search);
		}
		if ( Content.find(docName) != Content.end() ){
			if ( docName != "packages" )
				SEARCH;
			lastUpdate(Content[docName].date);
			for (auto i : Content[docName].text) cout << i << endl;
		} else {
			cout << "<h1>" << docName << " is not existing yet</h1>"
			<< endl;
			docName="under-construction";
			for (auto i : Content[docName].text) cout << i << endl;
		}
	}
	pos = sArgument.find("search=");
	if ( pos != string::npos )	{
		string sSearch = sArgument.substr(pos+7);
		if (sSearch.size() < 2) {
			cout << endl
			<< "<div class=\"note\"><img alt=\"[Note]\" \
			src=\"../graphics/note.gif\" />min 2 characters...</div>" << endl;
		} else {
			docName = "packages";
			vector<string> searchList;
			for (auto i : Content) {
				for (auto j : i.second.text) {
					if ( i.first == "packages" )
						continue;
					string lower = convertToLowerCase(j);
					pos = lower.find(convertToLowerCase(sSearch));
					if (pos != string::npos) {
						searchList.push_back("<a href=\"?page=" + i.first
						+ "\">" + i.first + "</a><br>...<br>");
						searchList.push_back(j);
						searchList.push_back("</div></pre>...<br><br>");
					}
				}
			}
			for ( auto i : searchList) cout << i << endl;
		}
	}
	pos = sArgument.find("searchpkg=");
	if ( pos != string::npos )	{
		string sSearch = sArgument.substr(pos+10);
		if (sSearch.size() < 2) {
			cout << "<div class=\"note\"><img alt=\"[Note]\" \
			src=\"../graphics/note.gif\" />min 2 characters...</div>" << endl;
		} else {
			docName = "packages";
			Content[docName] = getFormatedBinaryPackageList(sSearch);
			if ( Content[docName].text.size() > 0)
				for (auto i : Content[docName].text) cout << i << endl;
		}
	}
	FOOTERTEXT;

	/* This will never changer */

	cout << "   </tr>" <<endl
	<< "  </table>" << endl
  << " </body>" << endl
  << "</html>" << endl;

	return 0;
}
// vim:set ts=2 :
