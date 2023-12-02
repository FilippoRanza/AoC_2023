#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFF_LEN 4096

int main() {

    char *line = calloc(BUFF_LEN, sizeof(char));
    if (line == NULL)
        return -1;

    char *ptr;
    int total = 0;
    while ((ptr = fgets(line, BUFF_LEN, stdin)) && *ptr != EOF) {
        char first = -1;
        char second = -1;

        char curr;
        char *tmp = line;
        while ((curr = *tmp++) != '\0') {
            if (isdigit(curr)) {
                if (first == -1) {
                    first = second = curr;
                } else {
                    second = curr;
                }
            }
        }

        int n = (first - '0') * 10;
        n += second - '0';

        total += n;
    }

    printf("%d\n", total);

    free(line);

    return 0;
}
