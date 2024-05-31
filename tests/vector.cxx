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
	Vector.push_back("how");
	Vector.push_back("are");
	Vector.push_back("you");
	Vector.push_back("?");
	printf("First Value: %s\n",Vector.value(0));
	printf("Last Value: %s\n",Vector.value(Vector.size()-1));
	printf("Vector[1]: %s\n",Vector[1]);

	char string[] = "? I'm ok, what about you ?";

	/*
	We need to delete the char * string in the destination vector cell
	before we store a new char * string, otherwise we have a nice memory leak.
	Perhaps it can be done in the operator function ?
	*/
	free(Vector[4]);
	Vector[4]=nullptr;
	Vector[4]=strdup(string);
	printf("Vector[4]: %s\n",Vector[4]);
	for (unsigned int i = 0; i < Vector.size(); ++i )
		printf("%s ",Vector[i]);
	printf("\n");


 cards::vector Vector2;
 printf("Size: %u\nCapacity: %u\n\n",Vector2.size(),Vector2.capacity());
 Vector2.reserve(1500);
 printf("Size: %u\nCapacity: %u\n\n",Vector2.size(),Vector2.capacity());
 Vector2.reserve(1000);
 printf("Size: %u\nCapacity: %u\n\n",Vector2.size(),Vector2.capacity());

}
