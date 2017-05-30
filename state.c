#include <argp.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"

const char* argp_program_version = "persimmon - version 0.0";
static char doc[] = "persimmon - gtp client for x";

static struct argp_option options[] = {
    {"size", 's', "size", 0, "Set board size"},
    {"komi", 'k', "komi", 0, "Set komi"},
    {"black", 'b', "path", 0, "Set black engine path"},
    {"white", 'w', "path", 0, "Set white engine path"},
    {"default", 'd', "path", 0, "Set default engine path"},
    {0}
};

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;

    switch (key) {
        case 's':
            arguments->size = atoi(arg);
            break;
        case 'k':
            arguments->komi = atof(arg);
            break;
        case 'b':
            arguments->paths[_B_ENGINE] = arg;
            arguments->state |= _B_ENGINE;
            break;
        case 'w':
            arguments->paths[_W_ENGINE] = arg;
            arguments->state |= _W_ENGINE;
            break;
        case 'd':
            arguments->paths[_D_ENGINE] = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, NULL, doc};

void init_state(struct arguments* arguments, int argc, char* argv[]) {
    argp_parse(&argp, argc, argv, 0, 0, arguments);

    arguments->paths[_D_ENGINE] = "/usr/bin/pachi";
}
