#include "../data_structs.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_all(FILE *file) {

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buff = malloc((size + 1) * sizeof(char));
    if (buff == NULL)
        return NULL;

    char *tmp = buff;
    int c;
    while ((c = fgetc(file)) != EOF)
        *tmp++ = (char)c;
    *tmp = '\0';

    return buff;
}

typedef struct {
    char **lines;
    char *buff;
    size_t rows;
    size_t cols;
} InMat;

int count_lines(const char *restrict buff) {
    int count = 0;
    char c;
    while ((c = *buff++) != '\0') {
        if (c == '\n')
            count++;
    }

    return count;
}

int build_input_mat(InMat *mat, char *buff) {
    int n_lines = count_lines(buff);
    char **lines = malloc((n_lines + 1) * sizeof(char *));
    if (lines == NULL)
        return -1;

    mat->buff = buff;
    mat->rows = n_lines + 1;
    mat->lines = lines;

    while (n_lines--) {
        *lines++ = buff;
        mat->cols = 0;
        while (*buff != '\n') {
            buff++;
            mat->cols++;
        }
        *buff++ = '\0';
    }
    *lines = buff;

    return 0;
}

typedef struct {
    int i;
    int j;
} Coord;

Coord find_initial_point(InMat *mat) {
    for (size_t i = 0; i < mat->rows; i++) {
        char *row = mat->lines[i];
        for (size_t j = 0; j < mat->cols; j++) {
            if (*row++ == 'S') {
                Coord coord = {.i = i, .j = j};
                return coord;
            }
        }
    }

    Coord coord = {.i = -1, .j = -1};
    return coord;
}

char inmat_char_at(InMat *mat, int row, int col) {
    return mat->lines[row][col];
}

char inmat_at_is_one_of(InMat *mat, int row, int col, char *buff) {
    if (row < 0 || col < 0)
        return '\0';

    if (row >= mat->rows || col >= mat->cols)
        return '\0';

    char ch = inmat_char_at(mat, row, col);
    for (char tmp = *buff++; tmp != '\0'; tmp = *buff++) {
        if (tmp == ch)
            return ch;
    }

    return '\0';
}

char convert_initial_point(InMat *mat, Coord *coord) {
    char up = inmat_at_is_one_of(mat, coord->i - 1, coord->j, "|F7");
    char down = inmat_at_is_one_of(mat, coord->i + 1, coord->j, "|JL");
    char left = inmat_at_is_one_of(mat, coord->i, coord->j - 1, "-LF");
    char right = inmat_at_is_one_of(mat, coord->i, coord->j + 1, "-J7");

    char ch = '\0';
    if (up && down)
        ch = '|';
    else if (up && left)
        ch = 'J';
    else if (up && right)
        ch = 'L';
    else if (left && right)
        ch = '-';
    else if (left && down)
        ch = '7';
    else if (right && down)
        ch = 'F';

    mat->lines[coord->i][coord->j] = ch;

    return ch;
}

typedef struct {
    Coord coord;
    int dist;
} Element;

typedef struct {
    Element *heap;
    size_t len;
    // size_t head;
    // size_t tail;
    size_t capacity;
} Queue;

void queue_init(Queue *queue, size_t capacity) {

    Element *heap = malloc(sizeof(Element) * capacity);
    if (heap == NULL)
        abort();

    queue->heap = heap;
    queue->capacity = capacity;
    queue->len = 0;
    // queue->head = queue->tail = 0;
}

void queue_swap_items(Queue *queue, size_t curr, size_t parent) {
    Element e = queue->heap[curr];
    queue->heap[curr] = queue->heap[parent];
    queue->heap[parent] = e;
}

Element queue_pop(Queue *queue) {

    size_t curr = 0;
    Element output = queue->heap[curr];
    queue->len--;
    if (queue->len == 0)
        return output;

    queue_swap_items(queue, curr, queue->len);
    queue->heap[queue->len].dist = -1;

    bool run = true;
    while (run) {
        size_t left = (2 * curr) + 1;
        size_t right = (2 * curr) + 2;

        if (left < queue->len &&
            queue->heap[curr].dist > queue->heap[left].dist) {
            queue_swap_items(queue, curr, left);
            curr = left;
        } else if (right < queue->len &&
                   queue->heap[curr].dist > queue->heap[right].dist) {
            queue_swap_items(queue, curr, right);
            curr = right;
        } else {
            run = false;
        }
    }

    return output;
}

void queue_push(Queue *queue, Element e) {

    size_t curr = queue->len;
    queue->heap[curr] = e;
    size_t parent = curr / 2;

    while (queue->heap[curr].dist < queue->heap[parent].dist) {
        queue_swap_items(queue, curr, parent);
        curr = parent;
        parent = curr / 2;
    }

    queue->len++;
    if (queue->len == queue->capacity) {
        queue->capacity *= 2;
        queue->heap = realloc(queue->heap, sizeof(Element) * queue->capacity);
        if (queue->heap == NULL)
            abort();
    }
}

int queue_is_empty(Queue *queue) { return queue->len == 0; }

void queue_free(Queue *queue) { free(queue->heap); }

bool is_valid(Coord *c, InMat *mat) {

    if (c->i < 0 || c->i >= mat->rows)
        return false;

    if (c->j < 0 || c->j >= mat->cols)
        return false;

    return true;
}

DEF_MATRIX(CostMat, costmat, int)

bool should_update(Element *elem, CostMat *cmat) {
    int curr = costmat_get(cmat, elem->coord.j, elem->coord.i);
    return curr > elem->dist;
}

void insert_next(char type, Element curr, Queue *queue, InMat *mat,
                 CostMat *cmat) {
    Coord c1 = curr.coord;
    Coord c2 = curr.coord;

    switch (type) {
    case '|':
        c1.i--;
        c2.i++;
        break;
    case '-':
        c1.j--;
        c2.j++;
        break;
    case 'L':
        c1.i--;
        c2.j++;
        break;
    case 'J':
        c1.i--;
        c2.j--;
        break;
    case '7':
        c1.i++;
        c2.j--;
        break;
    case 'F':
        c1.i++;
        c2.j++;
        break;
    }

    if (is_valid(&c1, mat)) {
        Element new = {.coord = c1, .dist = curr.dist + 1};
        if (should_update(&new, cmat))
            queue_push(queue, new);
    }
    if (is_valid(&c2, mat)) {
        Element new = {.coord = c2, .dist = curr.dist + 1};
        if (should_update(&new, cmat))
            queue_push(queue, new);
    }
}

void update_cost_mat(CostMat *cmat, Element *el) {
    int cost = el->dist;
    costmat_set(cmat, el->coord.j, el->coord.i, cost);
}

int find_farthest_point(InMat *mat, Coord coord) {

    Queue queue = {0};
    queue_init(&queue, mat->cols * mat->rows);

    CostMat cmat = {0};
    costmat_new(&cmat, mat->cols, mat->rows);
    for (size_t i = 0; i < cmat.rows; i++) {
        for (size_t j = 0; j < cmat.cols; j++) {
            costmat_set(&cmat, j, i, INT_MAX);
        }
    }

    Element root = {.coord = coord, .dist = 0};
    queue_push(&queue, root);

    while (!queue_is_empty(&queue)) {
        Element curr_elem = queue_pop(&queue);
        update_cost_mat(&cmat, &curr_elem);
        char curr_type =
            inmat_char_at(mat, curr_elem.coord.i, curr_elem.coord.j);

        insert_next(curr_type, curr_elem, &queue, mat, &cmat);
    }

    size_t limit = cmat.cols * cmat.rows;
    int output = 0;
    for (size_t i = 0; i < limit; i++) {
        if (cmat.items[i] < INT_MAX) {
            if (output < cmat.items[i]) {
                output = cmat.items[i];
            }
        }
    }

    costmat_free(&cmat);
    queue_free(&queue);

    return output;
}

void inmat_free(InMat *mat) {
    free(mat->buff);
    free(mat->lines);
}

int main() {

    char *data = read_all(stdin);
    if (data == NULL)
        return -1;

    InMat mat = {0};
    if (build_input_mat(&mat, data)) {
        return -1;
    }

    Coord coord = find_initial_point(&mat);
    printf("Initial pos: %d %d\n", coord.i, coord.j);
    char subs = convert_initial_point(&mat, &coord);
    printf("Subst: %c\n", subs);

    int max_dist = find_farthest_point(&mat, coord);
    printf("Max dist: %d\n", max_dist);

    inmat_free(&mat);
    return 0;
}
