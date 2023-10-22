#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "cards.h"
#include "deck.h"
#include "eval.h"
#include "future.h"
#include "input.h"

int frees = 0;

void print_int_array(unsigned *arr, size_t size)
{
    printf("size: %ld\n", size);
    ssize_t last = size - 1;
    printf("[");
    for (size_t i = 0; i < size; ++i)
    {
        printf("%d", arr[i]);
        if (i < last)
        {
            printf(", ");
        }
    }
    printf("]\n");
}

void output_hand(deck_t *hand)
{
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < hand->n_cards; ++i)
    {
        print_card(*hand->cards[i]);
        printf(" ");
    }
    printf("\n---------------------------------------------------------\n");
}

void add_deck_to_deck(deck_t *deck, deck_t *new_deck)
{
    deck_t *combined_deck = realloc(deck, sizeof(*deck) * (deck->n_cards + new_deck->n_cards));
    if (combined_deck == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for larger deck. Error: %d\n", errno);
        return;
    }
    deck = combined_deck;
    for (int i = 0; i < new_deck->n_cards; ++i)
    {
        deck->cards[deck->n_cards + i] = new_deck->cards[i];
        ++deck->n_cards;
    }
}

int main(void)
{
    char *filename = "test1.txt";
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Failed to open file '%s'. Error: %d\n", filename, errno);
        return EXIT_FAILURE;
    }

    future_cards_t *fc = malloc(sizeof(*fc));
    if (fc == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for future cards. Error: %d\n", errno);
        fclose(f);
        return EXIT_FAILURE;
    }
    fc->decks = NULL;
    fc->n_decks = 0;

    size_t n_hands = 0;
    deck_t **hands = read_input(f, &n_hands, fc);

    if (hands == NULL)
    {
        printf("Hands == NULL\n");
    }
    else {
        deck_t *new_deck = generate_new_deck();
        future_cards_from_deck(new_deck, fc);
        free_deck(new_deck);
        for (int i = 0; i < n_hands; ++i)
        {
            print_hand(hands[i]);
            printf("\n");
        }
    }
    
    fclose(f);
    free_future_cards(fc);
    free_decks(hands, n_hands);
}