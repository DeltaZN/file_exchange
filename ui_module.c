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

void render_transfer_headers(ui_data_t *data) {
    mvprintw(0, 1, "Downloading");
    mvprintw(0, COLS / 2 + 2, "Uploading");
    refresh();
}

void render_download_window(ui_data_t *data, int8_t do_clear) {
    int rows, cols;
    getmaxyx(data->download_win, rows, cols);
    if (do_clear) {
        wclear(data->download_win);
        box(data->download_win, 0, 0);
    }
    int32_t available_rows = cols - 2;
    int32_t transfer_start = 1;
    for (int32_t i = 0; i < available_rows; i++) {
        list_item_t *action = get(data->ctx->events_module->download_list, i);
        if (action) {
            transfer_progress_t *progress = action->data;
            size_t name_len = strlen(progress->triplet.filename);

            wmove(data->download_win, transfer_start + i, 1);
            wprintw(data->download_win, progress->triplet.filename);
            wmove(data->download_win, transfer_start + i, name_len + 2);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->download_win, "%f/%fMB|[", transferred, all);
            float percent = transferred / all;
            int32_t xs = percent * 10;
            int32_t dots = 10 - xs;
            for (int j = 0; j < xs; ++j) {
                wprintw(data->download_win, "x");
            }
            for (int j = 0; j < dots; ++j) {
                wprintw(data->download_win, ".");
            }
            wprintw(data->download_win, "]");
        } else {
            break;
        }
    }
    wrefresh(data->download_win);
}

void render_upload_window(ui_data_t *data, int8_t do_clear) {
    int rows, cols;
    getmaxyx(data->upload_win, rows, cols);
    if (do_clear) {
        wclear(data->upload_win);
        box(data->upload_win, 0, 0);
    }
    int32_t available_rows = cols - 2;
    int32_t transfer_start = 1;
    for (int32_t i = 0; i < available_rows; i++) {
        list_item_t *action = get(data->ctx->events_module->upload_list, i);
        if (action) {
            transfer_progress_t *progress = action->data;
            size_t name_len = strlen(progress->triplet.filename);

            wmove(data->upload_win, transfer_start + i, 1);
            wprintw(data->upload_win, progress->triplet.filename);
            wmove(data->upload_win, transfer_start + i, name_len + 2);
            float transferred = progress->transferred / 1024.0 / 1024.0;
            float all = progress->triplet.filesize / 1024.0 / 1024.0;
            wprintw(data->upload_win, "%f/%fMB|[", transferred, all);
            float percent = transferred / all;
            int32_t xs = percent * 10;
            int32_t dots = 10 - xs;
            for (int j = 0; j < xs; ++j) {
                wprintw(data->upload_win, "x");
            }
            for (int j = 0; j < dots; ++j) {
                wprintw(data->upload_win, ".");
            }
            wprintw(data->upload_win, "]");
        } else {
            break;
        }
    }
    wrefresh(data->upload_win);
}

void render_transfer_area(ui_data_t *data, int8_t do_clear) {
    render_download_window(data, do_clear);
    render_upload_window(data, do_clear);
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
    int32_t transfer_area_start = 1;
    int32_t transfer_area_end = row / 2 - 1;
    int32_t events_log_start = row / 2 - 1;
    int32_t events_log_end = row - 1;
    int32_t input_start = row - 1;

    data->download_win = newwin(transfer_area_end - transfer_area_start, col / 2, transfer_area_start, 0);
    data->upload_win = newwin(transfer_area_end - transfer_area_start, col / 2, transfer_area_start, col / 2 + 1);
    data->events_win = newwin(events_log_end - events_log_start, col, events_log_start, 0);
    data->input_win = newwin(1, col, input_start, 0);
}

void destroy_ui_data(ui_data_t *data) {
    delwin(data->download_win);
    delwin(data->upload_win);
    delwin(data->events_win);
    delwin(data->input_win);
}

void render_screen(ui_data_t *ui_data) {
    render_events_log(ui_data, 1);
    render_input_field(ui_data);
}

void start_ui(ui_data_t *ui_data) {
    render_transfer_headers(ui_data);
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