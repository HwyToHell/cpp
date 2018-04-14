#include <iostream>
#include <algorithm>
#include "../../inc/program_options.h"

using namespace std;

void prnOpts(TOpt opt) {
		cout << opt.first << ": " << opt.second << endl;
}

int mapSize_test(ProgramOptions& opts) {
	for_each(opts.mOptMap.begin(), opts.mOptMap.end(), prnOpts);
	return opts.mOptMap.size(); 
}



int main(int argc, char* argv[]) {
	// create argc from argv
	char* argv_[] = { "progname", "-o", "-c", "-v", "file" };
	int argc_ = (sizeof(argv_)/sizeof(argv_[0]));

	ProgramOptions cmdlnOpts(argc_, argv_, "co:v:");

	int s = mapSize_test(cmdlnOpts);

	char chTest[] = "cov"; 
	char* ch = "cov";
	cout << endl;
	while (*ch) {
		if (cmdlnOpts.exists(*ch))
			cout << "opt arg " << *ch << ": " << cmdlnOpts.getOptArg(*ch) << endl;
		++ch;
	}
	

	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}