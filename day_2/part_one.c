#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 4096

typedef struct {
    int red;
    int green;
    int blue;
} Handful;

bool is_valid(Handful *part, Handful *set) {
    return part->red <= set->red && part->green <= set->green &&
           part->blue <= set->blue;
}

typedef enum {
    Game = 0,
    GameId,
    Digit,
    Color,
} State;

int parse_game(char *line) {
    int game_id = 0;
    Handful part = {0};
    Handful set = {.red = 12, .green = 13, .blue = 14};

    State state = Game;
    int curr = 0;
    bool colors = false;
    for (char *str = line, *tok = NULL; (tok = strtok(str, ": ,")) != NULL;
         str = NULL) {

        int last;
        switch (state) {
        case Game:
            if (strcmp(tok, "Game") == 0) {
                state = GameId;
            } else {
                return -1;
            }
            break;
        case GameId:
            game_id = atoi(tok);
            state = Digit;
            break;
        case Digit:
            curr = atoi(tok);
            state = Color;
            break;
        case Color:
            last = strlen(tok) - 1;
            if (tok[last] == ';')
                colors = true;

            if (*tok == 'r') {
                part.red = curr;
            } else if (*tok == 'b') {
                part.blue = curr;
            } else {
                part.green = curr;
            }
            state = Digit;
            break;
        }

        if (colors) {
            if (!is_valid(&part, &set))
                return 0;
            part.red = part.green = part.blue;

            colors = false;
        }
    }

    if (!is_valid(&part, &set))
        return 0;

    return game_id;
}

int main() {

    char *line = calloc(BUFF_LEN, sizeof(char));
    if (line == NULL)
        return -1;

    int total = 0;
    while (fgets(line, BUFF_LEN, stdin) != NULL) {
        int curr = parse_game(line);
        total += curr;
    }

    printf("%d\n", total);

    free(line);
    return 0;
}
