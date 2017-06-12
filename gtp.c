#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pthread.h>

#include "gtp.h"

#include <stdio.h>

#define _DEF_NARG 4
#define _MAX_NARG 16

pid_t childpid[_NENGINES] = {0};

int fd[_NENGINES][2] = {{0, 1}};
int parity = 0x00;

void print_pids() {
    printf("childpids:");
    for (int i = 0; i < _NENGINES; ++i)
        printf(" %i", childpid[i]);
    putchar('\n');
}

void print_fds() {
    printf("fds:");
    for (int i = 0; i < _NENGINES; ++i)
        printf(" [r: %i, w: %i]", fd[i][0], fd[i][1]);
    putchar('\n');
}

void init_engines(struct arguments arguments) {
    char command[64];
    sprintf(command, "boardsize %i\n" "komi %f\n",
        arguments.size, arguments.komi);

    for (int _ENGINE = 0; _ENGINE < _NENGINES; ++_ENGINE) {
        if ((!arguments.state && !_ENGINE) || (arguments.state & _ENGINE)) {
            start_engine(parse_path(arguments.paths[_ENGINE]), _ENGINE);
            write(fd[_ENGINE][1], command, strlen(command));
        }
    }

    if (arguments.state != 0x3) {
        pthread_t r_thread;
        pthread_create(&r_thread, NULL, read_engine,
            (void*)(intptr_t)arguments.state);
    }

    parity = 0x1;
}

void reset_engines(int state) {
    for (int _ENGINE = 0; _ENGINE < _NENGINES; ++_ENGINE)
        if ((!state && !_ENGINE) || (state & _ENGINE))
            write(fd[_ENGINE][1], "clear_board\n", 12);

    parity = 0x1;
}

void quit_engines(int state) {
    for (int _ENGINE = 0; _ENGINE < _NENGINES; ++_ENGINE)
        if ((!state && !_ENGINE) || (state & _ENGINE))
            write(fd[_ENGINE][1], "quit\n", 5);
}

char** parse_path(const char* engine_path) {
    char* path = strdup(engine_path);
    printf("path: %s\n", path);

    char** parsed_path = (char**)malloc(sizeof(char*) * _DEF_NARG);
    int engine_narg = 0;

    for (char* p = strtok(path, " "); p; p = strtok(NULL, " ")) {
        printf("%s\n", p);
        parsed_path[engine_narg++] = strdup(p);

        if (engine_narg == _DEF_NARG)
            engine_path = realloc(parsed_path, sizeof(char*) * _MAX_NARG);
    }
    parsed_path[engine_narg] = NULL;

    return parsed_path;
}

void start_engine(char* engine_path[], int _ENGINE) {
    int r_pipe[2];
    int w_pipe[2];

    pipe(r_pipe);
    pipe(w_pipe);

    childpid[_ENGINE] = fork();
    if (childpid[_ENGINE] == 0) {
        dup2(w_pipe[0], 0);
        dup2(r_pipe[1], 1);

        close(w_pipe[1]);
        close(r_pipe[0]);

        execvp(engine_path[0], engine_path);
    } else {
        close(w_pipe[0]);
        close(r_pipe[1]);

        fd[_ENGINE][0] = r_pipe[0];
        fd[_ENGINE][1] = w_pipe[1];
    }
}

void make_move(int state) {
    char command[16];

    char sides[3] = {' ', 'b', 'w'};
    if (state & parity) {
        sprintf(command, "genmove %c\n", sides[parity]);
    } else {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(0, &fdset);

        select(1, &fdset, NULL, NULL, NULL);
        int nbytes = read(0, command, 16);
        command[nbytes] = '\0';
    }

    int w_fd = (state & parity) ? parity : state;
    printf("write to fd: %i, command: %s", fd[w_fd][1], command);

    if (state == (_B_ENGINE | _W_ENGINE)) {
        char discard[256];
        while (read(fd[w_fd][0], discard, 256) == 256);

        write(fd[w_fd][1], command, strlen(command));

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(fd[parity][0], &fdset);

        select(fd[parity][0] + 1, &fdset, NULL, NULL, NULL);
        int nbytes = read(fd[parity][0], command, 16);
        command[nbytes] = '\0';
        printf("read from fd: %i, output: %s", fd[parity][0], command);

        char* move = strdup(command + 2);
        for (int c = 0; c < strlen(move); ++c) {
            if (move[c] == '\n')
                move[c] = '\0';
        }

        sprintf(command, "play %c %s\n", sides[parity], move);
        parity ^= 0x3;
        write(fd[parity][1], command, strlen(command));
    } else {
        write(fd[w_fd][1], command, strlen(command));
        parity ^= 0x3;
    }
}

void* read_engine(void* _V_ENGINE) {
    char buffer[128];

    int _ENGINE = (intptr_t)_V_ENGINE;
    int r_fd = fd[_ENGINE][0];

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(r_fd, &fdset);

    while (!waitpid(childpid[_ENGINE], 0, WNOHANG)) {
        if (select(r_fd + 1, &fdset, NULL, NULL, NULL) != -1) {
            read(r_fd, buffer, 128);
            printf("read from fd: %i: %s", r_fd, buffer);
        }
    }

    childpid[_ENGINE] = 0;
    printf("engine stopped\n");

    return NULL;
}
