// mysql.h
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
#ifndef MYSQL_H
#define MYSQL_H
#include <mysql/mysql.h>

#include "file_utils.h"
namespace Sql
{
struct boardInfo_t {
	std::string category;
	std::string name;
};
typedef std::map< std::string, boardInfo_t > board_t;
typedef std::map< std::string, std::string > category_t;

class mysql
{
public:
	virtual ~mysql();
	mysql(const char *configFileName);
	void lastPosts(const char *forum,int n);

private:
	const char *m_hostname;
	const char *m_database;
	const char *m_username;
	const char *m_password;
	const char *m_socket;
	enum {
		m_port_no = 3306,
		m_opt = 0
	};
	MYSQL *m_connection;
	MYSQL_RES *m_result;
	MYSQL_ROW rows;
	Config mysqlConfig;	
};
} /* namespace Sql */
#endif /* MYSQL_H */
// vim:set ts=2 : 
