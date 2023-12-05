#include "../data_structs.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 4096

typedef struct {
    ulong src_start;
    ulong dst_start;
    ulong length;
} Range;

#define MAPS 7

decl_array(Seeds, seeds, int) decl_array(Maps, maps, Range)

    void parse_seeds(Seeds *seeds, char *line) {
    bool first = true;
    for (char *str = line, *tok; (tok = strtok(str, " ")) != NULL; str = NULL) {
        if (!first) {
            int curr = atoi(tok);
            seeds_append(seeds, curr);
        } else {
            first = false;
        }
    }
}

void parse_map(Maps *maps, char *line) {
    Range range = {0};
    int state = 0;
    for (char *str = line, *tok; (tok = strtok(str, " ")) != NULL; str = NULL) {
        int curr = atoi(tok);
        switch (state) {
        case 0:
            range.dst_start = curr;
            break;
        case 1:
            range.src_start = curr;
            break;
        case 2:
            range.length = curr;
            break;
        }
        state++;
    }

    maps_append(maps, range);
}

ulong map_value(ulong input, const Maps *map) {
    const Range *range = map->items;

    for (size_t i = 0; i < map->len; i++) {
        if (input >= range->src_start) {
            ulong delta = input - range->src_start;
            if (delta < range->length) {
                return range->dst_start + delta;
            }
        }
        range++;
    }

    return input;
}

ulong full_map_value(ulong input, Maps *all_maps, int len) {
    for (int i = 0; i < len; i++) {
        input = map_value(input, all_maps++);
    }
    return input;
}

int main() {

    char *buff = calloc(BUFF_SIZE, sizeof(char));
    if (buff == NULL)
        return -1;

    Seeds seeds = {0};
    seeds_init(&seeds, 100);

    Maps all_maps[MAPS] = {0};
    for (int i = 0; i < MAPS; i++) {
        maps_init(&all_maps[i], 100);
    }

    int map_idx = -1;
    int state = 0;
    bool first = true;

    while (fgets(buff, BUFF_SIZE, stdin) != NULL) {
        if (state == 0) {
            parse_seeds(&seeds, buff);
            state = 1;
        } else {
            if (strlen(buff) > 1) {
                if (first) {
                    first = false;
                } else {
                    parse_map(all_maps + map_idx, buff);
                }
            } else {
                map_idx++;
                first = true;
            }
        }
    }

    ulong min = 1e10;
    for (size_t i = 0; i < seeds.len; i += 2) {
        ulong seed = seeds.items[i];
        ulong range = seeds.items[i + 1];
        while (--range) {
            ulong location = full_map_value(seed++, all_maps, MAPS);
            if (location < min) {
                min = location;
            }
        }
    }

    printf("%ld\n", min);

    for (int i = 0; i < MAPS; i++) {
        maps_free(&all_maps[i]);
    }
    seeds_free(&seeds);
    free(buff);

    return 0;
}
