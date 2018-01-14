#include <iostream>
#include <iomanip>
#include <string>
#include "wingetopt.h"

using namespace std;

int main(int argc, char* argv[]) {

	// check enum
	enum direction { left=0, right};
	char* dir[] = { "left", "right"};
	cout << dir[left];

	// check setw
	double d = 1234.567890;
	cout << endl;
	cout.precision(3); // double with 3 digits after .
	cout << setw(10) << 333 << setw (10) << fixed << d << endl;
	cout.precision(1); // double with 1 digit after .
	cout << setw(10) << 55555 << setw (10) << fixed << d << endl;
	cout << setw(10) << 555.55 << endl;

	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}