#pragma once

#include <string>
#include <time.h>

#include "scene.h"

struct context;

class SceneLoader {
    private:
        std::string _filename;
        timespec _timestamp;
        void* _handle;
        scene _scene;

        void releaseLibrary(context* ctx);
        void openLibrary(context* ctx);
    public:
        SceneLoader(std::string filename);
        ~SceneLoader();

        SceneLoader(const SceneLoader&) = delete;
        SceneLoader(SceneLoader&&) = delete;
        SceneLoader& operator=(const SceneLoader&) = delete;
        SceneLoader& operator=(SceneLoader&&) = delete;

        void update(context* ctx);
        void draw(float ticks, context* ctx);
};


