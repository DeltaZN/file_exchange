//
// Created by georgii on 28.03.2021.
//

#ifndef SPO_LAB_3_FILE_EXCHANGE_UI_MODULE_H
#define SPO_LAB_3_FILE_EXCHANGE_UI_MODULE_H

#include "app_context.h"
#include <curses.h>

typedef struct ui_data {
    WINDOW *download_win;
    WINDOW *upload_win;
    WINDOW *events_win;
    WINDOW *input_win;
    app_context_t *ctx;
} ui_data_t;

void render_transfer_area(ui_data_t *data, int8_t do_clear);
void render_events_log(ui_data_t *data, int8_t do_clear);
void init_ui_data(ui_data_t *data);
void destroy_ui_data(ui_data_t *data);
void render_screen(ui_data_t *ui_data);
void start_ui(ui_data_t *ui_data);
#endif //SPO_LAB_3_FILE_EXCHANGE_UI_MODULE_H
