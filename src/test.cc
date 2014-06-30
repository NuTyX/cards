#include "archive_utils.h"
#include <set>
#include <iostream>
#include <string>
using namespace std;


int main (int argi, char * argv[])
{
	try {
		ArchiveUtils  * au  = new ArchiveUtils(argv[1]) ;
//		cout << "Number of files: " << au->size() << endl;
//		cout << au->name() << " " << au->version() << endl;
//		cout << au->description() << endl;
//		au->printInfo();
//		string packageDate = au->namebuildn();
		vector<string> depList = au->listofDependencies();
		for (vector<string>::const_iterator i = depList.begin();i != depList.end();i++) {
			cout << *i << endl;
		}
//		au->printMeta();
//		cout << packageDate << endl;
	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
