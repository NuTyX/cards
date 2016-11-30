//  webcards.cxx
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
using namespace Sql;

void endOfPage(void)
{
	cout << "   </tr>" <<endl
	<< "  </table>" << endl
	<< " </body>" << endl
	<< "</html>" << endl;
}
string::size_type parseArguments(arguments_t &arguments)
{
	/* All the possible arguments */
	set<string> listOfArguments;
	char * pArgument = getenv ("QUERY_STRING");
	string::size_type pos;
	listOfArguments = parseDelimitedSetList(pArgument,'&');
	arguments.docName="index";
	for ( auto i : listOfArguments) {
		pos = i.find("arch=");
		if ( pos != string::npos ){
			arguments.packageArch = i.substr(pos+5);
		}
		pos = i.find("branch=");
		if ( pos != string::npos ){
			arguments.packageBranch = i.substr(pos+7);
		}
		pos = i.find("page=");
		if ( pos != string::npos ){
			arguments.docName = i.substr(pos+5);
		}
		pos = i.find("search=");
		if ( pos != string::npos ){
			arguments.stringSearch = i.substr(pos+7);
		}
		pos = i.find("searchpkg=");
		if ( pos != string::npos ){
			arguments.packageSearch = i.substr(pos+10);
		}
		pos = i.find("type=");
		if ( pos != string::npos ){
			arguments.type = i.substr(pos+5);
		}
	}
	if ( arguments.packageArch.size() == 0)
		arguments.packageArch="x86_64";
	if ( arguments.packageBranch.size() == 0)
		arguments.packageBranch="stable";
	if ( arguments.type.size() == 0)
		arguments.type="pkg";
	return pos;
}
void searchpkg(contentInfo_t &contentInfo, arguments_t &arguments)
{
	contentInfo.text.push_back( "  <form method=\"get\" action=\".\">");
	contentInfo.text.push_back( "  <fieldset>");
	contentInfo.text.push_back( "   <table>");
	contentInfo.text.push_back( "    <tr class=\"odd\">");
	contentInfo.text.push_back( "     <td width=\"20%\">");
	contentInfo.text.push_back( "      <h4>Type</h4>");
	contentInfo.text.push_back( "     <td>");
	if ( arguments.type == "pkg" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"type\" value=\"pkg\" checked=\"checked\"/> PACKAGE" );
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"type\" value=\"pkg\" /> PACKAGE" );
	if ( arguments.type == "col" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"type\" value=\"col\" checked=\"checked\"/> COLLECTION" );
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"type\" value=\"col\" /> COLLECTION" );
	contentInfo.text.push_back( "    <tr class=\"even\">");
	contentInfo.text.push_back( "     <td>");
	contentInfo.text.push_back( "      <h4>Branch</h4>");
	contentInfo.text.push_back( "     <td>");
	if ( arguments.packageBranch == "all" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"all\" checked=\"checked\"/> All" );
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"all\" /> All");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageBranch == "stable" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"stable\" checked=\"checked\"/> stable");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"stable\" /> stable");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageBranch == "development" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"development\" checked=\"checked\" /> development");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"development\" /> development");
	contentInfo.text.push_back( "    <tr class=\"odd\">");
	contentInfo.text.push_back( "     <td>");
	contentInfo.text.push_back( "      <h4>Arch</h4>");
	contentInfo.text.push_back( "     <td>");
	if ( arguments.packageArch == "all" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"all\" checked=\"checked\" /> All");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"all\" /> All");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageArch == "i686" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"i686\" checked=\"checked\" /> i686");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"i686\" /> i686");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageArch == "x86_64" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"x86_64\" checked=\"checked\" /> x86_64");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"x86_64\" /> x86_64");
	contentInfo.text.push_back( "    <tr class=\"even\" valign=\"middle\">");
	contentInfo.text.push_back( "     <td>");
	contentInfo.text.push_back( "      <h4>Search ...</h4>");
	contentInfo.text.push_back( "     <td>");
	contentInfo.text.push_back( "        <input id=\"search-field\" type=\"text\" name=\"searchpkg\" size=\"18\" maxlength=\"200\" value=\""
		+ arguments.packageSearch + "\" />");
	contentInfo.text.push_back( "   </table>");
	contentInfo.text.push_back( "  </fieldset>");
	contentInfo.text.push_back( "  </form>");
}
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
contentInfo_t getFormatedBinaryPackageList(arguments_t &arguments)
{
	string search = arguments.packageSearch;
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
			if ( arguments.type == "col" ) {
				if ( convertToLowerCase(search) == i.collection ) {
					if ( ( arguments.packageBranch != i.branch ) &&
						( arguments.packageBranch != "all" ) )
						continue;
					if ( ( arguments.packageArch != i.arch ) &&
						( arguments.packageArch != "all" )  )
						continue;
					INSERTPACKAGE(j.basePackageName);
					continue;
				}
				continue;
			}
			if ( search.size() == 0 ) {
				INSERTPACKAGE(j.basePackageName);
			} else {
				if ( ( arguments.packageBranch != i.branch ) &&
					( arguments.packageBranch != "all" ) )
					continue;
				if ( ( arguments.packageArch != i.arch ) &&
					( arguments.packageArch != "all" )  )
					continue;
				string::size_type pos;
				pos = i.collection.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE(j.basePackageName);
					continue;
				}
				pos = j.basePackageName.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE(j.basePackageName);
					continue;
				}
				pos = j.description.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE(j.basePackageName);
					continue;
				}
				set<string> groupList;
				groupList = parseDelimitedSetList(j.group,' ');
				for ( auto k : groupList ) {
					if ( convertToLowerCase(search) == k ) {
						string name = j.basePackageName
							+ "."
							+ k;
						INSERTPACKAGE(name);
					}
				}
			}
		}
	}
	contentInfo.text.push_back("<h1>NuTyX Packages</h1>");
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
	searchpkg(contentInfo,arguments);
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
	/* All the contents */
	content_t Content;
	Content = getContent(ArticleNamesList);

	string::size_type pos;
	arguments_t arguments;

	char * pArgument = getenv ("QUERY_STRING");
	/* If no arguments, no need to go on */
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

	/* If PATH is not found, no need to continue */
	if ( !pPwd)
		return 0;

	string  sPwd =  pPwd;
	pos = sPwd.find_last_of( "/\\" );

	/* If PATH is not found, no need to continue */
	if ( pos == string::npos )
		return 0;

	string sPath = sPwd.substr(0,pos);

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

	/* Parse all knows argument so far */
	pos = parseArguments(arguments);

/*	if  ( pos == string::npos )
		return 0;
*/
	if ( arguments.packageSearch.size() > 0 ) {
		if (arguments.packageSearch.size() < 2) {
			cout << "<div class=\"note\"><img alt=\"[Note]\" \
			src=\"../graphics/note.gif\" />min 2 characters...</div>" << endl;
		} else {
			Content["packages"] = getFormatedBinaryPackageList
				(arguments);
			if ( Content["packages"].text.size() > 0) {
				lastUpdate(Content["packages"].date);
				for (auto i : Content["packages"].text)
					cout << i << endl;
			}
		}
		FOOTERTEXT;
		endOfPage();
		return 0;
	}
	if ( arguments.stringSearch.size() > 0) {
		if (arguments.stringSearch.size() < 3) {
			cout << endl
			<< "<div class=\"note\"><img alt=\"[Note]\" \
			src=\"../graphics/note.gif\" />min 3 characters...</div>" << endl;
		} else {
			arguments.docName = "packages";
			vector<string> searchList;
			for (auto i : Content) {
				for (auto j : i.second.text) {
					if ( i.first == "packages" )
						continue;
					string lower = convertToLowerCase(j);
					pos = lower.find(convertToLowerCase(arguments.stringSearch));
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
		FOOTERTEXT;
		endOfPage();
		return 0;
	}
	if ( arguments.docName == "packages" ) {
		contentInfo_t contentInfo;
		contentInfo.text.push_back("<h1>NuTyX Packages</h1>");
		searchpkg(contentInfo,arguments);
		for (auto i : contentInfo.text) cout << i << endl;
		FOOTERTEXT;
		endOfPage();
		return 0;
	}
	if ( Content.find(arguments.docName) != Content.end() ){
		SEARCH;
		lastUpdate(Content[arguments.docName].date);
		for (auto i : Content[arguments.docName].text) cout << i << endl;
		FOOTERTEXT;
		endOfPage();
		return 0;
	}
	else
	{
		cout << "<h1>" << arguments.docName << " is not existing yet</h1>"
		<< endl;
		for (auto i : Content["under-construction"].text) cout << i << endl;
		FOOTERTEXT;
		endOfPage();
		return 0;
	}
	return 0;
}
// vim:set ts=2 :
