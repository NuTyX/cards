#include "vector.h"
namespace cards {
vector::vector() {
    m_capacity = 15;
    m_size = 1;

}
vector::vector(const unsigned int capacity){
    m_capacity = capacity;
    m_size = 1;
}
const unsigned int vector::size(){
    return m_size;
}
const unsigned int vector::capacity(){
    return m_capacity;
}

} // end of cards namespace

// vim:set ts=2 :

