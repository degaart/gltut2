#include "image.h"
#include "stb_image.h"
#include "exception.h"
#include <fmt/printf.h>

Image::Image(std::string filename, bool flip):
    _filename(filename)
{
    stbi_set_flip_vertically_on_load(flip);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename.c_str(), 
                                    &width, &height, 
                                    &nrChannels, 
                                    0); 
    if(!data) {
        throw Exception(fmt::format("Failed to load image\"{}\"", filename));
    }

    _width = width;
    _height = height;
    _channels = nrChannels;
    _data = data;
}

Image::~Image()
{
    stbi_image_free(_data);
}

const unsigned char* Image::getData() const
{
    return _data;
}

int Image::getWidth() const
{
    return _width;
}

int Image::getHeight() const
{
    return _height;
}

int Image::getChannels() const
{
    return _channels;
}


