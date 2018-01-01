#pragma once

#include "context.h"
#include <memory>

class text_context_impl;

class text_context : public context {
    private:
        std::shared_ptr<text_context_impl> impl;
    public:
        text_context();
        void draw(float ticks);
        void drawText(float x, float y, const std::string& text);
};

std::shared_ptr<text_context> make_text_context();

