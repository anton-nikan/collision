// test_correctness.cpp

#include <vector>
#include <iostream>
#include <random>
#include <cassert>
#include <collision/collision.hpp>

using namespace collision;

class my_object
{
public:
    int x, y, w, h;
    my_object(int x, int y, int sz) : x{x}, y{y}, w{sz}, h{sz} { }

    bool operator == (const my_object& o) const {
        return x == o.x && y == o.y && w == o.w && h == o.h;
    }

    bool operator != (const my_object& o) const {
        return !(*this == o);
    }
};

template<typename OStream>
OStream& operator << (OStream& os, const my_object& o)
{
    os << "[" << o.x << ", " << o.y << "; " << o.w << ", " << o.h << "]";
    return os;
}

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
    std::uniform_int_distribution<int> rnd_size(10, 20);
    while (objects.size() < num_samples)
        objects.emplace_back(rnd_pos(eng), rnd_pos(eng), rnd_size(eng));

    std::cout << "Running algorithm..." << std::endl;
    int correct_num = 0;
    int false_num = 0;
    std::vector<std::pair<my_object, my_object>> correct_collisions;
    collision::check_collision(std::begin(objects), std::end(objects), [&](const my_object& obj1, const my_object& obj2) {
        if (!check_collide(obj1, obj2)) {
            std::cerr << obj1 << " x " << obj2 << " - incorrect" << std::endl;
            ++false_num;
        }
        else {
            correct_collisions.emplace_back(obj1, obj2);
            ++correct_num;
        }
    });

    std::cout << "Verifying with brute force..." << std::endl;
    int real_num = 0;
    for (auto a = std::begin(objects); a != std::end(objects); ++a) {
        for (auto b = a + 1; b != std::end(objects); ++b) {
            if (check_collide(*a, *b)) {
                auto it = std::find(std::begin(correct_collisions), std::end(correct_collisions), std::make_pair(*a, *b));
                if (it == correct_collisions.end())
                    std::cerr << *a << " x " << *b << " - missing" << std::endl;
                ++real_num;
            }
        }
    }

    std::cout << num_samples << " objects tested, "
            << real_num << " real collisions, "
            << correct_num << " correct collisions found, "
            << false_num << " false collisions found."
            << std::endl;

    assert(real_num == correct_num && false_num == 0);
}