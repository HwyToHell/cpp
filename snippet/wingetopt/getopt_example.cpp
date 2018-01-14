#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include "wingetopt.h"

using namespace std;

char* optarg = nullptr;



int main(int argc, char* argv[]) {
	int argc_ = 5;
	char* argv_[] = { "progname", "-v", "-c", "-o", "filename", "d" };
	argc_ = (sizeof(argv_)/sizeof(argv_[0]));
	for (int n = 0; n<argc_; ++n) {
		cout << "pass: " << n << " size: " << sizeof(argv_[n]) << endl;
	}
	cout << endl;
	int c;
	int i = 0;

	while ((c = wingetopt(argc_, argv_, "co:v")) != -1) {
		++i;
		cout << "pass: " << i << " opt: " << (char)c << endl;
		switch (c) {
		case 'c':
			cout << "option 'c' " << endl;
			break;
		case 'o':
			cout << "option 'o' " << endl;
			cout << "argument: " << optarg << endl;
			break;
		case 'v':
			cout << "option 'v' " << endl;
			break;
		default:
			cout << "unknown option" << endl;
			break;
		}
	}


	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}