#include <cstring>
#include <string>
#include <iostream>

using namespace std;

char* optarg = nullptr;


/* getopt() parses argv similar to POSIX getopt()
	option delimiter: '-'
	optstr decribes all allowed options: 
		"x"  = simple option, no optional argument
		"x:" = extended option, with optional argument
	optional arguments are returned in global char* optarg 
	returns 0, if argv is not an allowed argument in optstr
	returns -1 after last argument has been parsed
*/
int getopt(int argc, char* argv[], char* optstr) {
	static int optind = 1;
	if (optind >= argc)
		return -1;
	char *optch, *colon;
	optarg = nullptr;
	if (*argv[optind] == '-') {
		++argv[optind];
		colon = optch = strchr(optstr, *argv[optind]);
		if (optch != nullptr) {
			++colon;
			++optind;
			if (*colon == ':') 
				optarg = argv[optind];
			return (int)*optch;
		}
	}
	else {
		++optind; }
	return 0;
}

int main(int argc, char* argv[]) {
	// create argc from argv
	char* argv_[] = { "progname", "-v", "videoinput", "-c", "-o", "filename", "d" };
	int argc_ = (sizeof(argv_)/sizeof(argv_[0]));

	// loop though argv
	char c;
	int i = 0;
	while ((c = getopt(argc_, argv_, "co:v:")) != -1) {
		++i;
		cout << endl << "pass: " << i << endl;

		switch (c) {
		case 'c':
			cout << "option 'c' " << endl;
			break;
		case 'o':
			cout << "option 'o' " << endl;
			cout << "optarg: " << optarg << endl;
			break;
		case 'v':
			cout << "option 'v' " << endl;
			break;
		default:
			cout << "no option" << endl;
			break;
		}
	}


	cout << endl << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);

	return 0;
}