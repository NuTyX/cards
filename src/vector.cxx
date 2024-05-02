#include "vector.h"
namespace cards {
vector::vector() {
    m_capacity = 15;
    m_items = (char**)malloc(sizeof m_items * m_capacity);
    m_size = 0;
}
vector::vector(const unsigned int capacity){
    m_capacity = capacity;
    m_items = (char**)malloc(sizeof m_items * m_capacity);
    m_size = 0;
}
vector::vector(char * element) {
    m_capacity = 15;
    m_items = (char**)malloc(sizeof m_items * m_capacity);
    m_size = 1;
    m_items[0] = strdup(element);
}
void vector::push_back(const char * element) {
	if ( m_capacity == m_size) {
		m_capacity = m_size * 2;
		m_items = (char **)realloc( m_items, sizeof m_items * m_capacity );
		if ( m_items == NULL ) {
			printf("Cannot reallocate m_items: %d", &m_items);
			return;
		}
	}
	m_items[ m_size ] = strdup(element);
	++m_size;
}
const char* vector::value(const unsigned int index) {
    if (index > m_size - 1)
        return m_items[m_size - 1];
    return m_items[index];
}
const unsigned int vector::size(){
    return m_size;
}
const unsigned int vector::capacity(){
    return m_capacity;
}
vector::~vector(){
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
} // end of cards namespace

// vim:set ts=2 :

