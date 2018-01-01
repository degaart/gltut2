#pragma once

#include <string>
#include <memory>
#include "camera.h"

#define BUFFER_OBJECT(i) ((void*)(i))

struct context {
    int windowWidth;
    int windowHeight;
    std::string resDir;
    Camera camera;
};




