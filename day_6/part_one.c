#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../data_structs.h"

#define BUFF_SIZE 4096

typedef struct {
    int time;
    int dist;
} Race;

decl_array(Races, races, Race)

    int main() {
    char *line = calloc(BUFF_SIZE, sizeof(char));
    if (line == NULL)
        return -1;

    Races races = {0};
    if (races_init(&races, 3))
        return -1;

    int line_idx = 0;
    while (fgets(line, BUFF_SIZE, stdin) != NULL) {
        bool first = true;
        int idx = 0;
        for (char *str = line, *tok; (tok = strtok(str, " ")) != NULL;
             str = NULL) {
            if (!first) {
                if (line_idx == 0) {
                    Race race = {.time = atoi(tok)};
                    races_append(&races, race);
                } else {
                    races.items[idx++].dist = atoi(tok);
                }
            } else {
                first = false;
            }
        }
        line_idx++;
    }

    for(int i = 0; i < races.len; i++) {
        printf("race: %d - T: %d D: %d\n", i, races.items[i].time, races.items[i].dist);
    }


    races_free(&races);
    free(line);
    return 0;
}
