// test_massive.cpp

#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <collision/collision.hpp>

using namespace collision;

class my_object
{
public:
    int x, y, w, h;
    my_object(int x, int y, int sz) : x{x}, y{y}, w{sz}, h{sz} { }
};

const int num_samples = 10000;

int main()
{
    std::vector<my_object> objects; objects.reserve(num_samples);
    std::default_random_engine eng(12345);
    std::uniform_int_distribution<int> rnd_pos(-500, 500);
    std::uniform_int_distribution<int> rnd_size(10, 50);
    while (objects.size() < num_samples)
        objects.emplace_back(rnd_pos(eng), rnd_pos(eng), rnd_size(eng));

    int num = 0;
    auto start = std::chrono::high_resolution_clock::now();
    collision::check_collision(std::begin(objects), std::end(objects), [&](const my_object& obj1, const my_object& obj2) {
        ++num;
    });
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << num_samples << " objects tested, "
            << num << " collisions found, "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl;
}