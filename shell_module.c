//
// Created by georgii on 28.03.2021.
//

#include <stdint.h>
#include <malloc.h>
#include "shell_module.h"
#include "cmd_handler.h"

char *get_line(void) {
    char *line = malloc(100), *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0) {
            len = lenmax;
            char *linen = realloc(linep, lenmax *= 2);

            if (linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *(line-1) = '\0';
    return linep;
}

void launch_shell(app_context_t *ctx) {
    int8_t exit = 0;
    while (!exit) {
        printf("> ");
        char *line = get_line();
        exit = handle_command(ctx, line);
        free(line);
    }
}