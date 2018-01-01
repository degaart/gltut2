#pragma once

struct context;

struct scene {
    void (*init)(context*);
    void (*release)(context*);
    void (*draw)(float, context*);
};

typedef void (*GETSCENEPROC)(scene*);



