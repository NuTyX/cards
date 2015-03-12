#include "config_parser.h"
#include "archive_utils.h"
#include <set>
#include <iostream>
#include <string>
using namespace std;


int main (int argi, char * argv[])
{
	try {
/*		ConfigParser  * cp = new ConfigParser("/etc/cards.conf");
		cp->parseMD5sumCategoryDirectory();
		cp->parseBasePackageList();
		cp->parsePackageInfoList();
		unsigned i = cp->getPortsList();
		if (i > 0 ) cout << endl << endl << "Number of Base Binaries: " << i << endl;

//		ArchiveUtils  * au  = new ArchiveUtils(argv[1]) ;
//		cout << "Number of files: " << au->size() << endl;
//		cout << au->name() << " " << au->version() << endl;
//		cout << au->description() << endl;
//		au->printInfo();
//		string packageDate = au->namebuildn();
//		set<string> depList = au->listofDependencies();
//		for (set<string>::const_iterator i = depList.begin();i != depList.end();i++) {
//			cout << argv[1] << ": " << *i << endl;
//		}
//		au->printMeta();
//		cout << packageDate << endl;
*/
	cout << RED << "Red" << NORMAL << endl;
	cout << "Normal" << endl;
	cout << YELLOW << "Yellow" << NORMAL << endl;

	}  catch (runtime_error& e) {
			cerr << e.what() << endl;
			return -1;
	}
	return 0;
}
