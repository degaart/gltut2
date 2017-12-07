#pragma once

#include <string>
#include <memory>

#define BUFFER_OBJECT(i) ((void*)(i))

extern int windowWidth;
extern int windowHeight;

extern std::string resDir;

class text_context;
extern std::shared_ptr<text_context> textContext;

class context {
    public:
        virtual ~context() {}
        virtual void draw(float ticks) = 0;
};


