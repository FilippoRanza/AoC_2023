#include <math.h>
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

DECL_ARRAY(Races, races, Race)

int parse_input(Races *races, FILE *input) {

    char *line = calloc(BUFF_SIZE, sizeof(char));
    if (line == NULL)
        return -1;

    int line_idx = 0;
    while (fgets(line, BUFF_SIZE, input) != NULL) {
        bool first = true;
        int idx = 0;
        for (char *str = line, *tok; (tok = strtok(str, " ")) != NULL;
             str = NULL) {
            if (!first) {
                if (line_idx == 0) {
                    Race race = {.time = atoi(tok)};
                    races_append(races, race);
                } else {
                    races->items[idx++].dist = atoi(tok);
                }
            } else {
                first = false;
            }
        }
        line_idx++;
    }

    free(line);
    return 0;
}

typedef struct {
    double x1;
    double x2;
} Sol;

double mfloor(double n) {
    double tmp = floor(n);
    if (tmp == n)
        tmp -= 1;
    return tmp;
}


double mceil(double n) {
    double tmp = ceil(n);
    if (tmp == n)
        tmp += 1;
    return tmp;
}

Sol solve_equation(double a, double b, double c) {
    Sol sol;
    double delta = sqrt((b * b) - (4 * a * c));
    sol.x1 = (-delta - b) / (2 * a);
    sol.x2 = (delta - b) / (2 * a);
    return sol;
}

int main() {

    Races races = {0};
    if (races_init(&races, 3))
        return -1;

    if (parse_input(&races, stdin))
        return -1;

    int prod = 1;
    for (size_t i = 0; i < races.len; i++) {
        Sol sol = solve_equation(1, -races.items[i].time, races.items[i].dist);
        double a = mceil(sol.x1);
        double b = mfloor(sol.x2);
        double dist = b - a + 1;
        prod = prod * (int)dist;
    }
    printf("%d\n", prod);

    races_free(&races);
    return 0;
}
