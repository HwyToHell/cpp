#include "stdafx.h"
#include <iostream>
#include <string>
#include <utility> 


/// pause console until <enter> has been pressed
bool waitForEnter();

class A {
private:
    int a;
public:
	A(int a_) : a(a_) {}
	A(A&& other) { 
		a = other.a;
		std::cout << "move c'tor" << std::endl;
	}
    A& operator=(A&& other) { 
        a = other.a;
        return *this;
		std::cout << "move assign" << std::endl;
    }

    void showA() { std::cout << a << std::endl; }
};




int main(int argc, char* argv[]) {
    A obj1(10);
    A obj2(20);
	obj1.showA();
    obj2.showA();


	std::swap(obj1, obj2);
	std::cout << "swapped:" << std::endl;
	obj1.showA();
    obj2.showA();



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





