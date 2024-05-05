#ifndef ENTITY_H
#define ENTITY_H

#ifdef __EMSCRIPTEN__
    #include "../libs/glm/glm.hpp" // not included in git repo, download from glm official repo and add to libs
#else
    #include <glm/glm.hpp>
#endif

#include <SDL.h>

class Entity {
    public:
        Entity(glm::vec2 pos);

        void update();
        void draw();

    private:
        double _seed;
        glm::vec2 _position;
        glm::vec2 _rotation;
};

#endif