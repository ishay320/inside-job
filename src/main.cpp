#include <iostream>
#include <string>

#include "hashmap_tree.h"

int main(int argc, char const *argv[])
{
    HashmapTree<std::string, std::string> tree;
    std::string tee[] = {"hello", "world"};
    tree.insert(tee, 2, "test");
    std::string goo[] = {"hello", "world2"};
    tree.insert(goo, 2, "test2");
    tree.insert(goo, 2, "test3");

    auto foo = tree.get(tee, 1);
    for (auto &&i : foo)
    {
        std::cout << i << " ";
    }
    std::cout << '\n';
    return 0;
}
