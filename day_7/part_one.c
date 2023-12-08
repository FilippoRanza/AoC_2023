#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "../data_structs.h"

#define CARDS 5
#define BUFF_LEN 128
#define N_CARDS 15

typedef struct {
    char cards[CARDS];
    int point;
    int bid;
} Hand;

DECL_ARRAY(Hands, hands, Hand)

char card_mapper(char c) {
    switch (c) {
    case 'A':
        c = 14;
        break;
    case 'K':
        c = 13;
        break;
    case 'Q':
        c = 12;
        break;
    case 'J':
        c = 11;
        break;
    case 'T':
        c = 10;
        break;
    default:
        c -= '0';
        break;
    }
    return c;
}

Hand parse_hand(char *line) {
    Hand hand = {0};
    char c;
    char *card = hand.cards;
    while ((c = *line++) && !isspace(c)) {
        *card++ = card_mapper(c);
    }
    hand.bid = atoi(line);
    return hand;
}

int is_n_of_a_kind(char *count, int n) {
    for (int i = 0; i < N_CARDS; i++)
        if (count[i] == n)
            return 1;

    return 0;
}

int is_five_of_a_kind(char *count) { return is_n_of_a_kind(count, 5); }

int is_four_of_a_kind(char *count) { return is_n_of_a_kind(count, 4); }

int is_three_of_a_kind(char *count) { return is_n_of_a_kind(count, 3); }

int is_full_house(char *count) {
    return is_n_of_a_kind(count, 3) && is_n_of_a_kind(count, 2);
}

int count_pairs(char *count) {
    int pairs = 0;
    for (int i = 0; i < N_CARDS; i++)
        if (count[i] == 2)
            pairs++;

    return pairs;
}

int is_two_pairs(char *count) { return count_pairs(count) == 2; }

int is_one_pair(char *count) { return count_pairs(count) == 1; }

void hand_compute_points(Hand *h) {
    char count[N_CARDS] = {0};

    for (int i = 0; i < CARDS; i++) {
        count[h->cards[i]]++;
    }

    if (is_five_of_a_kind(count)) {
        h->point = 6;
    } else if (is_four_of_a_kind(count)) {
        h->point = 5;
    } else if (is_full_house(count)) {
        h->point = 4;
    } else if (is_three_of_a_kind(count)) {
        h->point = 3;
    } else if (is_two_pairs(count)) {
        h->point = 2;
    } else if (is_one_pair(count)) {
        h->point = 1;
    } else {
        h->point = 0;
    }
}

int load_hands(Hands *hands, FILE *file) {
    char line[BUFF_LEN];

    while (fgets(line, BUFF_LEN, file) != NULL) {
        Hand hand = parse_hand(line);
        hand_compute_points(&hand);
        if (hands_append(hands, hand))
            return 1;
    }

    return 0;
}
int hand_find_largest(const Hand *h1, const Hand *h2) {
    for (int i = 0; i < CARDS; i++) {
        int diff = h1->cards[i] - h2->cards[i];
        if (diff != 0)
            return diff;
    }
    return 0;
}

int hand_compare(const void *a, const void *b) {

    const Hand *h_a = a;
    const Hand *h_b = b;

    int pd = h_a->point - h_b->point;
    if (pd == 0)
        pd = hand_find_largest(h_a, h_b);

    return pd;
}


int main() {

    Hands hands = {0};
    if (hands_init(&hands, 100))
        abort();

    if (load_hands(&hands, stdin))
        abort();

    qsort(hands.items, hands.len, sizeof(Hand), hand_compare);


    long points = 0;
    for(size_t i = 0; i < hands.len; i++) {
        points += (i + 1) * hands.items[i].bid;
    }
    printf("%ld\n", points);


    hands_free(&hands);

    return 0;
}
