/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "file_utils.h"
#include <mysql/mysql.h>

namespace Sql {

struct boardInfo_t {
    std::string category;
    std::string name;
};
struct memberInfo_t {
    std::string member_name;
    std::string real_name;
};
struct messageInfo_t {
    std::string id_msg;
    std::string id_board;
    std::string id_topic;
    std::string id_member;
    std::string poster_time;
    std::string poster_name;
    std::string subject;
    std::string icon;
    std::string body;
};
typedef std::map<std::string, boardInfo_t> board_t;
typedef std::map<std::string, std::string> category_t;
typedef std::map<std::string, memberInfo_t> member_t;
typedef std::vector<messageInfo_t> message_t;

class mysql {
public:
    mysql(const char* configFileName);
    virtual ~mysql();

    void lastPosts(const char* forum);
    void lastPosts(const char* forum, const char* id_board, int n);

private:
    void listOfBoards();
    void listOfCategories();
    void listOfMembers();
    void listOfMessages();

    const char* m_hostname;
    const char* m_database;
    const char* m_username;
    const char* m_password;
    const char* m_socket;

    std::vector<std::string> m_list;

    enum {
        m_port_no = 3306,
        m_opt = 0
    };
    MYSQL* m_connection;
    cards::conf m_sqlConfig;

    board_t m_listOfBoards;
    category_t m_listOfCategories;
    member_t m_listOfMembers;
    message_t m_listOfMessages;
};
} /* namespace Sql */

