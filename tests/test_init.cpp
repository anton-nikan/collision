// test_init.cpp

#include <cassert>
#include <vector>
#include <iostream>
#include <collision/collision.hpp>

using namespace collision;

class my_object
{
public:
    int x, y, w, h;
    my_object(int x, int y, int sz) : x{x}, y{y}, w{sz}, h{sz} { }
};

int main()
{
    std::vector<my_object> objects;
    objects.emplace_back(10, 10, 10);
    objects.emplace_back(19, 19, 10);
    objects.emplace_back(28, 28, 10);
    objects.emplace_back(37, 37, 10);

    int num = 0;
    collision::check_collision(std::begin(objects), std::end(objects), [&](const my_object& obj1, const my_object& obj2) {
        std::cout << "collision!" << std::endl;
        ++num;
    });
    std::cout << num << " collisions found." << std::endl;

    assert(num == 3);
}