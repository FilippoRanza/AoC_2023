#include "../data_structs.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 4096
#define KEY_SZ 4

DECL_ARRAY(Moves, moves, char)

typedef struct {
    char key[KEY_SZ];
    char left[KEY_SZ];
    char right[KEY_SZ];
} Element;

DECL_ARRAY(Elements, elements, Element)

typedef struct {
    Moves moves;
    Elements elements;
} Instance;

int instance_init(Instance *inst) {
    if (moves_init(&inst->moves, 100)) {
        return 1;
    }

    if (elements_init(&inst->elements, 100)) {
        moves_free(&inst->moves);
        return 1;
    }

    return 0;
}

void instance_free(Instance *inst) {
    moves_free(&inst->moves);
    elements_free(&inst->elements);
}

int parse_moves(char *line, Instance *inst) {

    char ch;
    while ((ch = *line++)) {
        char c = -1;
        switch (ch) {
        case 'L':
            c = 0;
            break;
        case 'R':
            c = 1;
            break;
        }
        if (c != -1) {
            if (moves_append(&inst->moves, c)) {
                return 1;
            }
        }
    }

    return 0;
}

int parse_element(char *line, Instance *inst) {

    int state = 0;
    Element element = {0};
    for (char *str = line, *tok; (tok = strtok(str, " =(,)")) != NULL;
         str = NULL) {
        switch (state) {
        case 0:
            strcpy(element.key, tok);
            break;
        case 1:
            strcpy(element.left, tok);
            break;
        case 2:
            strcpy(element.right, tok);
            break;
        }
        state++;
    }

    return elements_append(&inst->elements, element);
}

#define parse_or_fail(f)                                                       \
    if (f) {                                                                   \
        return -1;                                                             \
    }

int load_input(Instance *inst, FILE *file) {

    char *buff = calloc(BUFF_SIZE, sizeof(char));
    if (buff == NULL)
        return -1;

    int state = 0;
    while (fgets(buff, BUFF_SIZE, file) != NULL) {
        switch (state) {
        case 0:
            parse_or_fail(parse_moves(buff, inst));
            state = 1;
            break;
        case 1:
            state = 2;
            break;
        case 2:
            parse_or_fail(parse_element(buff, inst));
            break;
        }
    }

    free(buff);
    return 0;
}

unsigned int hash(char *str) {
    unsigned int output = 0xF54E3B2C;

    char c;
    while ((c = *str++)) {
        output += 13 * c;
    }

    return output;
}

typedef struct {
    char key[KEY_SZ];
    int value;
} Item;

typedef struct {
    Item *buff;
    int len;
} Map;

int map_init(Map *map, int len) {
    Item *buff = calloc(sizeof(Item), len);
    if (buff == NULL)
        return 1;

    map->buff = buff;
    map->len = len;

    return 0;
}

void map_set(Map *map, char *str, int value) {
    unsigned int idx = hash(str) % map->len;

    while (map->buff[idx].key[0]) {
        idx = (idx + 1) % map->len;
    }
    map->buff[idx].value = value;
    strcpy(map->buff[idx].key, str);
}

int map_get(Map *map, char *str) {
    unsigned int idx = hash(str) % map->len;

    char *key;
    while ((key = map->buff[idx].key) && strcmp(key, str) != 0 && key[0]) {
        idx = (idx + 1) % map->len;
    }

    if (key[0] == 0)
        return -1;

    return map->buff[idx].value;
}

void map_free(Map *map) {
    free(map->buff);
    map->buff = NULL;
    map->len = 0;
}

#define check_or_insert(str)                                                   \
    if (map_get(map, str) == -1) {                                             \
        map_set(map, str, idx++);                                              \
    }

int build_name_mapping(Instance *inst, Map *map) {
    int cap = 3 * inst->elements.len;
    if (map_init(map, cap))
        return -1;

    int idx = 0;
    for (size_t i = 0; i < inst->elements.len; i++) {
        Element *el = inst->elements.items + i;
        check_or_insert(el->key);
        check_or_insert(el->left);
        check_or_insert(el->right);
    }

    return idx;
}

typedef struct {
    int left;
    int right;
} Arc;

Arc *build_adj_list(Instance *inst, Map *map) {
    Arc *output = malloc(sizeof(Arc) * inst->elements.len);
    if (output == NULL)
        return NULL;

    for (size_t i = 0; i < inst->elements.len; i++) {
        Element *el = inst->elements.items + i;

        int key = map_get(map, el->key);
        int left = map_get(map, el->left);
        int right = map_get(map, el->right);

        output[key].left = left;
        output[key].right = right;
    }

    return output;
}

typedef struct {
    int *start;
    int *dests;
    int len;
} Places;

int places_init(Places *places, int size) {

    int *start = malloc(sizeof(int) * size);
    if (start == NULL)
        return 1;

    int *dests = malloc(sizeof(int) * size);
    if (dests == NULL)
        return 1;

    places->start = start;
    places->dests = dests;
    places->len = size;

    return 0;
}

void places_free(Places *places) {
    free(places->start);
    free(places->dests);
}

int fill_locations(Instance *inst, Map *map, Places *places) {

    int n_as = 0;
    int n_zs = 0;

    for (size_t i = 0; i < inst->elements.len; i++) {
        char *key = inst->elements.items[i].key;
        if (key[2] == 'A')
            n_as++;
        if (key[2] == 'Z')
            n_zs++;
    }

    if (n_as != n_zs)
        return 1;

    if (places_init(places, n_as))
        return 1;

    int a_idx = 0;
    int z_idx = 0;
    for (size_t i = 0; i < inst->elements.len; i++) {
        char *key = inst->elements.items[i].key;
        if (key[2] == 'A')
            places->start[a_idx++] = map_get(map, key);
        if (key[2] == 'Z')
            places->dests[z_idx++] = map_get(map, key);
    }

    return 0;
}

bool is_done(int curr, Places *places) {

    for (int i = 0; i < places->len; i++) {
        if (curr == places->dests[i])
            return true;
    }

    return false;
}

int find_loop_size(Arc *arcs, int curr, Places *places, Moves *moves) {
    int steps = 0;
    int idx = 0;
    while (!is_done(curr, places)) {
        char mv = moves->items[idx];
        if (mv) {
            curr = arcs[curr].right;
        } else {
            curr = arcs[curr].left;
        }

        idx = (idx + 1) % moves->len;
        steps++;
    }

    return steps;
}

long gcd(long a, long b) {
    if (a < b) {
        long tmp = a;
        a = b;
        b = tmp;
    }

    while (b != 0) {
        long tmp = a % b;
        a = b;
        b = tmp;
    }

    return a;
}

long find_path_len(Arc *arcs, Places *places, Instance *instance) {
    long prod;
    bool first = true;
    for (int i = 0; i < places->len; i++) {
        long curr =
            find_loop_size(arcs, places->start[i], places, &instance->moves);

        if (first) {
            prod = curr;
            first = false;
        } else {
            long div = gcd(prod, curr);
            prod = (curr * prod) / div;
        }
    }

    return prod;
}

int main() {

    Instance instance = {0};
    if (instance_init(&instance))
        return -1;

    if (load_input(&instance, stdin))
        return -1;

    Map map = {0};
    if (build_name_mapping(&instance, &map) < 0)
        return -1;

    Arc *arcs = build_adj_list(&instance, &map);
    if (arcs == NULL)
        return -1;

    Places places = {0};
    if (fill_locations(&instance, &map, &places))
        return -1;

    long steps = find_path_len(arcs, &places, &instance);
    printf("%ld\n", steps);

    places_free(&places);
    free(arcs);
    map_free(&map);
    instance_free(&instance);

    return 0;
}
