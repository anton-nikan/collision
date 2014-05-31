// test_correctness.cpp

#include <set>
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

    bool operator < (const my_object& o) const {
        return x < o.x || (x == o.x && (y < o.y || (y == o.y && (w < o.w || (w == o.w && h < o.h)))));
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

    std::cout << "Running algorithm on " << num_samples << " objects..." << std::endl;
    std::set<std::pair<my_object, my_object>> test_collisions;
    int test_num = 0;
    collision::check_collision(std::begin(objects), std::end(objects), [&](const my_object& obj1, const my_object& obj2) {
        test_collisions.emplace(obj1, obj2);
        ++test_num;
    });

    std::cout << test_num << " collisions found." << std::endl;

    std::cout << "Verifying with brute force..." << std::endl;
    int real_num = 0;
    int false_num = 0;
    for (auto a = std::begin(objects); a != std::end(objects); ++a) {
        for (auto b = a + 1; b != std::end(objects); ++b) {
            auto it = test_collisions.find(std::make_pair(*a, *b));
            if (check_collide(*a, *b)) {
                ++real_num;
                if (it == test_collisions.end())
                    std::cerr << *a << " x " << *b << " - missing" << std::endl;
            }
            else {
                if (it != test_collisions.end()) {
                    std::cerr << *a << " x " << *b << " - false" << std::endl;
                    ++false_num;
                }
            }
        }
    }

    std::cout << num_samples << " objects tested, "
            << real_num << " real collisions, "
            << test_num << " correct collisions, "
            << false_num << " false collisions."
            << std::endl;

    assert(real_num == test_num && false_num == 0);
}