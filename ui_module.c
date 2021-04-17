//
// Created by georgii on 28.03.2021.
//

#include <stdint.h>
#include <malloc.h>
#include <ncurses.h>
#include <string.h>
#include "ui_module.h"
#include "cmd_handler.h"

void render_transfer_area(app_context_t *ctx, int32_t start, int32_t end, int32_t cols) {
    mvprintw(start, 0, "Downloading");
    mvprintw(start, cols / 2, "Uploading");
    for (int32_t i = 0; i < cols; i++) {
        mvprintw(start + 1, i, "-");
    }
    for (int32_t i = 0; i < cols; i++) {
        mvprintw(end, i, "_");
    }
    for (int32_t i = start; i <= end; i++) {
        mvprintw(i, cols / 2 - 1, "|");
    }
    int32_t available_rows = end - start - 2;
    int32_t transfer_start = start + 2;
    for (int32_t i = 0; i < available_rows; i ++) {
        list_item_t *action = get(ctx->events_module->download_list, i);
        if (action) {
            transfer_progress_t *progress = action->data;
            mvprintw(transfer_start + i, 0, progress->triplet.filename);
            size_t name_len = strlen(progress->triplet.filename);
            mvprintw(transfer_start + i, name_len + 1, "%f/%fMB|[xxxx....]", progress->transferred / 1024.0 / 1024.0, progress->triplet.filesize / 1024.0 / 1024.0);
        } else {
            break;
        }
    }
    for (int32_t i = 0; i < available_rows; i ++) {
        list_item_t *action = get(ctx->events_module->upload_list, i);
        if (action) {
            transfer_progress_t *progress = action->data;
            mvprintw(transfer_start + i, 0, progress->triplet.filename);
            size_t name_len = strlen(progress->triplet.filename);
            mvprintw(transfer_start + i, name_len + 1, "%f/%fMB|[xxxx....]", progress->transferred / 1024.0 / 1024.0, progress->triplet.filesize / 1024.0 / 1024.0);
        } else {
            break;
        }
    }
}

void render_events_log(app_context_t *ctx, int32_t start, int32_t end, int32_t cols) {
    for (int32_t i = 0; i < cols; i++) {
        mvprintw(start, i, "_");
    }
    mvprintw(start + 1, 0, "Actions/events log");
    for (int32_t i = 0; i < cols; i++) {
        mvprintw(end, i, "_");
    }
    int32_t available_rows = end - start - 2;
    int32_t log_start = start + 2;
    for (int32_t i = 0; i < available_rows; i ++) {
        list_item_t *action = get(ctx->events_module->actions_list, i);
        if (action) {
            mvprintw(log_start + i, 0, action->data);
        } else {
            break;
        }
    }
}

void render_input_field(int32_t start) {
    mvprintw(start, 0,"> ");
}

void render_screen(app_context_t *ctx, int8_t render_input) {
    int32_t row,col;
    getmaxyx(stdscr,row,col);
    int32_t transfer_area_start = 0;
    int32_t transfer_area_end = row / 2 - 2;
    int32_t events_log_start = row / 2 - 1;
    int32_t events_log_end = row - 2;
    int32_t input_start = row - 1;

    clear();
    render_transfer_area(ctx, transfer_area_start, transfer_area_end, col);
    render_events_log(ctx, events_log_start, events_log_end, col);
    if (render_input) {
        render_input_field(input_start);
    }
}

void launch_shell(app_context_t *ctx) {
    initscr();
    int8_t exit = 0;
    while (!exit) {
        char *str = calloc(1, 256);
        render_screen(ctx, 1);
        mvprintw(LINES - 1, 0,"> ");
        getstr(str);
        exit = handle_command(ctx, str);
        free(str);
    }
    endwin();
}