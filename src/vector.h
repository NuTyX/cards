#pragma once
namespace cards {

class vector
{
public:
    vector();
    vector(const unsigned int capacity);
    ~vector(){};

    void push_back(const void * element);
    const unsigned int size();
    const unsigned int capacity();


private:
    unsigned int m_capacity;
    unsigned int m_size;

};
} // end of cards namespace

// vim:set ts=2 :

