// test_init.cpp

#include <cassert>
#include <vector>
#include <iostream>
#include <collision/collision.hpp>

using namespace collision;

class my_object
{
    float x, y;
    collision_object cobj;
public:
    my_object(float x, float y) : x{x}, y{y}, cobj{x, y, 10, 10} { }
    collision_object& get_cobject() { return cobj; }
};

int main()
{
    std::vector<my_object> objects;
    objects.emplace_back(10, 10);
    objects.emplace_back(19, 19);
    objects.emplace_back(28, 28);
    objects.emplace_back(37, 37);

    collision_group group;
    for (auto& obj: objects)
        group.add(obj.get_cobject());

    int num = 0;
    group.on_collide([&](const collision_object& obj1, const collision_object& obj2) {
        std::cout << "collision!" << std::endl;
        ++num;
    });

    collision::check_collision(group);
    std::cout << num << " collisions found." << std::endl;

    assert(num == 3);
}