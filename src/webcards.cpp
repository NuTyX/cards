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
void printFormatedBinaryPackageList(void)
{
	string row = "odd";
	vector<RepoInfo> List;
	Pkgrepo repoList(".webcards.conf");
	List = repoList.getRepoInfo();
	cout << "<h1>NuTyX Packages 8.2</h1>" << endl
		<< " <table>" << endl
		<< "  <tr class=\"header\">" << endl
		<< "   <td>ARCH</td>"
		<< "   <td>COLLECTION</td>"
		<< "   <td>NAME</td>"
		<< "   <td>VERSION</td>"
		<< "   <td>DESCRIPTION</td>"
		<< "   <td>DATE</td>" << endl
		<< "  </tr>" << endl
		<< "  <tbody id=\"fbody\">" << endl;
	for (auto i : List) {
		for (auto j : i.basePackageList) {
			cout << "   <tr class=\"" <<  row << "\">"
				<< endl
				<< "    <td>" << i.arch << "</td>"
				<< "<td>" << i.collection << "</td>"
				<< "<td>" << j.basePackageName << "</td>"
				<< "<td>" << j.version << "-" << j.release << "</td>"
				<< "<td>" << j.description << "</td>"
				<< "<td>" << getDateFromEpoch(j.buildDate) 
				<< "</td>" << endl
				<< "   </tr>" << endl;
			if ( row=="odd")
				row="even";
			else
				row="odd";
		}
	}
	cout << " </trbody>" << endl
		<< " </table>" << endl;
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

	if ( docName == "packages" ) {
		cout << "<table width=\"25%\" cellspacing=\"0\">" << endl
			<< "<tr class=\"header\">" << endl
			<< "<td><input id=\"searchInput\" placeholder=\"Search for ... \"></td>" << endl
			<< "</tr>" << endl
			<< "</table>" << endl
			<< "<table>" << endl ;
		printFormatedBinaryPackageList();
		cout << "<script src=\"https://code.jquery.com/jquery-2.1.4.min.js\"></script>" 
			<< endl
			<< "<script>" << endl
			<< "jQuery(function() {" << endl
			<< "jQuery(\"#searchInput\").keyup(function() {" << endl
			<< "         var rows = jQuery(\"#fbody\").find(\"tr\").hide();" << endl
			<< "         var data = this.value.split(\" \");" << endl
			<< "         jQuery.each(data, function(i, v) {" << endl
			<< "         v = v.toLowerCase();" << endl
			<< "         rows.filter(function() {" << endl
			<< "         return (this.textContent || this.innerText || this.text() || \"\").toLowerCase().indexOf(v) >= 0;" << endl
			<< "        })" << endl
			<< "       .show();" << endl
			<< "     });" << endl
			<< "   	});" << endl
			<< "   });" << endl
			<< "  </script>" << endl;
	} else {
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
	}
	FOOTERTEXT;
	return 0;
}
// vim:set ts=2 :
