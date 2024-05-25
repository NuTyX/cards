#include "../src/libcards.h"

int main() {
    std::string string1;
    printf("Capacity of string1: %u\n",string1.capacity());
    string1="123456789012345678";
    printf("Capacity of string1: %u\n",string1.capacity());
    string1.shrink_to_fit();
    printf("Size of string1: %u\n",string1.size());
    printf("Lenght of string1: %u\n",string1.length());
    printf("Capacity of string1: %u\n",string1.capacity());
    printf("Size of string1: %u\n",sizeof string1 );
    printf("Size of std::string: %u\n",sizeof (std::string) );
    printf("Size of std::string::size: %u\n",sizeof (string1.size()) );
    printf("Size of std::string::capacity: %u\n",sizeof (string1.capacity()) );
    printf("Size of *char: %u\n",sizeof (char*) );

    const char *ConstString = "This is a constant string in c++";
    const char& ConstRef = *ConstString;

    char NonConstString[] = "This is a non constant string in C++";
    char& Ref = *NonConstString;
    printf("%s\n%s\n%s\n%s\n",ConstString,NonConstString,&Ref,&ConstRef);

    char* my_string_pointer = new char[10]; // points to a char array
    my_string_pointer[0] = 'a';
    my_string_pointer[1] = 'b';
    my_string_pointer[2] = 'c';
    char* my_char_pointer = my_string_pointer + 1; // points to 'b'
    const char& my_char_reference = *my_char_pointer; // reference to 'b'
/* This is not possible anymore
   A reference is READONLY and cannot be modify in such a way
char my_char = *my_char_pointer; // only a copy of 'b'
*/
}
