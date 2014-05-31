// collision.hpp

#pragma once

#include <vector>

namespace collision {
    namespace detail {
        enum bound_type {
            inner,
            outer
        };

        struct bound {
            int index;
            bound_type type;
            bound(int idx, bound_type t) : index{idx}, type{t} {}
        };

        template<typename F> void process_line(const std::vector<bound>& cont, F callback)
        {
            for (auto it = std::begin(cont); it != std::end(cont); ++it) {
                if (it->type == bound_type::inner) {
                    for (auto i = it+1; i->index != it->index; ++i)
                        callback(it->index, i->index);
                }
            }
        }
    }

    template<typename RandomIt>
    struct collision_data
    {
        std::vector<detail::bound> x_bounds;
        std::vector<detail::bound> y_bounds;
        RandomIt begin;
        RandomIt end;
        std::size_t num_objects;
    };

    template <typename RandomIt>
    void prepare(RandomIt begin, RandomIt end, collision_data<RandomIt>& data)
    {
        data.begin = begin;
        data.end = end;
        data.num_objects = std::distance(begin, end);
        data.x_bounds.reserve(data.num_objects * 2);
        data.y_bounds.reserve(data.num_objects * 2);
        for (std::size_t i = 0; i < data.num_objects; ++i) {
            data.x_bounds.emplace_back(i, detail::bound_type::inner);
            data.x_bounds.emplace_back(i, detail::bound_type::outer);
            data.y_bounds.emplace_back(i, detail::bound_type::inner);
            data.y_bounds.emplace_back(i, detail::bound_type::outer);
        }

        std::sort(std::begin(data.x_bounds), std::end(data.x_bounds), [&](const detail::bound& a, const detail::bound& b) {
            auto ax = (begin + a.index)->x + ((a.type == detail::bound_type::outer) ? (begin + a.index)->w : 0);
            auto bx = (begin + b.index)->x + ((b.type == detail::bound_type::outer) ? (begin + b.index)->w : 0);
            return ax < bx || (ax == bx && a.type < b.type);    // making sure touching bounds collide
        });
        std::sort(std::begin(data.y_bounds), std::end(data.y_bounds), [&](const detail::bound& a, const detail::bound& b) {
            auto ay = (begin + a.index)->y + ((a.type == detail::bound_type::outer) ? (begin + a.index)->h : 0);
            auto by = (begin + b.index)->y + ((b.type == detail::bound_type::outer) ? (begin + b.index)->h : 0);
            return ay < by || (ay == by && a.type < b.type);    // making sure touching bounds collide
        });
    }

    template <typename RandomIt>
    inline collision_data<RandomIt> make_preparation(RandomIt begin, RandomIt end)
    {
        collision_data<RandomIt> data;
        prepare(begin, end, data);
        return data;
    }

    template <typename RandomIt, typename F>
    void check_collision(const collision_data<RandomIt>& data, F callback)
    {
        std::vector<bool> ctable(data.num_objects * data.num_objects, false);
        detail::process_line(data.x_bounds, [&](int ia, int ib) {
            auto mm = std::minmax(ia, ib);
            ctable[mm.first * data.num_objects + mm.second] = true;
        });
        detail::process_line(data.y_bounds, [&](int ia, int ib) {
            auto mm = std::minmax(ia, ib);
            if (ctable[mm.first * data.num_objects + mm.second]) {
                callback(*(data.begin + mm.first), *(data.begin + mm.second));
                ctable[mm.first * data.num_objects + mm.second] = false;    // marking as reported
            }
        });
    }

    template <typename RandomIt, typename F>
    inline void check_collision(RandomIt begin, RandomIt end, F callback)
    {
        auto data = make_preparation(begin, end);
        check_collision(data, callback);
    }
}

        // TODO: Some memory could be saved based on the fact we don't allow repeating collisions for same pair
        // std::vector<bool> ctable(num_objects * (num_objects / 2 - 1), false);
        // detail::process_line(x_bounds, [&](int a, int b) {
        //     try {
        //         if (a < b) ctable.at(a * num_objects - (a * (a + 1) / 2) - 1 + b) = true;
        //     }
        //     catch (...) {
        //         std::cerr << "Out of bounds: " << a << ":" << b << std::endl;
        //     }
        // });
        // detail::process_line(y_bounds, [&](int a, int b) {
        //     if (ctable[a * num_objects - (a * (a + 1) / 2) - 1 + b])
        //         group.handler(group.objects[a], group.objects[b]);
        // });
