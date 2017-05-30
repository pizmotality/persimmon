#ifndef _STATE_H
#define _STATE_H

#define _D_ENGINE 0
#define _B_ENGINE 1
#define _W_ENGINE 2
#define _NENGINES 3

struct arguments {
    int size;
    float komi;
    int state;
    char* paths[_NENGINES];
};

void init_state(struct arguments* arguments, int argc, char* argv[]);

#endif
