#include "../data_structs.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 1 << 17

typedef struct {
    int first;
    int second;
} Couple;

DEF_MATRIX(CoupMat, coup_mat, Couple)

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
            if (!isdigit(c) && c != '*')
                c = '.';
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

bool has_adjacent_symbol(Couple *couple, Matrix *mat, int col, int row) {
    for (int i = -1; i <= 1; i++) {
        int a = col + i;
        if (a >= 0 && a < mat->cols) {
            for (int j = -1; j <= 1; j++) {
                int b = row + j;
                if (b >= 0 && b < mat->rows) {
                    char c = matrix_get(mat, a, b);
                    if (c != 0 && !isdigit(c) && c == '*') {
                        couple->first = a;
                        couple->second = b;
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

    CoupMat cmat = {0};
    if (coup_mat_new(&cmat, mat.cols, mat.rows)) {
        return -1;
    }

    int curr = 0;
    bool adj = false;
    Couple couple = {0};
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            char c = matrix_get(&mat, j, i);
            if (isdigit(c)) {
                curr = (curr * 10) + (c - '0');
                if (!adj)
                    adj = has_adjacent_symbol(&couple, &mat, j, i);
            } else {
                if (adj) {
                    int j = couple.first;
                    int i = couple.second;
                    Couple tmp = coup_mat_get(&cmat, i, j);
                    if (tmp.first)
                        tmp.second = curr;
                    else
                        tmp.first = curr;

                    coup_mat_set(&cmat, i, j, tmp);
                }

                curr = 0;
                adj = false;
            }
        }
    }
    int total = 0;
    size_t len = cmat.cols * cmat.rows;
    Couple *buff = cmat.items;
    while (len--) {
        int prod = buff->first * buff->second;
        total += prod;
        buff++;
    }

    printf("%d\n", total);

    coup_mat_free(&cmat);
    free(buffer);

    return 0;
}
