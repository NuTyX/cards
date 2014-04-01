#include "archive_utils.h"
#include <set>
#include <iostream>

using namespace std;


int main (int argi, char * argv[])
{
	try {
		ArchiveUtils  * au  = new ArchiveUtils(argv[1]) ;
		cout << "Number of files: " << au->size() << endl;
		cout << au->name() << " " << au->version() << endl;
		cout << au->description() << endl;
		au->printInfo();
	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
