/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include "string_utils.h"

namespace cards {
// cards::vector of char* (string of characters c style)
class vector {
public:
    using value_type = char*;
    using const_value_type = const char*;

    using pointer    = value_type*;
    using reference  = value_type&;

    class iterator {
    public:
        iterator(pointer ptr)
            :m_ptr(ptr) {}

        reference operator*() const {
            return *m_ptr;
        }
        // ++iterators
        iterator& operator++() {
            ++m_ptr;
             return *this;
        }
        // iterator++ yes it need's an int as argument, don't ask me why
        iterator operator++(int) {
            iterator Iterator = *this;
            ++(*this);
            return Iterator;
        }
        bool operator==(const iterator& i) {
            return m_ptr == i.m_ptr;
        }
        bool operator!=(const iterator& i) {
            return m_ptr != i.m_ptr;
        }
    private:
        pointer m_ptr;
    };

    vector();
    vector(const unsigned int capacity);
    vector(value_type element);
    ~vector();
    const_value_type operator[](const unsigned int i) const;
    reference operator[](unsigned int i);
    const_value_type value(const unsigned int index);
    void reserve(const unsigned int capacity);
    void push_back(const_value_type element);
    const unsigned int size();
    const unsigned int capacity();

    iterator begin();
    iterator end();


private:
    pointer      m_items;
    unsigned int m_size;
    unsigned int m_capacity;
};
} // end of cards namespace
