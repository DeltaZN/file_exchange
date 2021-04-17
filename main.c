#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include "file_reader.h"
#include "list.h"
#include "net/udp_server.h"
#include "ui_module.h"
#include "events_module.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("[MAIN] Please, specify working directory\n");
        return 0;
    } else {
        list_item_t *list;
        int8_t ret_code = run_file_reader(argv[1], &list);
        if (ret_code == -1) {
            printf("[MAIN] Unable to access directory\n");
            return 0;
        }

        app_context_t *ctx = calloc(1, sizeof(app_context_t));
        events_module_data_t *events = calloc(1, sizeof(events_module_data_t));
        init_events_module(events);
        ctx->events_module = events;
        ctx->triplet_list = list;
        ctx->exit = 0;

        ui_data_t *ui_data = malloc(sizeof(ui_data_t));
        init_ui_data(ui_data);
        events->ui_data = ui_data;
        ui_data->ctx = ctx;

        pthread_t *udp_server = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create(udp_server, NULL, start_udp_server, ctx);

        launch_shell(ui_data);

        pthread_join(*udp_server, NULL);

        destroy_list(list, (int (*)(void *)) destroy_file_triplet);
        destroy_events_module(ctx->events_module);
        free(ctx);
    }
}
