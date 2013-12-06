// collision_object.hpp

#pragma once

namespace collision {
    class collision_group;
    class collision_object {
        float x, y, w, h;
        friend void check_collision(const collision_group& group);
    public:
        collision_object(float x, float y, float w, float h) : x{x}, y{y}, w{w}, h{h} { }
        // void update(float x, float y, float w, float h) {
        //     this->x = x;
        //     this->y = y;
        //     this->w = w;
        //     this->h = h;
        // }
    };
}