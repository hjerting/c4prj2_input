#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "cards.h"
#include "deck.h"
#include "future.h"

#define CHAR_LIMIT 4
#define LAST CHAR_LIMIT - 1

int is_white_space(char c)
{
    return((c >= 9 && c <= 13) || c == 32 || c == 133 || c == 160);
}

char *trim_hand(const char *str)
{
    size_t start = 0;
    while (is_white_space(str[start]))
    {
        ++start;
    }
    if (str[start] == '\0')
    {
        return NULL;
    }
    size_t end = strlen(str) - 1;
    while (is_white_space(str[end]))
    {
        --end;
    }
    char *trimmed = malloc(sizeof(*trimmed) * (end - start + 2));
    size_t i = 0;
    while (start <= end)
    {
        trimmed[i++] = str[start++];
    }
    trimmed[i] = '\0';
    return trimmed;
}

char *card_from_string(const char *str, size_t *index, char *c, size_t length)
{
    while (is_white_space(str[*index]))
    {
        ++*index;
    }
    if (str[*index] == '\0') return NULL;
    size_t i = 0;
    while (i < LAST && (!is_white_space(str[*index]) && str[*index] != '\0'))
    {
        c[i++] = str[*index++];
    }
    c[i] = '\0';
    return c;
}

deck_t * hand_from_string(const char * str, future_cards_t * fc)
{
    deck_t *hand = initialize_deck();
    card_t card;
    card_t *card_p;
    char c[CHAR_LIMIT];

    size_t start = 0;
    size_t end;
    size_t length = strlen(str);
    size_t i;
    int index;

    while (start < length)
    {
        if (is_white_space(str[start]))
        {
            ++start;
        }
        end = start;
        while (!is_white_space(str[end]) && str[end] != '\0')
        {
            ++end;
        }
        if (start < end)
        {
            i = 0;
            while (start < end && i < LAST)
            {
                c[i++] = str[start++];
            }
            c[i] = '\0';
        }
        if (c[0] == '?')
        {
            // This is a future card
            index = atoi(c + 1);
            if (index >= DECK_SIZE)
            {
                fprintf(stderr, "Invalid card index.\n");
                continue;
            }
            card_p = add_empty_card(hand);
            if (card_p == NULL)
            {
                fprintf(stderr, "Failed to add future card to hand.\n");
                continue;
            }
            add_future_card(fc, index, card_p);
        }
        else
        {
            card = card_from_letters(c[0], c[1]);
            if (!is_card_valid(card))
            {
                fprintf(stderr, "Invalid card.\n");
                continue;
            }
            add_card_to(hand, card);
        }
    }
    return hand;
}

deck_t ** read_input(FILE * f, size_t * n_hands, future_cards_t * fc)
/*
   This function reads the input from f. The input file has one hand per line 
   (a hand is of type deck_t). A deck_t type deck is allocated for each hand
   and it is placed into an array of pointers to deck_t decks, which is 
   returned. The number of decks is passed back trough the n_hands pointer.
   
   For any future future cards (?0, ?1, ...) in the deck, the add_empty_card is 
   used to create a placeholder in the hand. The add_future_card function is 
   used later to make sure the hand is updated correctly when cards are drawn 
   later. 
   
   The code assumes that a poker hand has AT LEAST 5 cards in it. If there are 
   fewer than 5 cards, a useful error message is printed before exit.
*/
{
    deck_t **hands = NULL;
    deck_t **new_hands = NULL;
    deck_t *new_hand = NULL;

    char *line = NULL;
    char *trimmed = NULL;
    size_t size = 0;
    ssize_t length = 0;

    while ((length = getline(&line, &size, f)) > 0)
    {
        trimmed = trim_hand(line);
        if (trimmed == NULL) continue;
        new_hand = hand_from_string(trimmed, fc);
        if (new_hand->n_cards < 5)
        {
            fprintf(stderr, "Not enough cards in hand.\n");
            free_deck(new_hand);
            for (size_t i = 0; i < *n_hands; ++i)
            {
                free_deck(hands[i]);
            }
            free(hands);
            return NULL;
        }
        new_hands = realloc(hands, sizeof(*hands) * (*n_hands + 1));
        if (new_hands == NULL)
        {
            fprintf(stderr, "Failed to allocate memorfy for hand. Error: %d\n", errno);
            free_deck(new_hand);
            free_decks(hands, *n_hands);
            *n_hands = 0;
            return NULL;
        }
        hands = new_hands;
        hands[*n_hands] = new_hand;
        ++*n_hands;
        free(trimmed);
    }
    if (line != NULL)
    {
        free(line);
    }
    return hands;
}
