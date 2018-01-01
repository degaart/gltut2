#pragma once

#include <string>

class Image {
    public:
        Image(std::string filename, bool flip = false);
        Image(const Image&) = delete;
        ~Image();
        Image& operator=(const Image&) = delete;

        const unsigned char* getData() const;
        int getWidth() const;
        int getHeight() const;
        int getChannels() const;
    private:
        unsigned char* _data;
        std::string _filename;
        int _width;
        int _height;
        int _channels;
};

