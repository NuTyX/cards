// mysql.cxx
//
//  Copyright (c) 2016 - 2020 by NuTyX team (http://nutyx.org)
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
using namespace std;
namespace Sql
{
mysql::mysql(const char *configFileName)
{
	cards::Conf mysqlConfig(configFileName);
	m_socket=NULL;

	m_connection = mysql_init(NULL);
	mysql_options(m_connection, MYSQL_INIT_COMMAND, "SET NAMES utf8");
	mysql_real_connect(m_connection,
		mysqlConfig.hostname().c_str(),
		mysqlConfig.username().c_str(),
		mysqlConfig.password().c_str(),
		mysqlConfig.database().c_str(),
		m_port_no,
		m_socket,
		m_opt);
}
mysql::~mysql()
{
	if (m_result)
		mysql_free_result(m_result);
	if (m_connection)
		mysql_close(m_connection);
}
void mysql::lastPosts(const char *forum, int n)
{
	// retrieve the list of boards
	boardInfo_t info;
	if(mysql_query(m_connection,
		"select id_board, name, id_cat from smf_boards order by id_board"))
			std::cout << mysql_error(m_connection)
				<< std::endl;
	m_result= mysql_use_result(m_connection);
	board_t listOfBoards;

	while ((rows = mysql_fetch_row(m_result)) != NULL) {
		info.category = rows[2];
		info.name = rows[1];
		listOfBoards[ rows[0] ] = info;
	}

	// retrieve the list of categories
	if(mysql_query(m_connection,
	"select id_cat, name from smf_categories order by id_cat"))
		std::cerr << mysql_error(m_connection) << std::endl;
	m_result= mysql_use_result(m_connection);

	category_t listOfCategories;
	while ((rows = mysql_fetch_row(m_result)) != NULL) {
		listOfCategories[ rows[0] ] = rows[1];
	}

	// retrieve the list of members
	if (mysql_query(m_connection,
	"select id_member, member_name, real_name from smf_members order by id_member"))
		std::cerr << mysql_error(m_connection) << std::endl;
	m_result= mysql_use_result(m_connection);

	userInfo_t user;
	user_t listOfUsers;
	while (( rows = mysql_fetch_row(m_result)) != NULL) {
		user.member_name=rows[1];
		user.real_name=rows[2];
		listOfUsers[ rows[0] ] = user;
	}
	// retrieve the list of messages
	if(mysql_query(m_connection,
	"select id_topic, id_msg, id_member, poster_time, subject, icon, id_board from smf_messages order by id_msg"))
		std::cerr << mysql_error(m_connection) << std::endl;
	m_result= mysql_use_result(m_connection);

	std::vector<std::string> list;
	std::string sforum = forum;
	while ((rows = mysql_fetch_row(m_result)) != NULL) {
		std:string category = "<div style=\"text-transform: uppercase;\">";
		category += listOfCategories[ listOfBoards[rows[6]].category ] + ":</div>";
		std::string id_topic = rows[0];
		std::string id_msg = rows[1];
		std::string time = "<p class=\"updated\">";
		time +=getDateFromEpoch(strtoul(rows[3],NULL,0)) + " UTC</p>";
        std::string board = "<b>";
		board += listOfBoards[ rows[6] ].name;
		std::string author = "<i>";
		author += listOfUsers[ rows[2] ].real_name;
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
	int i = 1;
	while ( i < n + 1) {
		cout << list[list.size()-i] << endl;
		i++;
		if ( i != n + 1)
			cout << "<hr align=\"center\" style=\"width: 50%;\">" << endl;
	}
}
void mysql::lastPosts(const char *forum, const char *id_board, int n)
{
	std::string sboard = id_board;
	std::string query = "select poster_name, subject, body icon from smf_messages where id_board = " \
			+ sboard + " order by id_msg";
	if(mysql_query(m_connection, query.c_str()))
		std::cerr << mysql_error(m_connection) << std::endl;
	m_result= mysql_use_result(m_connection);
}
} /* namespace Sql */
// vim:set ts=2 :
