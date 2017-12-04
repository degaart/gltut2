#pragma once

#include <string>

#define BUFFER_OBJECT(i) ((void*)(i))

extern int windowWidth;
extern int windowHeight;

extern std::string resDir;

class context {
    public:
        virtual ~context() {}
        virtual void draw(float ticks) = 0;
};


