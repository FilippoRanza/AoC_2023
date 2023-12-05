#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 4096

int parse_card_points(char *line) {

    bool winning[100] = {0};
    int skip = 2;
    bool parse_winning = true;
    int score = 0;

    for (char *str = line, *tok = NULL; (tok = strtok(str, " ")) != NULL;
         str = NULL) {
        if (skip == 0) {
            if (strcmp(tok, "|") == 0) {
                parse_winning = false;
            } else {
                int n = atoi(tok);
                if (parse_winning) {
                    winning[n] = true;
                } else {
                    if (winning[n]) {
                        score++;
                    }
                }
            }

        } else {
            skip--;
        }
    }

    return score;
}

int main() {

    char *line = calloc(BUFF_SIZE, sizeof(char));
    if (line == NULL)
        return -1;

    int *scores = calloc(BUFF_SIZE, sizeof(int));
    if (scores == NULL)
        return -1;

    int idx = 0;
    while (fgets(line, BUFF_SIZE, stdin) != NULL) {
        scores[idx] += 1;
        int curr = parse_card_points(line);
        for (int i = idx + 1; curr; i++, curr--) {
            scores[i] += scores[idx];
        }

        idx++;
    }

    int points = 0;
    for (int i = 0; i <= idx; i++) {
        int curr = scores[i];
        points += curr;
    }
    printf("%d\n", points);

    free(scores);
    free(line);

    return 0;
}
