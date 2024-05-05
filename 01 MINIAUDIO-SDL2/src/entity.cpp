#include "entity.h"

using namespace glm;

Entity::Entity(vec2 pos) {
    _position = position;
    _seed = 1000;
}

void Entity::update() {

}

void Entity::draw(SDL_Renderer* renderer) {
    SDL_RenderDrawLine(renderer);

    for (float i = 0; i < 6; i += 0.5) {
        
    }
}