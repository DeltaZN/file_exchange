//
// Created by georgii on 24.03.2021.
//

#include <dirent.h>
#include <stdio.h>

int run_file_reader(char* dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == 0x0) {
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return 0;
}
