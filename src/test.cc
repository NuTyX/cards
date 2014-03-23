#include "archive_utils.h"
#include <set>
#include <iostream>

using namespace std;


int main (int argi, char * argv[])
{
	try {
		ArchiveUtils  * au  = new ArchiveUtils(argv[1]) ;
		string name = au->name() + " " + au->version() ;
		au->list();
		cout << endl << name << endl<<endl;
		au->printInfo();
		cout << endl;
		au->printMeta();
		cout << endl << "build On: " <<  au->builddate()<<endl;

	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
