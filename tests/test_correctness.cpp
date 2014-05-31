// test_init.cpp

#include <vector>
#include <iostream>
#include <random>
#include <collision/collision.hpp>

using namespace collision;

class my_object
{
public:
    int x, y, w, h;
    my_object(int x, int y, int sz) : x{x}, y{y}, w{sz}, h{sz} { }
};

template <typename T>
bool check_collide(const T& obj1, const T& obj2)
{
    if (obj1.x+obj1.w < obj2.x || obj2.x+obj2.w < obj1.x) return false;
    if (obj1.y+obj1.h < obj2.y || obj2.y+obj2.h < obj1.y) return false;

    return true;
}

const int num_samples = 10000;

int main()
{
    std::vector<my_object> objects; objects.reserve(num_samples);
    std::default_random_engine eng(12345);
    std::uniform_int_distribution<int> rnd_pos(-500, 500);
    //std::uniform_int_distribution<int> rnd_size(10, 50);
    while (objects.size() < num_samples)
        objects.emplace_back(rnd_pos(eng), rnd_pos(eng), 10);

    int correct_num = 0;
    int false_num = 0;
    collision::check_collision(std::begin(objects), std::end(objects), [&](const my_object& obj1, const my_object& obj2) {
        if (!check_collide(obj1, obj2)) {
            std::cerr << "False collision: ["
                << obj1.x << ", " << obj1.y << "; " << obj1.w << ", " << obj1.h
                << "] x ["
                << obj2.x << ", " << obj2.y << "; " << obj2.w << ", " << obj2.h
                << "]" << std::endl;
            ++false_num;
        }
        else
            ++correct_num;
    });

    int real_num = 0;
    for (auto a = std::begin(objects); a != std::end(objects); ++a) {
        for (auto b = a + 1; b != std::end(objects); ++b) {
            if (check_collide(*a, *b))
                ++real_num;
        }
    }

    std::cout << num_samples << " objects tested, "
            << real_num << " real collisions, "
            << correct_num << " correct collisions found, "
            << false_num << " false collisions found."
            << std::endl;
}