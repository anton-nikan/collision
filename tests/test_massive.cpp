// test_init.cpp

#include <vector>
#include <iostream>
#include <random>
#include <chrono>
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

const int num_samples = 10000;

int main()
{
    std::vector<my_object> objects; objects.reserve(num_samples);
    std::default_random_engine eng(12345);
    std::uniform_int_distribution<> rnd(-500, 500);
    while (objects.size() < num_samples)
        objects.emplace_back(rnd(eng), rnd(eng));

    collision_group group;
    for (auto& obj: objects)
        group.add(obj.get_cobject());

    int num = 0;
    group.on_collide([&](const collision_object& obj1, const collision_object& obj2) {
        ++num;
    });

    auto start = std::chrono::high_resolution_clock::now();
    collision::check_collision(group);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << num_samples << " objects tested, "
            << num << " collisions found, "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl;
}