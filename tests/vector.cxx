#include "../src/libcards.h"

int main() {
    FILE* fp = fopen("filesList", "r");
	if (!fp) {
		printf("Cannot open file filesList\n");
		return -1;
	}

	char line[BUFSIZ];
	cards::vector myVector(1);

	while (fgets(line, BUFSIZ, fp)) {
        if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';
		myVector.push_back(line);
    }
    fclose(fp);
    printf("Capacity of myVector: %u\n",myVector.capacity());
    printf("Size of myVector: %u\n",myVector.size());

/*	for (unsigned int i = 0; i < myVector.size(); ++i )
		printf("%s\n",myVector.value(i));
*/
	printf("First Value: %s\n",myVector.value(0));
	printf("Last Value: %s\n",myVector.value(myVector.size()));

	cards::vector Vector;
	Vector.push_back("hello");
	Vector.push_back("how ");
	Vector.push_back("are");
	Vector.push_back("you");
	Vector.push_back("?");
	printf("First Value: %s\n",Vector.value(0));
	printf("Last Value: %s\n",Vector.value(Vector.size()-1));
	printf("First value: %s\n",Vector[3]);
/* TODO
   Find out what's wrong here
	Vector[3]="Hello";
*/
	printf("Value 4: %s\n",Vector[3]);

 cards::vector Vector2;
 printf("Size: %u\nCapacity: %u\n\n",Vector2.size(),Vector2.capacity());
 Vector2.reserve(1500);
 printf("Size: %u\nCapacity: %u\n\n",Vector2.size(),Vector2.capacity());
 Vector2.reserve(1000);
 printf("Size: %u\nCapacity: %u\n\n",Vector2.size(),Vector2.capacity());

}
