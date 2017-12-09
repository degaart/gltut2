#pragma once

#include <string>
#include <memory>

#define BUFFER_OBJECT(i) ((void*)(i))

extern int windowWidth;
extern int windowHeight;

extern std::string resDir;

class text_context;
extern std::shared_ptr<text_context> textContext;

#include "camera.h"
extern Camera camera;

class context {
    public:
        virtual ~context() {}
        virtual void draw(float ticks) = 0;

        static const unsigned KEY_UP    = 0x0001;
        static const unsigned KEY_DOWN  = 0x0002;
        static const unsigned KEY_LEFT  = 0x0004;
        static const unsigned KEY_RIGHT = 0x0008;
        static const unsigned KEY_Z     = 0x0010;
        static const unsigned KEY_S     = 0x0020;
        static const unsigned KEY_Q     = 0x0040;
        static const unsigned KEY_D     = 0x0080;

};


