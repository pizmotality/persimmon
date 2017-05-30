#ifndef _GTP_H
#define _GTP_H

#include <unistd.h>

#include "state.h"

extern pid_t childpid[_NENGINES];

void init_engines(struct arguments arguments);
void reset_engines(int state);
void quit_engines(int state);

char** parse_path(const char* path);
void start_engine(char* engine_path[], int _ENGINE);

void make_move(int state);
void* read_engine(void* _V_ENGINE);

#endif
