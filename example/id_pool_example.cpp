#include "stdafx.h"

#include "../inc/id_pool.h"
//#include "../../cpp/inc/id_pool.h"
#include "../inc/pick_list.h"


/// pause console until <enter> has been pressed
bool waitForEnter();

/// derived from IdPool for instance counting
class DataWithID : public IdPool {
public:
	DataWithID(int data) : m_data(data) {}
	int data() { return m_data; }
private:
	int m_data;
};

/// pushes additional data to instantiated objects
void pushDataToArray(std::vector<DataWithID>& dataArray) {
	DataWithID data(555);
	dataArray.push_back(data);
	return;
}


int main(int argc, char* argv[]) {
	using namespace std;
	/// ID Gen

	// ID Gen instance with 9 uint
	IdGen occIds(9);
	cout << "pool size: " << occIds.poolSize() << endl;
	list<size_t> lst;
	for (size_t n = 1; n <= 4; ++n)
		lst.push_back(occIds.allocID());
	
	// put ID #1 back to pool
	occIds.freeID(lst.front());
	lst.remove(lst.front());

	// put ID #4 back to pool
	occIds.freeID(lst.back());
	lst.remove(lst.back());
	
	// put ID #3 back to pool
	occIds.freeID(lst.back());
	lst.remove(lst.back());

	// next ID will be #3
	size_t id3 = occIds.allocID();
	cout << "next ID #3: " << id3 << endl;

	// after sorting next ID will be #1 (#4 without sort)
	occIds.sortGreater();
	size_t id1 = occIds.allocID();
	cout << "next ID #1: " << id1 << endl;

	/*
	/// ID Pool
	// construction
	IdPool a1;
	cout << "1st instance id: " << a1.id() << endl;
	cout << endl;

	// copy construction
	IdPool a2(a1);
	cout << "2nd copied instance id: " << a2.id() << endl;
	cout << endl;

	// assignment construction
	IdPool a3;
	a3 = a1;
	cout << "3rd assignment instance id: " << a3.id() << endl;
	cout << endl;

	// move construction
	IdPool a4(std::move(a1));
	cout << "4th moved instance id: " << a4.id() << endl;
	cout << endl;

	// move assignment
	IdPool a5;
	a5 = std::move(a1);
	cout << "5th move assignment instance id: " << a5.id() << endl;
	cout << endl;

	// derived class
	std::vector<DataWithID> vec;
	pushDataToArray(vec);
	cout << "6th instance pushed to vector id: " << vec.back().id() << endl;
	cout << endl;

	// derived class uses default assignment operator
	DataWithID data(1);
	data = vec.front();
	cout << "data id: " << data.id() << " data: " << data.data() << endl;
	cout << " vec id: " << vec.front().id() << "  data: " << vec.front().data() << endl;
	cout << endl;

	// destruction
	vec.clear();
	cout << "list cleared" << endl << endl;

	// construction
	IdPool a7;
	cout << "7th instance id: " << a7.id() << endl;
	cout << endl;

	// construction
	IdPool a8;
	cout << "8th instance id: " << a8.id() << endl;
	cout << endl;
	*/

	waitForEnter();
	return 0;
}