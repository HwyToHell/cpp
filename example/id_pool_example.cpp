#include "../inc/id_pool.h"

#include <iostream>
#include <string>


/// pause console until <enter> has been pressed
bool waitForEnter();



class DataWithID : public IdPool {
public:
	DataWithID(int data) : m_data(data) {}
	int data() { return m_data; }
private:
	int m_data;
};

void pushDataToArray(std::vector<DataWithID>& dataArray) {
	DataWithID data(555);
	dataArray.push_back(data);
	return;
}


int main(int argc, char* argv[]) {
	using namespace std;
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

	waitForEnter();
	return 0;
}

	
bool waitForEnter() {
	using namespace std;
	cout << endl << "Press <enter> to exit" << endl;
	string str;
	getline(cin, str);
	return true;
}





