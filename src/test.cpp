#include "pkgrepo.h"
#include "archive_utils.h"
#include <pwd.h>
#include <set>
#include <iostream>
#include <string>


using namespace std;


int main (int argi, char * argv[])
{
	try {
		ArchiveUtils  * au  = new ArchiveUtils(argv[1]) ;
		cout << "Name: " << au->name() << endl;
		set< pair<string,time_t> > depList = au->listofDependenciesBuildDate();
		for (set< pair<string,time_t> >::const_iterator i = depList.begin();i != depList.end();i++) {
			cout << i->first << " " << i->second <<  endl;
		}
	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
// vim:set ts=2 :
