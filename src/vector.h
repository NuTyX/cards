#pragma once
#include "string_utils.h"

namespace cards {

class vector
{
public:
    vector();
    vector(const unsigned int capacity);
    vector(char * element);
    ~vector();
    char* operator[](unsigned int i) const;
    char*& operator[](unsigned int i);
    const char* value(const unsigned int index);
    void reserve(const unsigned int capacity);
    void push_back(const char* element);
    const unsigned int size();
    const unsigned int capacity();


private:
    char** m_items;
    unsigned int m_size;
    unsigned int m_capacity;
};
} // end of cards namespace

// vim:set ts=2 :

