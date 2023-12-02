#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 4096

typedef struct {
    int red;
    int green;
    int blue;
} Handful;

typedef enum {
    Game = 0,
    GameId,
    Digit,
    Color,
} State;

int max(int a, int b) { return (a > b) ? a : b; }

int parse_game(char *line) {
    Handful part = {0};

    State state = Game;
    int curr = 0;
    for (char *str = line, *tok = NULL; (tok = strtok(str, ": ,")) != NULL;
         str = NULL) {

        switch (state) {
        case Game:
            if (strcmp(tok, "Game") == 0) {
                state = GameId;
            } else {
                return -1;
            }
            break;
        case GameId:
            state = Digit;
            break;
        case Digit:
            curr = atoi(tok);
            state = Color;
            break;
        case Color:
            if (*tok == 'r') {
                part.red = max(part.red, curr);
            } else if (*tok == 'b') {
                part.blue = max(part.blue, curr);
            } else {
                part.green = max(part.green, curr);
            }
            state = Digit;
            break;
        }
    }

    int output = part.red * part.green * part.blue;

    return output;
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
