#include <cstring>
#include "../inc/program_options.h"

char ProgramOptions::getOpt(int argc, char* argv[], char* optstr) {
	//int ret = 0;
	if (mOptInd >= argc)
		return mEof;
	char *optch, *colon;
	mOptArg = nullptr;
	if (*argv[mOptInd] == '-') {
		++argv[mOptInd];
		colon = optch = strchr(optstr, *argv[mOptInd]);
		++mOptInd;
		if (optch != nullptr) {
			++colon;
			if (*colon == ':') {
				if (mOptInd < argc) {// else: mOptArg = nullptr = no opt arg
					// check for leading '-'
					if (*argv[mOptInd] != '-') // opt arg starts with '-' = no opt arg
						mOptArg = argv[mOptInd];
				}
			}
			return *optch;
		}
	}
	else {
		++mOptInd; }
	return '\0';
}


bool ProgramOptions::parseCmdLine(int ac, char* av[], char* optDesc) {
	mOptInd = 1;
	char c;
	while ((c = getOpt(ac, av, optDesc)) != mEof) {
		if (c) {
			if (mOptArg)
				mOptMap.insert(TOpt(c, mOptArg));
			else
				mOptMap.insert(TOpt(c, ""));
		}
	}
	return true;
}
	
TOptMap& ProgramOptions::getOptMap()  { return mOptMap; }