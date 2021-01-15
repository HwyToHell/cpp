// project specific


// std
#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <memory>
#include <string>


// test memory management
int main_scoped_ptr(int argc, char *argv[])
{
    (void)argc; (void)argv;
    using namespace std;


    // allocate array on stack
    // test with std compiler flags: 10MB exeeds stack size
    const size_t stackElem = 5;
    std::array<int,stackElem> myArr{1, 2, 3};
    cout << "myArray on stack with " << myArr.size() << " elements" << endl;
    std::for_each(myArr.begin(), myArr.end(), [](int c){cout << c << endl;});

    // allocate large array dynamically in heap
    const size_t heapElem = 1000000000; // 1GB
    unique_ptr<array<int, heapElem>> pArray = make_unique<array<int, heapElem>>();
    pArray.get()->at(0) = 1;
    pArray->at(1) = 2;
    cout << "array in heap with " << pArray->size() << " elements" << endl;
    size_t elemToShow = 5;
    cout << "first " << elemToShow << endl;
    std::for_each(pArray->begin(), pArray->begin() + elemToShow,
                  [](int c){cout << c << endl;});

    // push array to deque
    std::deque<array<int, heapElem>> dqArray;
    dqArray.push_back(*pArray.get());
    cout << "accessing deque, first element of array" << endl;
    cout << dqArray.back().at(0) << endl;

    // push large arrays to deque until it throws bad_alloc
    // test results: iterates 14 times with 1GB array,
    // process was automatically killed afterwards, without throwing exception
    int cnt = 0;
    while (true) {
        try {
            dqArray.push_back(*pArray.get());
            cout << "pass: " << ++cnt << endl;
        } catch (const std::exception& e) {
            cout << "exception: " << e.what() << endl;
            break;
        }
    }

    return 0;
}
