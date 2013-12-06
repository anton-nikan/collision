// collision_group.hpp

#pragma once

#include <vector>
#include <functional>
#include <collision/collision_object.hpp>

namespace collision {
    class collision_group {
        std::vector<collision_object> objects;
        std::function<void(const collision_object&, const collision_object&)> handler;

        friend void check_collision(const collision_group& group);
    public:
        void add(collision_object obj) {
            objects.push_back(obj);
        }
        void on_collide(std::function<void(const collision_object&, const collision_object&)> callback) {
            handler = callback;
        }
    };
}