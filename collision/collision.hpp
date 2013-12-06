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
            std::vector<int> open;
            for (auto& bound: cont) {
                if (bound.type == bound_type::inner)
                    open.push_back(bound.index);
                else if (open.back() == bound.index)
                    open.pop_back();
                else {  // collision
                    for (auto i = open.end() - 1; i >= open.begin(); --i) {
                        if (*i == bound.index) {
                            open.erase(i);
                            break;
                        }
                        callback(bound.index, *i);
                    }
                }
            }
            assert(open.size() == 0);
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
            ctable[a * group.objects.size() + b] = true;
            ctable[b * group.objects.size() + a] = true;
        });
        detail::process_bound(y_bounds, [&](int a, int b) {
            if (ctable[a * group.objects.size() + b] || ctable[b * group.objects.size() + a])
                group.handler(group.objects[a], group.objects[b]);
        });
    }
}