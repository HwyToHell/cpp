#include <iostream>
#include "../../inc/id_pool.h"

int main(int argc, const char* argv[]) {
    (void)argc; (void)argv;

    IdGen id(3);
    std::cout << "first id: " << id.allocID() << std::endl;
    std::cout << "second id: " << id.allocID() << std::endl;
    std::cout << "third id: " << id.allocID() << std::endl;
    std::cout << "fourth id: " << id.allocID() << std::endl << std::endl;

    IdUnique uid;
    IdUnique uid2;
    std::cout << "first unique id: " << uid.allocID() << std::endl;
    std::cout << "second unique id: " << uid.allocID() << std::endl;
    std::cout << "third unique two id: " << uid2.allocID() << std::endl;
    std::cout << "fourth unique two id: " << uid2.allocID() << std::endl;

    return 0;
}
