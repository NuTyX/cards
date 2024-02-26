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
void mysql::lastPosts(const char *forum)
{
	listOfBoards();
	listOfCategories();
	listOfMembers();
	listOfMessages();

	int i = 0;

	while ( i < 10) {

		std::cout			<< "   <div class=\"post-time\">"
			<< getDateFromEpoch(strtoul(m_listOfMessages[i].poster_time.c_str(),NULL,0))
			<< " UTC</div>\n   <div class=\"post-category\">"
			<< m_listOfCategories[m_listOfBoards[m_listOfMessages[i].id_board].category]
			<< ":</div>\n   <div class=\"post-subject\">"
			<< m_listOfBoards[ m_listOfMessages[i].id_board ].name
			<< "</div>\n   <div class=\"post-author\">"
			<< m_listOfMembers[ m_listOfMessages[i].id_member].real_name
			<< "</div>\n   <img class=\"post-icon\" src=\"../../graphics/"
			<< m_listOfMessages[i].icon
			<< ".gif\" alt=\"\" />\n   <a class=\"post-link\" href=\""
			<< forum
			<< "/index.php?topic="
			<< m_listOfMessages[i].id_topic
			<< ".msg"
			<< m_listOfMessages[i].id_msg
			<< "#msg"
			<< m_listOfMessages[i].id_msg
			<< "\">\n    "
			<< m_listOfMessages[i].subject
			<< "\n   </a>\n\n   ";

		i++;

		if ( i < 10)
			std::cout << "<hr align=\"center\" style=\"width: 50%;\">"
				<< std::endl;

	}
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

	if (res = mysql_use_result(m_connection))
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

	if(res = mysql_use_result(m_connection))
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
		from smf_messages order by id_msg \
		DESC LIMIT 10"))
			std::cout << mysql_error(m_connection)
				<< std::endl;

	if(res = mysql_use_result(m_connection))
		std::cout << mysql_error(m_connection)
			<< std::endl;

	while (( rows = mysql_fetch_row(res)) !=NULL) {
		message.id_msg=rows[0];
		message.id_board=rows[1];
		message.id_topic=rows[2];
		message.id_member=rows[3];
		message.poster_time=rows[4];
		message.subject=rows[5];
		message.icon=rows[6];

		m_listOfMessages.push_back(message);

	}

	if(res)
		mysql_free_result(res);
}
} /* namespace Sql */
// vim:set ts=2 :
