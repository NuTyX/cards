// mysql.cxx
//
//  Copyright (c) 2016 - 2024 by NuTyX team (http://nutyx.org)
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
#include "mysql.h"

namespace Sql
{
mysql::mysql(const char *configFileName)
{
	cards::Conf m_sqlConfig(configFileName);
	m_socket=NULL;

	m_connection = mysql_init(NULL);
	mysql_options(m_connection, MYSQL_INIT_COMMAND, "SET NAMES utf8");
	mysql_real_connect(m_connection,
		m_sqlConfig.hostname().c_str(),
		m_sqlConfig.username().c_str(),
		m_sqlConfig.password().c_str(),
		m_sqlConfig.database().c_str(),
		m_port_no,
		m_socket,
		m_opt);
}
mysql::~mysql()
{
	if (m_connection)
		mysql_close(m_connection);
}
void mysql::lastPosts(const char *forum, int n)
{
	listOfBoards();
	listOfCategories();
	listOfMembers();
	listOfMessages();
	MYSQL_RES *res;
	MYSQL_ROW rows;
	std::vector<std::string> list;

	// retrieve the list of messages
	if(mysql_query(m_connection,
	"select id_topic,id_msg,id_member,poster_time,subject,icon,id_board \
		from smf_messages order by id_msg"))
			std::cout << mysql_error(m_connection)
				<< std::endl;
	res= mysql_use_result(m_connection);

	std::string sforum = forum;
	while ((rows = mysql_fetch_row(res)) != NULL) {
		std::string category = "<div style=\"text-transform: uppercase;\">";
		category += m_listOfCategories[ m_listOfBoards[rows[6]].category ] + ":</div>";
		std::string id_topic = rows[0];
		std::string id_msg = rows[1];
		std::string time = "<p class=\"updated\">";
		time +=getDateFromEpoch(strtoul(rows[3],NULL,0)) + " UTC</p>";
        std::string board = "<b>";
		board += m_listOfBoards[ rows[6] ].name;
		std::string author = "<i>";
		author += m_listOfMembers[ rows[2] ].real_name;
		author += "</i>";
		std::string subject = rows[4];
		std::string icon = rows[5];
		std::string message = time;
		message += category;
		message += board + "</b><br>";
		message += author;
		message += "<br><img src=\"../../graphics/" + icon;
		message += ".gif\" alt=\"\" /> <a href=\"" + sforum;
		message += "/index.php?topic=" + id_topic;
		message += ".msg" + id_msg;
		message += "#msg" + id_msg;
		message += "\">";
		message += subject;
		message += "</a><br><br>";
		list.push_back(message);
	}

	if (res)
		mysql_free_result(res);

	int i = 1;
	while ( i < n + 1) {
		std::cout << list[list.size()-i]
			<< std::endl;
		i++;
		if ( i != n + 1)
			std::cout << "<hr align=\"center\" style=\"width: 50%;\">"
				<< std::endl;
	}
}

void mysql::lastPosts(const char *forum, const char *id_board, int n)
{
				/*
	std::string sboard = id_board;
	std::string query = "select poster_name,subject,body,icon \
		from smf_messages where id_board = " \
			+ sboard + " order by id_msg";
	if(mysql_query(m_connection, query.c_str()))
		std::cout << mysql_error(m_connection)
						<< std::endl;
	m_result= mysql_use_result(m_connection);

	std::string sforum = forum;

	std::string message = "<h1>Messages</h1>\n";
	message+="<table>\n <tr class=\"header\">\n";
	message+="  <td>Author</td><td>Topic</td>\n";
	message+="   <tbody id=\"fbody\">\n";
	m_list.push_back(message);
	while ((m_rows = mysql_fetch_row(m_result)) != NULL) {
		std::string author=" <td><b>";
		author += m_rows[0];
		author += "</b></td>";
		message = "  <tr>\n";
		message += author;
		std::string subject="<td><b>";
		subject += m_rows[1];
		subject +="\n<p>\n<hr align=\"center\" style=\"width: 100%;\">\n";
		subject += "</b></td>\n";
		message += subject;
		message +="  </tr>\n  <tr>\n  <td></td><td>";
		message += m_rows[2];
		message +="\n</a><br><br>";
		message += "\n  </td>\n</tr>";
		m_list.push_back(message);

	}
	message = "</tbody>\n</table>";
	m_list.push_back(message);

	int i = 0;
	while ( i < m_list.size()) {
					std::cout << m_list[i]
									<< std::endl;
		i++;
	}
	*/
}

void mysql::listOfBoards()
{
	MYSQL_RES *res;
	MYSQL_ROW rows;
	boardInfo_t info;


	if(mysql_query(m_connection,
		"select id_board,name,id_cat \
			from smf_boards order by id_board"))
				std::cout << mysql_error(m_connection)
					<< std::endl;

	if (res= mysql_use_result(m_connection))
			std::cout << mysql_error(m_connection)
				<< std::endl;

	while ((rows = mysql_fetch_row(res)) != NULL) {
		info.category = rows[2];
		info.name = rows[1];
		m_listOfBoards[ rows[0] ] = info;
	}

	if (res)
		mysql_free_result(res);
}
void mysql::listOfCategories()
{
	MYSQL_RES *res;
	MYSQL_ROW rows;
	category_t info;

	if(mysql_query(m_connection,
	"select id_cat,name \
		from smf_categories order by id_cat"))
			std::cout << mysql_error(m_connection)
				<< std::endl;

	if(res= mysql_use_result(m_connection))
		std::cout << mysql_error(m_connection)
			<< std::endl;

	while ((rows = mysql_fetch_row(res)) != NULL) {
		m_listOfCategories[ rows[0] ] = rows[1];
	}

	if (res)
		mysql_free_result(res);
}
void mysql::listOfMembers()
{
	MYSQL_RES *res;
	MYSQL_ROW rows;
	memberInfo_t member;

	if(mysql_query(m_connection,
	"select id_member,member_name,real_name \
		from smf_members order by id_member"))
			std::cout << mysql_error(m_connection)
				<< std::endl;

	if(res= mysql_use_result(m_connection))
		std::cout << mysql_error(m_connection)
			<< std::endl;

	while (( rows = mysql_fetch_row(res)) != NULL) {
		member.member_name=rows[1];
		member.real_name=rows[2];
		m_listOfMembers[ rows[0] ] = member;
	}

	if (res)
		mysql_free_result(res);
}
void mysql::listOfMessages()
{
	MYSQL_RES *res;
	MYSQL_ROW  rows;
	messageInfo_t message;

	if(mysql_query(m_connection,
	"select id_msg,id_board,id_topic,id_member,poster_time,subject,icon \
		from smf_messages order by id_msg"))
			std::cout << mysql_error(m_connection)
				<< std::endl;

	if(res=mysql_use_result(m_connection))
		std::cout << mysql_error(m_connection)
			<< std::endl;

	while (( rows = mysql_fetch_row(res)) !=NULL) {
		message.id_board=rows[1];
		message.id_topic=rows[2];
		message.id_member=rows[3];
		message.poster_time=rows[4];
		message.subject=rows[5];
		message.icon=rows[6];

		m_listOfMessages [ rows[0] ] = message;

	}

	if(res)
		mysql_free_result(res);
}
} /* namespace Sql */
// vim:set ts=2 :
