#include "archive_utils.h"
#include <set>
#include <iostream>

using namespace std;


int main (int argi, char * argv[])
{
	try {
		ArchiveUtils au(argv[1]);
/*		au.printMetaInfo();
		set<string> list = au.getFilesList();
		for (set<string>::iterator i = list.begin();i != list.end();++i) {
			cout << *i << endl;
		}*/
		au.printMetaInfo();
	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
