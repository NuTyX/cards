/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "string.h"

namespace cards {
string::string() {
    init();
}
string::string(string &str) {
    init();
    assign(str);
}
string::string(const char *str) {
    init();
    assign(str);
}
string::~string() {
    if (m_value)
        ::free(m_value);
    init();
}
const unsigned int string::size() {
    return m_size;
}
const unsigned int string::length() {
    return m_size;
}
const unsigned int string::capacity() {
    return m_capacity;
}
const char *string::data() {
    if (!m_value)
        reserve(1);
    return m_value;
}
const char *string::c_str() {
    if (!m_value)
        reserve(1);
    return m_value;
}
string &string::assign(const char *str){
    if (str && *str) {
        unsigned int len = strlen(str);
        return assign(str,len);
    } else {
        reserve(0);
        return *this;
    }
}
string &string::assign(string &str){
    if (&str == this)
        return *this;
    return assign(str.data(),str.size());
}
string &string::assign(const char *str,const unsigned int size){
    if (size > 0) {
        reserve(size);
        memcpy(m_value,str,size);
        m_value[size] = 0;
        m_size = size;
    } else {
        reserve(0);
    }
    return *this;
}
void string::init(){
    m_value = nullptr;
    m_size = 0;
    m_capacity = 0;
}
void string::reserve(const unsigned int capacity) {
    if (m_capacity = 0 ) {
        m_capacity = capacity;
        m_value = (char*)malloc (sizeof (char) * m_capacity);
        return;
    }
    if (m_capacity >= capacity)
        return;

    m_capacity = capacity;
    m_value = (char*)realloc( m_value, sizeof m_value * m_capacity);
    if ( m_value == nullptr ) {
        printf("Cannot reallocate m_items: %d", &m_value);
        return;
    }

}
string &string::operator=(const char *str) {
    return assign(str);
}
char string::operator[](unsigned int i) const {
    if (m_value)
        return m_value[i];
    else
        return 0;
}
char &string::operator[](unsigned int i) {
    if (!m_value)
        reserve(1);
    return m_value[i];
}
void string::shrink_to_fit() {
    /*TODO*/
}
}
