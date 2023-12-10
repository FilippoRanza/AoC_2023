#include "../data_structs.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 4096
#define LINE_SZ 100

DECL_ARRAY(Data, data, int)

void get_line_values(char *line, Data *values) {

    data_clear(values);

    for (char *str = line, *tok; (tok = strtok(str, " ")) != NULL; str = NULL) {
        int curr = atoi(tok);
        data_append(values, curr);
    }
}

bool all_zeros(Data *values) {

    for (size_t i = 0; i < values->len; i++) {
        int curr = values->items[i];
        if (curr)
            return false;
    }

    return true;
}

int get_next_number(Data *values, Data *tails) {

    data_clear(tails);

    while (!all_zeros(values)) {
        int prev;
        bool first = true;
        for (size_t i = 0; i < values->len; i++) {
            int curr = values->items[i];
            if (!first) {
                values->items[i - 1] = curr - prev;
            }

            first = false;
            prev = curr;
        }
        values->len--;
        data_append(tails, prev);
    }

    int estimate = 0;
    for (size_t i = 0; i < tails->len; i++) {
        estimate += tails->items[i];
    }

    return estimate;
}

int main() {

    char *line = malloc(sizeof(char) * BUFF_SIZE);
    if (line == NULL)
        return -1;

    Data line_values = {0};
    Data tail_values = {0};

    if (data_init(&line_values, LINE_SZ))
        return 1;

    if (data_init(&tail_values, LINE_SZ))
        return 1;

    int total = 0;
    while (fgets(line, BUFF_SIZE, stdin) != NULL) {
        get_line_values(line, &line_values);
        int next = get_next_number(&line_values, &tail_values);
        total += next;
    }
    printf("%d\n", total);

    data_free(&line_values);
    data_free(&tail_values);
    free(line);

    return 0;
}
