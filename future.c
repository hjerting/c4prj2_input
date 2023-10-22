#include <errno.h>
#include <stdio.h>
#include "cards.h"
#include "deck.h"
#include "future.h"

void print_future_cards(future_cards_t *fc)
{
    size_t last;
    printf("------------------------\n");
    for (int i = 0; i < fc->n_decks; ++i)
    {
        if (1)//(fc->decks[i].n_cards > 0)
        {
            printf("index: %d [", i);
            last = fc->decks[i].n_cards - 1;
            for (int j = 0; j < fc->decks[i].n_cards; ++j)
            {
                print_card(*fc->decks[i].cards[j]);
                if (j < last)
                {
                    printf(", ");
                }
            }
            printf("]\n");
        }
    }
    printf("------------------------\n");
}

void add_future_card(future_cards_t * fc, size_t index, card_t * ptr)
{
    deck_t *new_decks = NULL;
    deck_t *new_deck = NULL;
    while (index >= fc->n_decks)
    {
        new_deck = initialize_deck();
        if (new_deck == NULL)
        {
            fprintf(stderr, "Memory failed to add deck to future cards. Error %d\n", errno);
            return;
        }
        new_decks = realloc(fc->decks, sizeof(*fc->decks) * (fc->n_decks + 1));
        if (new_decks == NULL)
        {
            fprintf(stderr, "Failed to allocate more memory to future card deck. Error: %d\n", errno);
            return;
        }
        fc->decks = new_decks;
        fc->decks[fc->n_decks] = *new_deck;
        ++fc->n_decks;
    }
    add_card_pointer_to_deck(&fc->decks[index], ptr);
}

void future_cards_from_deck(deck_t * deck, future_cards_t * fc)
{
    int index = 0;
    
    for (size_t i = 0; i < fc->n_decks; ++i)
    {
        for (size_t j = 0; j < fc->decks[i].n_cards; ++j)
        {
            fc->decks[i].cards[j]->suit = deck->cards[index]->suit;
            fc->decks[i].cards[j]->value = deck->cards[index]->value;
        }
        ++index;
    }
}

future_cards_t *init_future_cards(void)
{
    future_cards_t *fc = malloc(sizeof(*fc));
    if (fc == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for unknown cards. Error: %d\n", errno);
        return NULL;
    }
    fc->decks = NULL;
    fc->n_decks = 0;
    return fc;
}

void free_future_cards(future_cards_t *fc)
{
    if (fc == NULL) return;
    for (int i = 0; i < fc->n_decks; ++i)
    {
        if (fc->decks[i].cards != NULL)
        {
            free(fc->decks[i].cards);
        }
    }
    if (fc->decks != NULL)
    {
        free(fc->decks);
    }
    free(fc);
}
