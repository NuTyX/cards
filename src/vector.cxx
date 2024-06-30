/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "vector.h"

namespace cards {
// TODO findout why this is necessary
using value_type = char*;
using const_value_type = const char*;
using reference  = value_type&;

vector::vector() {
    m_capacity = 15;
    m_items = (pointer)malloc(sizeof m_items * m_capacity);
    m_size = 0;
}
vector::vector(const unsigned int capacity) {
    m_capacity = capacity;
    m_items = (pointer)malloc(sizeof m_items * m_capacity);
    m_size = 0;
}
vector::vector(value_type element) {
    m_capacity = 15;
    m_items = (pointer)malloc(sizeof m_items * m_capacity);
    m_size = 1;
    m_items[0] = strdup(element);
}
void vector::push_back(const_value_type element) {
	if ( m_capacity == m_size) {
		reserve(m_size * 2);
	}
	m_items[ m_size ] = strdup(element);
	++m_size;
}
void vector::reserve(const unsigned int capacity) {
    if (m_capacity >= capacity)
        return;
    m_capacity = capacity;
    m_items = (pointer)realloc( m_items, sizeof m_items * m_capacity);
    if ( m_items == nullptr ) {
        printf("Cannot reallocate m_items: %d", &m_items);
        return;
    }
}
const_value_type vector::value(const unsigned int index) {
    if (index > m_size - 1)
        return m_items[m_size - 1];
    return m_items[index];
}
const unsigned int vector::size() {
    return m_size;
}
const unsigned int vector::capacity() {
    return m_capacity;
}
const_value_type vector::operator[](const unsigned int i) const {
	return m_items[i];
}
reference vector::operator[](unsigned int i) {
	return m_items[i];
}
vector::~vector() {
    for (unsigned int i=0; i < m_size;i++) {
        if (m_items[i] != nullptr) {
            free(m_items[i]);
            m_items[i] = nullptr;
        }
    }
    if (m_items != nullptr) {
        free(m_items);
        m_items = nullptr;
    }
}
vector::iterator vector::begin() {
    return vector::iterator(&m_items[0]);
}
vector::iterator vector::end() {
    return vector::iterator(&m_items[m_size]);
}
} // end of cards namespace
