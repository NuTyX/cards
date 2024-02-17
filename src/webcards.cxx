//  webcards.cxx
//
//  Copyright (c) 2017 - 2024 by NuTyX team (http://nutyx.org)
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

std::vector<std::string> parseHTMLDelimitedList
(const std::vector<std::string>& text,
	const std::string startTagValue,
	const std::string contentTitle )
{
	std::string::size_type start = 0, end = 0;
	std::vector<std::string> page;
	std::vector<std::string> body;
	std::string endTagValue = "</";
	int previousHeaderLevel = '2';
	/* First pass to check if we have h2, h3, h3, h4, h5 , h6 paragraphs
     h1 can only be ones at the first line */
	bool haveContent=false;
	int ref=0;
	for (auto line : text) {
		start = 0;
		end = 0;
		start = line.find(startTagValue);

		/* line contains startTag  not found, just copy the line*/
		if ( start == std::string::npos) {
			/* if they is no content yet we go on with copy */
			if (! haveContent ) {
				page.push_back(line);
			} else {
				body.push_back(line);
			}
			continue;
		}
		int headerLevel =  line[start+2];
		/* line contains <h1>... is the title, this should happens only once */
		if ( headerLevel == '1' ) {
			page.push_back(line);
			continue;
		}

		/* line contains first '>' after startTag found ? */
		start = line.find('>', start + 1);
		if ( start == std::string::npos) {
			page.push_back(line);
			continue;
		}

		/* start is pos of first character of our line to add into contents */
		start++;
		end = line.find(endTagValue);
		/* line contains endTag
		We store the line as it is*/
		if ( end != std::string::npos) {
			/* First time we find a paragraph header ? */
			if ( ! haveContent ) {
				page.push_back( "<div class=\"toc\">\n <h2>");
				page.push_back( contentTitle );
				page.push_back( "</h2>");
				page.push_back( "  <ul style=\"list-style-type: none; padding: 0;\">" );
			}
			haveContent = true;
			ref++;

			std::string sRef = "";
			if ( headerLevel != previousHeaderLevel ) {
				int i = headerLevel ;
				while ( i > previousHeaderLevel ) { /* means > '2' */
					sRef += "<li>\n <ul style=\"list-style-type: none;\">";
					i--;
				}
				i = headerLevel ;
				while ( i < previousHeaderLevel ) {
					sRef += "  </ul>\n</li>";
					i++;
				}
			}
			sRef += "<li><a href=\"#";
			sRef += itos(ref);
			sRef += "\">";
			sRef += line.substr(start, end - start);
			sRef += "</a>";
			page.push_back(sRef);
			string Newline = "";
			Newline += "<a name=\"";
			Newline += itos(ref);
			Newline += "\"></a>";
			Newline += line;
			body.push_back(Newline);
		}
		/* We'll need to know what was the previous level */
		previousHeaderLevel = headerLevel;
	}
	if ( haveContent )
		page.push_back( "</ul>\n</div>");

	/* they is no content, page is completed */
	if (! haveContent )
		return page;

	for (auto line : body) {
		page.push_back(line);
	}
	return page;
}
void visitOfPage(char * argument)
{
	FILE *pVisits = fopen("content/.visits","r+");
	if (pVisits != NULL) {
		time_t timer;
		time(&timer);
		fseek(pVisits,0,SEEK_END);
		fprintf(pVisits,"%d,%s\n",timer,argument);
		fclose(pVisits);
	}
}
void endOfPage(void)
{
	std::cout << "   </tr>"
		<< std::endl
		<< "  </table>"
		<< std::endl
		<< " </body>"
		<< std::endl;
}
string::size_type parseArguments(arguments_t &arguments)
{
	/* All the possible arguments */
	std::set<std::string> listOfArguments;
	char * pArgument = getenv ("QUERY_STRING");
	std::string::size_type pos;
	listOfArguments = parseDelimitedSetList(pArgument,"&");
	arguments.docName="index";
	for ( auto i : listOfArguments) {
		pos = i.find("arch=");
		if ( pos != std::string::npos ){
			arguments.packageArch = i.substr(pos+5);
		}
		pos = i.find("branch=");
		if ( pos != std::string::npos ){
			arguments.packageBranch = i.substr(pos+7);
		}
		pos = i.find("page=");
		if ( pos != std::string::npos ){
			arguments.docName = i.substr(pos+5);
		}
		pos = i.find("search=");
		if ( pos != std::string::npos ){
			arguments.stringSearch = i.substr(pos+7);
		}
		pos = i.find("searchpkg=");
		if ( pos != std::string::npos ){
			arguments.packageSearch = i.substr(pos+10);
		}
		pos = i.find("type=");
		if ( pos != std::string::npos ){
			arguments.type = i.substr(pos+5);
		}
	}
	if ( arguments.packageArch.size() == 0)
		arguments.packageArch="x86_64";
	if ( arguments.packageBranch.size() == 0)
		arguments.packageBranch="rolling";
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
	contentInfo.text.push_back( "      <h4>Release</h4>");
	contentInfo.text.push_back( "     <td>");

	if ( arguments.packageBranch == "rolling" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"rolling\" checked=\"checked\" /> Rolling");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"rolling\" /> Rolling");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageBranch == "testing" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"testing\" checked=\"checked\"/> Testing");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"testing\" /> Testing");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageBranch == "all" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"all\" checked=\"checked\"/> All" );
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"branch\" value=\"all\" /> All");

	contentInfo.text.push_back( "    <tr class=\"odd\">");
	contentInfo.text.push_back( "     <td>");
	contentInfo.text.push_back( "      <h4>Architecture</h4>");
	contentInfo.text.push_back( "     <td>");
	if ( arguments.packageArch == "x86_64" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"x86_64\" checked=\"checked\" /> x86_64");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"x86_64\" /> x86_64");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageArch == "i686" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"i686\" checked=\"checked\" /> i686");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"i686\" /> i686");
	contentInfo.text.push_back( "     <br>");
	if ( arguments.packageArch == "all" )
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"all\" checked=\"checked\" /> All");
	else
		contentInfo.text.push_back( "      <input type=\"radio\" name=\"arch\" value=\"all\" /> All");

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
	std::cout << "  <td class=\"sidebar\" width=\"20%\">"
		<< std::endl
		<< "   <h4>Forum</h4>\n"
		<< "    <div>"
		<< std::endl;
	mysql forumDB("content/.mysql.conf");
	forumDB.lastPosts(forum);
	std::cout << "\n    </div>"
		<< std::endl
		<< "<hr>";

}
void postedMessages ( const char *forum, const char *boardId)
{
		mysql forumDB("content/.mysql.conf");
		forumDB.lastPosts(forum,boardId,10);

}
void lastUpdate(std::string& date)
{
	std::cout << "    <p class=\"updated\"> "
		<< date
		<< " UTC</p>"
		<< std::endl;
}
content_t getContent(std::set<string>& list)
{
  content_t content;
  for ( auto i : list) {
		keyValue pv = splitKeyValue(i,'.');
		std::string fullName = "content/" + i;
		contentInfo_t contentInfo;
		contentInfo.date = getModifyTimeFile(fullName);
		contentInfo.boardId= pv.value;
		parseFile(contentInfo.text,fullName.c_str());

		content[pv.parameter] = contentInfo;
	}
	return content;
}
contentInfo_t getFormatedBinaryPackageList(arguments_t &arguments)
{
	std::string search = arguments.packageSearch;
	std::vector<std::string> v_search = parseDelimitedVectorList(search, "%");
	for (auto i : v_search ) {
		if ( i.size() > 0) {
			search = i;
			break;
		}
	}
	v_search = parseDelimitedVectorList(search, "+");
	for (auto i : v_search ) {
		if ( i.size() > 0) {
			search = i;
			break;
		}
	}
	time_t timer;
	time(&timer);
	std::string row = "odd";
	contentInfo_t contentInfo;
	contentInfo.date = getDateFromEpoch(timer);
	std::vector<RepoInfo> List;
	Pkgrepo repoList(".webcards.conf");
	std::set<std::string> listOfPackages;
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
					INSERTPACKAGE(j.basePackageName, j.basePackageName);
					continue;
				}
				continue;
			}
			if ( search.size() == 0 ) {
				INSERTPACKAGE(j.basePackageName, j.basePackageName);
			} else {
				if ( ( arguments.packageBranch != i.branch ) &&
					( arguments.packageBranch != "all" ) )
					continue;
				if ( ( arguments.packageArch != i.arch ) &&
					( arguments.packageArch != "all" )  )
					continue;
				std::string::size_type pos;
				pos = i.collection.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE(j.basePackageName, j.basePackageName);
					continue;
				}
				pos = j.basePackageName.find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE(j.basePackageName, j.basePackageName);
					continue;
				}
				pos = convertToLowerCase(j.description).find(convertToLowerCase(search));
				if (pos != std::string::npos) {
					INSERTPACKAGE(j.basePackageName, j.basePackageName);
					continue;
				}
				std::set<std::string> groupList;
				groupList = parseDelimitedSetList(j.group," ");
				for ( auto k : groupList ) {
					if ( convertToLowerCase(search) == k ) {
						std::string name = j.basePackageName
							+ "."
							+ k;
						INSERTPACKAGE(j.basePackageName, name);
					}
				}
				std::set<std::string> aliasList;
				aliasList = parseDelimitedSetList(j.alias," ");
				for ( auto k : aliasList ) {
					if ( convertToLowerCase(search) == k ) {
						std::string name = j.basePackageName;
						INSERTPACKAGE(j.basePackageName, name);
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
	contentInfo.text.push_back("  <td>ARCH</td><td>RELEASE</td>\
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
	using namespace std::chrono;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	std::set<std::string> ArticleNamesList;

	findDir(ArticleNamesList, "content/");
	/* All the contents */
	content_t Content;
	Content = getContent(ArticleNamesList);

	std::string::size_type pos;
	arguments_t arguments;

	char * pArgument = getenv ("QUERY_STRING");
	/* If no arguments, no need to go on */
	if ( !pArgument)
		return 0;

	char * pPwd = getenv ("SCRIPT_NAME");

	/* If PATH is not found, no need to continue */
	if ( !pPwd)
		return 0;

	std::string  sPwd =  pPwd;
	pos = sPwd.find_last_of( "/\\" );

	/* If PATH is not found, no need to continue */
	if ( pos == string::npos )
		return 0;

	std::string sPath = sPwd.substr(0,pos);

	HEADERTEXT;
	CSSDATA;
	pos = sPath.find_last_of( "/\\" );

	if ( pos == std::string::npos )
		return 0;

	std::string sLang = sPath.substr(pos+1);
	const char * tocTitle;
	const char * search;
	if ( sLang == "fr" ) {
		MENUFR;
		tocTitle = "Sommaire";
		search = "Recherche ...";

	} else if ( sLang == "tr" ) {
		MENUTR;
		search = "Ara ...";
		tocTitle = "İçindekiler ";
	 } else {
		MENUEN;
		search = "Search ...";
		tocTitle = "Contents";
	}
	/* The main table */
	std::cout << "<table border=\"0\" cellpadding=\"15\" \
cellspacing=\"10\" width=\"100%\">"
		<< std::endl
  		<< " <tr valign=\"top\">"
		<< std::endl;
	const char * forumAdress;
	if ( sLang == "fr" )
	{
		forumAdress = "https://forum.nutyx.org";
	} else{
		forumAdress = "https://forums.nutyx.org";
	}
	sideBar(forumAdress);
	std::cout << "  <td valign=\"top\" align=\"left\" width=\"100%\">"
		<< std::endl;

	/* Parse all knows argument so far */
	pos = parseArguments(arguments);

	if  ( pos == string::npos ) {
		std::cout << "Wrong arguments"
			<< std::endl;
		return 0;
	}
	if ( arguments.packageSearch.size() > 0 ) {
		visitOfPage(pArgument);
		if (arguments.packageSearch.size() < 2) {
			std::cout << "<div class=\"note\"><img alt=\"[Note]\" \
			src=\"../graphics/note.gif\" />min 2 characters...</div>"
				<< std::endl;
		} else {
			Content["packages"] = getFormatedBinaryPackageList
				(arguments);
			if ( Content["packages"].text.size() > 0) {
				lastUpdate(Content["packages"].date);
				for (auto i : Content["packages"].text)
					std::cout << i << std::endl;
			}
		}
	} else if ( arguments.stringSearch.size() > 0) {
		if (arguments.stringSearch.size() < 3) {
			std::cout << std::endl
				<< "<div class=\"note\"><img alt=\"[Note]\" \
			src=\"../graphics/note.gif\" />min 3 characters...</div>"
				<< std::endl;
		} else {
			arguments.docName = "packages";
			std::vector<std::string> searchList;
			for (auto i : Content) {
				for (auto j : i.second.text) {
					if ( i.first == "packages" )
						continue;
					std::string lower = convertToLowerCase(j);
					pos = lower.find(convertToLowerCase(arguments.stringSearch));
					if (pos != std::string::npos) {
						searchList.push_back("<a href=\"?page=" + i.first
						+ "\">" + i.first + "</a><br>...<br>");
						searchList.push_back(j);
						searchList.push_back("</div></pre>...<br><br>");
					}
				}
			}
			for ( auto i : searchList) std::cout << i << std::endl;
		}
	} else if ( arguments.docName == "packages" ) {
		contentInfo_t contentInfo;
		contentInfo.text.push_back("<h1>NuTyX Packages</h1>");
		searchpkg(contentInfo,arguments);
		for (auto i : contentInfo.text) std::cout << i << std::endl;
	} else if ( Content.find(arguments.docName) != Content.end() ){
		SEARCH;
		lastUpdate(Content[arguments.docName].date);
		std::vector<std::string> page = parseHTMLDelimitedList( Content[arguments.docName].text,
			"<h",
			tocTitle);
		for (auto i : page) std::cout << i << std::endl;
		if (Content[arguments.docName].boardId.size() > 0) {
			const char * forumAdress;

			std::cout << "<p>"
				<< std::endl
				<< "<a href=\"https://";
			if ( sLang == "fr" ) {
				forumAdress = "https://forum.nutyx.org";
				std::cout << "forum.nutyx.org/index.php/board,"
					<< Content[arguments.docName].boardId
					<< "\">Soyez le premier à venir commenter le sujet.</a>";
/*
 * 				postedMessages(forumAdress,Content[arguments.docName].boardId.c_str());
 */
			} else {
				forumAdress = "https://forums.nutyx.org";
				std::cout << "forums.nutyx.org/index.php/board,"
					<< Content[arguments.docName].boardId
					<< "\">Be the first to comment on this subject.</a>";
/*
 * 				postedMessages(forumAdress,Content[arguments.docName].boardId.c_str());
 */
			}
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "<h1>"
			<< arguments.docName
			<< " is not existing yet</h1>"
			<< std::endl;
		for (auto i : Content["under-construction"].text)
			std::cout << i << std::endl;
	}

	FOOTERTEXT;
	endOfPage();

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>((t2 - t1)*1000);
	std::cout << time_span.count() << " mS"
		<< std::endl
		<< "</html>"
		<< std::endl;

	return 0;
}
// vim:set ts=2 :
