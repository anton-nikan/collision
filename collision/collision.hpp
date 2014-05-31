// collision.hpp

#pragma once

#include <vector>

namespace collision {
    namespace detail {
        enum class bound_type {
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

    template <typename RandomIt, typename F>
    void check_collision(RandomIt begin, RandomIt end, F callback)
    {
        const auto num_objects = std::distance(begin, end);

        std::vector<detail::bound> x_bounds; x_bounds.reserve(num_objects * 2);
        std::vector<detail::bound> y_bounds; y_bounds.reserve(num_objects * 2);

        for (std::size_t i = 0; i < num_objects; ++i) {
            x_bounds.emplace_back(x_bounds.size()/2, detail::bound_type::inner);
            x_bounds.emplace_back(x_bounds.size()/2, detail::bound_type::outer);
            y_bounds.emplace_back(y_bounds.size()/2, detail::bound_type::inner);
            y_bounds.emplace_back(y_bounds.size()/2, detail::bound_type::outer);
        }

        std::sort(std::begin(x_bounds), std::end(x_bounds), [&](const detail::bound& a, const detail::bound& b) {
            auto ax = (begin + a.index)->x + ((a.type == detail::bound_type::outer) ? (begin + a.index)->w : 0);
            auto bx = (begin + b.index)->x + ((b.type == detail::bound_type::outer) ? (begin + b.index)->w : 0);
            return ax < bx;
        });
        std::sort(std::begin(y_bounds), std::end(y_bounds), [&](const detail::bound& a, const detail::bound& b) {
            auto ay = (begin + a.index)->y + ((a.type == detail::bound_type::outer) ? (begin + a.index)->h : 0);
            auto by = (begin + b.index)->y + ((b.type == detail::bound_type::outer) ? (begin + b.index)->h : 0);
            return ay < by;
        });

        std::vector<bool> ctable(num_objects * num_objects, false);
        detail::process_line(x_bounds, [&](int a, int b) {
            if (a < b) ctable.at(a * num_objects + b) = true;
        });
        detail::process_line(y_bounds, [&](int a, int b) {
            if (ctable[a * num_objects + b])
                callback(*(begin + a), *(begin + b));
        });
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
