#include "scene_loader.h"

#include <dlfcn.h>
#include <sys/stat.h>

#include <fmt/printf.h>

#include "exception.h"
#include "scene.h"

SceneLoader::SceneLoader(std::string filename):
    _filename(filename),
    _timestamp{},
    _handle(nullptr),
    _scene{}
{

}

SceneLoader::~SceneLoader()
{
    releaseLibrary(nullptr);
}

void SceneLoader::draw(float ticks, context* ctx)
{
    if(_scene.draw)
        _scene.draw(ticks, ctx);
}

void SceneLoader::update(context* ctx)
{
    // Check if should reload scene
    struct stat st;
    int ret = stat(_filename.c_str(), &st);
    if(ret != -1) {
        if(st.st_mtimespec.tv_sec != _timestamp.tv_sec || 
           st.st_mtimespec.tv_nsec != _timestamp.tv_nsec) {
            releaseLibrary(ctx);
            openLibrary(ctx);

            fmt::printf("Scene reloaded\n");
        }
    }
}

void SceneLoader::releaseLibrary(context* ctx)
{
    if(_scene.release) {
        if(ctx)
            _scene.release(ctx);
    }

    if(_handle) {
        dlclose(_handle);
    }

    _scene = {};
    _handle = nullptr;
    _timestamp = {};
}

void SceneLoader::openLibrary(context* ctx)
{
    struct stat st;
    int ret = stat(_filename.c_str(), &st);
    if(ret == -1) {
        throw Exception(fmt::sprintf("File not found: \"%s\"", _filename));
    }

    void* sceneHandle = dlopen(_filename.c_str(), 0);
    if(!sceneHandle) {
        throw Exception(fmt::sprintf("Failed to load file: \"%s\"", _filename));
    }

    GETSCENEPROC getScene = (GETSCENEPROC)dlsym(sceneHandle, "getScene");
    if(!getScene) {
        dlclose(sceneHandle);
        throw Exception(fmt::sprintf("Invalid scene file: \"%s\"", _filename));
    }

    scene sceneObj = {};
    getScene(&sceneObj);

    sceneObj.init(ctx);

    _handle = sceneHandle;
    _timestamp = st.st_mtimespec;
    _scene = sceneObj;
}


