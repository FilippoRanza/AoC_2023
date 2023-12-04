#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 1 << 17

typedef struct {
    char *buff;
    int cols;
    int rows;
} Matrix;

char matrix_get(Matrix *mat, int c, int r) {
    int idx = (r * mat->cols) + c;
    return mat->buff[idx];
}

int read_file(FILE *file, Matrix *mat, int size) {
    int cols = 0;
    int rows = 1;
    char *buff = mat->buff;
    for (char c = fgetc(file); --size && c != EOF; c = fgetc(file)) {
        if (c != '\n') {
            *buff++ = c;
            cols++;
        } else {
            rows++;
            mat->cols = cols;
            cols = 0;
        }
    }
    *buff = '\0';
    mat->rows = rows;

    return (size == 0) ? 1 : 0;
}

bool has_adjacent_symbol(Matrix *mat, int c, int r) {
    for (int i = -1; i <= 1; i++) {
        int a = c + i;
        if (a >= 0 && a < mat->cols) {
            for (int j = -1; j <= 1; j++) {
                int b = r + j;
                if (b >= 0 && b < mat->rows) {
                    char c = matrix_get(mat, a, b);
                    if (c != 0 && !isdigit(c) && c != '.') {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main() {

    char *buffer = calloc(BUFF_SIZE, sizeof(char));
    if (buffer == NULL)
        return -1;

    Matrix mat = {0};
    mat.buff = buffer;
    if (read_file(stdin, &mat, BUFF_SIZE))
        return -1;

    int total = 0;
    int curr = 0;
    bool adj = false;
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            char c = matrix_get(&mat, j, i);
            if (isdigit(c)) {
                curr = (curr * 10) + (c - '0');
                if (!adj)
                    adj = has_adjacent_symbol(&mat, j, i);
            } else {
                if (adj) {
                    total += curr;
                }
                curr = 0;
                adj = false;
            }
        }
    }
    if (adj)
        total += curr;

    printf("%d\n", total);

    free(buffer);

    return 0;
}
