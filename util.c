//
// Created by georgii on 25.03.2021.
//

#include "util.h"
#include <argp.h>
#include <string.h>
#include <malloc.h>

void get_hex_chars(uint8_t byte, char *hex) {
    unsigned char firstNibble;  // a Nibble is 4 bits, half a byte, one hexadecimal character
    unsigned char secondNibble;


    firstNibble = (byte >> 4);  // isolate first 4 bits

    if (firstNibble < 10U) {
        hex[0] = (char) ('0' + firstNibble);
    } else {
        firstNibble -= 10U;
        hex[0] = (char) ('A' + firstNibble);
    }

    secondNibble = (byte & 0x0F);  // isolate last 4 bits

    if (secondNibble < 10U) {
        hex[1] = (char) ('0' + secondNibble);
    } else {
        secondNibble -= 10U;
        hex[1] = (char) ('A' + secondNibble);
    }
}

int parse(const char *cmd, char **args) {
    const char *p = cmd;
    int count = 0;

    for (;;) {
        while (isspace(*p)) p++;
        if (count >= 3) {
            return count;
        }
        if (*p == '\0') break;

        if (*p == '"' || *p == '\'') {
            int quote = *p++;
            const char *begin = p;

            while (*p && *p != quote) p++;
            if (*p == '\0') return -1;
            strncpy(args[count], begin, p - begin);
            count++;
            p++;
            continue;
        }

        if (strchr("<>()|", *p)) {
            args[count] = calloc(1, 256);
            strncpy(args[count], p, 1);
            count++;
            p++;
            continue;
        }

        if (isalnum(*p) || *p == '.' || *p == '/' || *p == '-' || *p == ':') {
            const char *begin = p;

            while (isalnum(*p) || *p == '.' || *p == '/' || *p == '-' || *p == ':') p++;
            strncpy(args[count], begin, p - begin);
            count++;
            continue;
        }

        return -1;
    }

    return count;
}