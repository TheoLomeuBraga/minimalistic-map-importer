#pragma once
#include <string>
#include "glm/glm.hpp"

#ifndef MAP_SCALE
#define MAP_SCALE 0.03125
#endif

class MAP_Plane
{
public:
    MAP_Plane() {}
    glm::vec3 position,normal;
    MAP_Plane(glm::vec3 position , glm::vec3 normal) {
        this->position = position;
        this->normal = normal;
    }
};

