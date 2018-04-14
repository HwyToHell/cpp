/* for_each with function objects
 ******************************************************************************
 * function object is passed by value, not by reference --> for_each works with a local copy
 * the return value of operator() is ignored --> according to documentation:
 * "signature should be void fun(const Type &a)"
 * for_each returns the function object after it has been applied to each element

 * see also:
 * http://xenon.arcticus.com/c-morsels-std-for-each-functors-member-variables
 */



#include <algorithm>
#include <iostream>
#include <list>
#include <string>


using namespace std;

// function object to sum over for_each elements
class Functor {
	int mSum;
	int mPassCnt;
public:
	Functor(int sum=0) : mSum(sum), mPassCnt(0) {}
	void operator() (int track) {
		mSum += track;
		++mPassCnt;
		cout << "pass: " << mPassCnt << "  sum: " << mSum << endl;
	}
	int getSum() {
		return mSum;
	}
};

// function object that's initialized by pointing to existing variable
class Functor_Init_External {
	int* mPSum;
	int mPassCnt;
public:
	Functor_Init_External(int* pSum) : mPSum(pSum), mPassCnt(0) {}
	void operator() (int track) {
		*mPSum += track;
		++mPassCnt;
		cout << "pass: " << mPassCnt << "  sum: " << *mPSum << endl;
	}
};

int main(int argc, char* argv[]) {
	
	list<int> trackList;

	for(int i=0; i<5; ++i) {
		trackList.push_back(i);
	}

	// use returned functor
	cout << "summation in returned functor:" << endl;
	Functor sum = for_each(trackList.begin(), trackList.end(), Functor(10));
	cout << "----------------------------" << endl;
	cout << "total sum: " << sum.getSum() << endl << endl << endl;

	// use function intialized with pointer to existing variable
	int s = 10;
	Functor_Init_External sum_ext(&s);
	cout << "summation in existing variable, passed by pointer to instantiated functor:" << endl;
	for_each(trackList.begin(), trackList.end(), sum_ext);
	cout << "----------------------------" << endl;
	cout << "total sum: " << s << endl << endl << endl;


	cout << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);
	return 0;
}

