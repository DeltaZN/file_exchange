//
// Created by georgii on 25.03.2021.
//

#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include "cmd_handler.h"
#include "util.h"

const static char* DISPLAY_CMD = "display";
const static char* DOWNLOAD_CMD = "download";
const static char* EXIT_CMD = "exit";

/** non-zero value == terminate programm */
int8_t handle_command(const char* cmd) {
    int8_t ret_code = 0;
    char *args[3];
    args[0] = calloc(1, 256);
    args[1] = calloc(1, 256);
    args[2] = calloc(1, 256);
    parse(cmd, args);
    if (!strcmp(args[0], DISPLAY_CMD)) {
        // access to store and display
    } else if (!strcmp(args[0], DOWNLOAD_CMD)) {
        // start download
    } else if (!strcmp(args[0], EXIT_CMD)) {
        ret_code = 1;
    }

    free(args[0]);
    free(args[1]);
    free(args[2]);

    return ret_code;
}
