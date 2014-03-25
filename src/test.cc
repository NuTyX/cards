#include "archive_utils.h"
#include <set>
#include <iostream>

using namespace std;


int main (int argi, char * argv[])
{
	try {
		ArchiveUtils  * au  = new ArchiveUtils(argv[1]) ;
		string name = au->name() + " " + au->version() ;
//		au->list();
//		cout << "Name: " <<  name << endl<<endl;
//		au->printInfo();
		cout << endl;
		cout << "Description: "<< au->description()<<endl;
//		au->printMeta();
//		cout << "build On: " <<  au->builddate()<<endl;
//		cout << "build Number: " <<  au->buildn()<<endl;
		cout << "Number of files: " << au->size()<<endl;

	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
