#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_LEN 4096
#define N_DIGITS 9

typedef struct {
    char *str;
    size_t len;
    int value;
} DigitInfo;

int match_number(char *buff, DigitInfo *infos) {
    for (int i = 0; i < N_DIGITS; i++) {
        if (infos->len <= strlen(buff)) {
            int res = strncmp(infos->str, buff, infos->len);
            if (res == 0)
                return i;
        }

        infos++;
    }

    return -1;
}

void set_digits(int *first, int *second, int a) {
    if (*first == -1) {
        *first = *second = a;
    } else {
        *second = a;
    }
}

int main() {
    char *numbers[] = {"one", "two",   "three", "four", "five",
                       "six", "seven", "eight", "nine"};

    DigitInfo infos[N_DIGITS];
    for (int i = 0; i < N_DIGITS; i++) {
        infos[i].str = numbers[i];
        infos[i].len = strlen(numbers[i]);
        infos[i].value = i + 1;
    }

    char *line = calloc(BUFF_LEN, sizeof(char));
    if (line == NULL)
        return -1;

    int total = 0;
    while (fgets(line, BUFF_LEN, stdin) != NULL) {
        int first = -1;
        int second = -1;

        char *tmp = line;
        while (*tmp) {
            if (isdigit(*tmp)) {
                set_digits(&first, &second, *tmp - '0');
            } else {
                int match = match_number(tmp, infos);
                if (match != -1) {
                    set_digits(&first, &second, infos[match].value);
                } 
            }
            tmp++;
        }
    

        int n = first * 10 + second;
        total += n;
    }

    printf("%d\n", total);

    free(line);

    return 0;
}
