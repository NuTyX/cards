// mysql.cpp
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
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
//  USA.
//
#include "mysql.h"
using namespace std;
mysql::mysql(const char *configFileName)
{
	getConfig(configFileName,mysqlConfig);
	m_socket=NULL;

	m_connection = mysql_init(NULL);
	mysql_options(m_connection, MYSQL_INIT_COMMAND, "SET NAMES utf8");	
	mysql_real_connect(m_connection,
		mysqlConfig.hostname.c_str(),
		mysqlConfig.username.c_str(),
		mysqlConfig.password.c_str(),
		mysqlConfig.database.c_str(),
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
	if(mysql_query(m_connection,
	"select id_topic,id_msg, poster_name, poster_time, subject from smf_messages"))
		cerr << mysql_error(m_connection) << endl;
	m_result= mysql_use_result(m_connection);
	vector<string> list;
	string sforum = forum;
	while ((rows = mysql_fetch_row(m_result)) != NULL) {
		string id_topic = rows[0];
		string id_msg = rows[1];
		string time = "<p class=\"updated\">";
		time +=getDateFromEpoch(strtoul(rows[3],NULL,0)) + "</p>";
		string author = "<i>";
		author += rows[2];
		string subject = rows[4];
		string message = time; 
		message += author;
		message += "</i><br><a href=\"" + sforum;
		message += "/index.php?topic=" + id_topic;
		message += ".msg" + id_msg;
		message += "#msg" + id_msg;
		message += "\">" +subject;
		message += "</a><br><br>";
		list.push_back(message);
	}
	int i = 1;
	while ( i < n + 1) {
		cout << list[list.size()-i] << endl;
		i++;
	}
}
// vim:set ts=2 :
