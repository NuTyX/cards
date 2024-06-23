/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace cards {

class string {
    char *m_value;
    unsigned int m_capacity;
    unsigned int m_size;
    void init();

public:
    string();
    string(char *str);
    string(const char *str); 
    string(string & str);
    ~string();

    string& operator=(const char *str);
    char operator[](unsigned int i) const;
    char& operator[](unsigned int i);
    const unsigned int size();
    const unsigned length();
    const unsigned int capacity();
    const char *data();
    const char *c_str();
    void reserve(const unsigned int capacity);
    void shrink_to_fit(); /* TODO */
    string &assign(const char *str);
    string &assign(string &str);
    string &assign(const char *str,const unsigned int size);
};

}
