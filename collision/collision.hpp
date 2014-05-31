// collision.hpp

#pragma once

#include <vector>
#include <cassert>
#include <collision/collision_group.hpp>

namespace collision {
    namespace detail {
        enum class bound_type {
            inner,
            outer
        };
        struct bound {
            float pos;
            int index;
            bound_type type;

            bound(float pos, int index, bound_type type) : pos{pos}, index{index}, type{type} { }
        };

        template<typename F> void process_bound(const std::vector<bound>& cont, F callback)
        {
            for (auto it = cont.begin(); it != cont.end(); ++it) {
                if (it->type == bound_type::inner) {
                    // collisions
                    for (auto i = it+1; i->index != it->index; ++i)
                        callback(it->index, i->index);
                }
            }
        }
    }

    void check_collision(const collision_group& group)
    {
        std::vector<detail::bound> x_bounds; x_bounds.reserve(group.objects.size() * 2);
        std::vector<detail::bound> y_bounds; y_bounds.reserve(group.objects.size() * 2);

        for (const auto& obj: group.objects) {
            x_bounds.emplace_back(obj.x-0.5*obj.w, x_bounds.size()/2, detail::bound_type::inner);
            x_bounds.emplace_back(obj.x+0.5*obj.w, x_bounds.size()/2, detail::bound_type::outer);
            y_bounds.emplace_back(obj.y-0.5*obj.h, y_bounds.size()/2, detail::bound_type::inner);
            y_bounds.emplace_back(obj.y+0.5*obj.h, y_bounds.size()/2, detail::bound_type::outer);
        }

        std::sort(std::begin(x_bounds), std::end(x_bounds), [](const detail::bound& a, const detail::bound& b) { return a.pos < b.pos; });
        std::sort(std::begin(y_bounds), std::end(y_bounds), [](const detail::bound& a, const detail::bound& b) { return a.pos < b.pos; });

        std::vector<bool> ctable(group.objects.size() * group.objects.size(), false);
        detail::process_bound(x_bounds, [&](int a, int b) {
            if (a < b) ctable[a * group.objects.size() + b] = true;
        });
        detail::process_bound(y_bounds, [&](int a, int b) {
            if (ctable[a * group.objects.size() + b])
                group.handler(group.objects[a], group.objects[b]);
        });
    }
}