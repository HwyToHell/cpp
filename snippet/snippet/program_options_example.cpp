#include <iostream>
#include <algorithm>
#include "../../inc/program_options.h"

using namespace std;


void prnOpts(TOpt opt) {
	cout << opt.first << ": " << opt.second << endl;
}



int main(int argc, char* argv[]) {
	// create argc from argv
	char* argv_[] = { "progname", "-o", "-c", "-v", "file" };
	int argc_ = (sizeof(argv_)/sizeof(argv_[0]));

	ProgramOptions cmdlnOpts;
	cmdlnOpts.parseCmdLine(argc_, argv_, "co:v:");

	for_each(cmdlnOpts.getOptMap().begin(), cmdlnOpts.getOptMap().end(), prnOpts);
	

	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}