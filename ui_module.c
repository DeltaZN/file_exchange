//
// Created by georgii on 28.03.2021.
//

#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include "ui_module.h"
#include "cmd_handler.h"

void focus_input(ui_data_t *data) {
    int32_t x, y;
    getyx(data->input_win, y, x);
    wmove(data->input_win, y, x);
}

void render_transfer_area(ui_data_t *data, int8_t do_clear) {
    int rows, cols;
    getmaxyx(data->transfer_win, rows, cols);
    if (do_clear) {
        wclear(data->transfer_win);
        box(data->transfer_win, 0, 0);

        wmove(data->transfer_win, 0, 0);
        wprintw(data->transfer_win, "Downloading");
        wmove(data->transfer_win, 0, cols / 2);
        wprintw(data->transfer_win, "Uploading");
        for (int32_t i = 0; i < rows; i++) {
            wmove(data->transfer_win, i, cols / 2 - 1);
            wprintw(data->transfer_win, "|");
        }
    }

    int32_t available_rows = cols - 2;
    int32_t transfer_start = 1;
    for (int32_t i = 0; i < available_rows; i++) {
        list_item_t *action = get(data->ctx->events_module->download_list, i);
        if (action) {
            transfer_progress_t *progress = action->data;
            size_t name_len = strlen(progress->triplet.filename);

            wmove(data->transfer_win, transfer_start + i, 1);
            wprintw(data->transfer_win, progress->triplet.filename);
            wmove(data->transfer_win, transfer_start + i, name_len + 2);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->transfer_win, "%f/%fMB|[", transferred, all);
            float percent = transferred / all;
            int32_t xs = percent * 10;
            int32_t dots = 10 - xs;
            for (int j = 0; j < xs; ++j) {
                wprintw(data->transfer_win, "x");
            }
            for (int j = 0; j < dots; ++j) {
                wprintw(data->transfer_win, ".");
            }
            wprintw(data->transfer_win, "]");
        } else {
            break;
        }
    }
    for (int32_t i = 0; i < available_rows; i++) {
        list_item_t *action = get(data->ctx->events_module->upload_list, i);
        if (action) {
            transfer_progress_t *progress = action->data;
            size_t name_len = strlen(progress->triplet.filename);

            wmove(data->transfer_win, transfer_start + i, cols / 2 + 2);
            wprintw(data->transfer_win, progress->triplet.filename);
            wmove(data->transfer_win, transfer_start + i, cols / 2 + 2 + name_len + 1);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->transfer_win, "%f/%fMB|[", transferred, all);
            float percent = transferred / all;
            int32_t xs = percent * 10;
            int32_t dots = 10 - xs;
            for (int j = 0; j < xs; ++j) {
                wprintw(data->transfer_win, "x");
            }
            for (int j = 0; j < dots; ++j) {
                wprintw(data->transfer_win, ".");
            }
            wprintw(data->transfer_win, "]");
        } else {
            break;
        }
    }
    wrefresh(data->transfer_win);
    focus_input(data);
}

void render_events_log(ui_data_t *data, int8_t do_clear) {
    int rows, cols;
    getmaxyx(data->events_win, rows, cols);

    if (do_clear) {
        wclear(data->events_win);
        box(data->events_win, 0, 0);
        wmove(data->events_win, 1, 1);
        wprintw(data->events_win, "Actions/events log");
    }

    int32_t available_rows = rows - 3;
    int32_t log_start = 2;
    for (int32_t i = 0; i < available_rows; i++) {
        list_item_t *action = get(data->ctx->events_module->actions_list, i);
        if (action) {
            wmove(data->events_win, log_start + i, 1);
            wprintw(data->events_win, action->data);
        } else {
            break;
        }
    }
    wrefresh(data->events_win);
    focus_input(data);
}

void render_input_field(ui_data_t *data) {
    wclear(data->input_win);
    wprintw(data->input_win, "> ");
    wrefresh(data->input_win);
    focus_input(data);
}

void init_ui_data(ui_data_t *data) {
    initscr();
    curs_set(0);
    refresh();

    int32_t row, col;
    getmaxyx(stdscr, row, col);
    int32_t transfer_area_start = 0;
    int32_t transfer_area_end = row / 2 - 1;
    int32_t events_log_start = row / 2 - 1;
    int32_t events_log_end = row - 1;
    int32_t input_start = row - 1;

    data->transfer_win = newwin(transfer_area_end - transfer_area_start, COLS, transfer_area_start, 0);
    data->events_win = newwin(events_log_end - events_log_start, COLS, events_log_start, 0);
    data->input_win = newwin(1, COLS, input_start, 0);
}

void destroy_ui_data(ui_data_t *data) {
    delwin(data->transfer_win);
    delwin(data->events_win);
    delwin(data->input_win);
}

void render_screen(ui_data_t *ui_data) {
    render_events_log(ui_data, 1);
    render_input_field(ui_data);
}

void launch_shell(ui_data_t *ui_data) {
    render_transfer_area(ui_data, 1);
    render_events_log(ui_data, 1);
    render_input_field(ui_data);
    int8_t exit = 0;
    while (!exit) {
        char *str = calloc(1, 256);
        usleep(1000 * 50);
        render_screen(ui_data);
        wgetstr(ui_data->input_win, str);
        exit = handle_command(ui_data->ctx, str);
        free(str);
    }
    destroy_ui_data(ui_data);
    endwin();
}