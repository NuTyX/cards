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
contentInfo_t getFormatedBinaryPackageList()
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
			listOfPackages.insert("<td>" \
+ i.arch + "</td>" \
+ "<td>" + i.branch + "</td>"  \
+ "<td>" + i.collection + "</td>" \
+ "<td>" + j.basePackageName + "</td>" \
+ "<td>" + j.version + "-" + itos(j.release) + "</td>" \
+ "<td>" + j.description + "</td>" \
+ "<td>" + getDateFromEpoch(j.buildDate) + "</td>");
		}
	}
	contentInfo.text.push_back("<h1>NuTyX Packages 8.2</h1>");
	contentInfo.text.push_back(" <h2>" + itos(listOfPackages.size()) \
+ " packages founds</h2>");
  contentInfo.text.push_back("<table width=\"25%\" cellspacing=\"0\">");
  contentInfo.text.push_back("<tr class=\"header\">");
  contentInfo.text.push_back("<td><input id=\"searchInput\" \
placeholder=\"Search for ... \"></td>");
  contentInfo.text.push_back("</tr>");
  contentInfo.text.push_back("</table>");
  contentInfo.text.push_back("<table>");
	contentInfo.text.push_back("  <tr class=\"header\">");
	contentInfo.text.push_back("  <td>ARCH</td><td>BRANCH</td>\
<td>COLLECTION</td>\
<td>NAME</td>\
<td>VERSION</td>\
<td>DESCRIPTION</td>\
<td>UPDATE</td>");
	contentInfo.text.push_back("<tbody id=\"fbody\">");
	for (auto i : listOfPackages ){
			contentInfo.text.push_back("<tr class=\"" +  row + "\">");
			contentInfo.text.push_back(i);
			contentInfo.text.push_back("</tr>");
			if ( row=="odd")
				row="even";
			else
				row="odd";
	}
	contentInfo.text.push_back(" </trbody>");
	contentInfo.text.push_back(" </table>");
  contentInfo.text.push_back("<script \
src=\"https://code.jquery.com/jquery-2.1.4.min.js\"></script>");
  contentInfo.text.push_back("<script>");
  contentInfo.text.push_back("jQuery(function() {");
  contentInfo.text.push_back("jQuery(\"#searchInput\").keyup(function() {");
  contentInfo.text.push_back("         var rows = jQuery(\"#fbody\").find(\"tr\").hide();");
  contentInfo.text.push_back("         var data = this.value.split(\" \");");
  contentInfo.text.push_back("         jQuery.each(data, function(i, v) {");
  contentInfo.text.push_back("         v = v.toLowerCase();");
  contentInfo.text.push_back("         rows.filter(function() {");
  contentInfo.text.push_back("         return (this.textContent || \
this.innerText || this.text() \
|| \"\").toLowerCase().indexOf(v) >= 0;");
  contentInfo.text.push_back("        })");
  contentInfo.text.push_back("       .show();");
  contentInfo.text.push_back("     });");
  contentInfo.text.push_back("    });");
  contentInfo.text.push_back("   });");
  contentInfo.text.push_back("  </script>");

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

	string docName = "index";
	pos = sArgument.find("page=");
	if ( pos != string::npos )
		docName = sArgument.substr(pos+5);
	if ( docName == "packages" )
		Content[docName] = getFormatedBinaryPackageList();
	if ( Content.find(docName) != Content.end() ){
		cout << "<table border=\"0\" cellpadding=\"15\" cellspacing=\"10\" width=\"100%\">"
		<< "<tr valign=\"top\">"
		<< "<td valign=\"top\" align=\"left\" width=\"100%\">"
		<< "<p class=\"updated\"> "
		<< Content[docName].date
		<< " UTC</p>" << endl;
		for (auto i : Content[docName].text) cout << i << endl;
	} else {
		cout << "<h1>" << docName << " is not existing yet</h1>"
			<< endl;
		docName="under-construction";
		for (auto i : Content[docName].text) cout << i << endl;
	}
	FOOTERTEXT;
	return 0;
}
// vim:set ts=2 :
