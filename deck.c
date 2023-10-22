#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "deck.h"

int cards_equal(card_t card1, card_t card2)
{
  return card1.value == card2.value && card1.suit == card2.suit;
}

void print_hand(deck_t * hand){
  size_t n_cards = hand->n_cards;
  size_t last = n_cards - 1;
  card_t **cards = hand->cards;
  for (int i = 0; i < n_cards; ++i)
  {
    print_card(*cards[i]);
    if (i < last) printf(" ");
  }
}

int deck_contains(deck_t * d, card_t c)
{
  for (int i = 0; i < d->n_cards; ++i)
  {
    if (cards_equal(*d->cards[i], c)) return 1;
  }
  return 0;
}

void shuffle(deck_t * d)
{
  size_t n_cards = d->n_cards;
  card_t **cards = d->cards;
  size_t random;
  card_t *temp;
  for (int i = n_cards - 1; i >= 0; --i)
  {
    random = rand() % (i + 1);
    temp = cards[random];
    cards[random] = cards[i];
    cards[i] = temp;
  }
}

void assert_full_deck(deck_t * d)
{
  assert(d->n_cards == DECK_SIZE);
  card_t card;
  for (int i = 0; i < d->n_cards; ++i)
  {
    card = card_from_num(i);
    assert(deck_contains(d, card));
  }
  printf("Full deck asserted!\n");
}

/* New stuff c4 */
void print_deck(deck_t *deck)
{
  printf("\n--------------------------------------\n");
  printf("Deck size: %ld\n", deck->n_cards);
  printf("--------------------------------------\n");
  for (int i = 0; i < deck->n_cards; ++i)
  {
    print_card(*deck->cards[i]);
    if ((i + 1) % 13 == 0)
    {
      printf("\n");
    }
    else
    {
      printf(" ");
    }
  }
  printf("\n--------------------------------------\n");
}

deck_t *initialize_deck()
{
  deck_t *deck = malloc(sizeof(*deck));
  if (deck == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for new deck. Error: %d\n", errno);
    return NULL;
  }
  deck->n_cards = 0;
  deck->cards = NULL;
  return deck;
}

deck_t *generate_new_deck()
{
    deck_t *deck = initialize_deck();
    if (deck == NULL)
    {
      return NULL;
    }
    deck->cards = malloc(sizeof(*deck->cards) * DECK_SIZE);
    if (deck->cards == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for cards. Error: %d\n", errno);
      free(deck);
      return NULL;
    }
    card_t card;
    for (int i = 0; i < DECK_SIZE; ++i)
    {
      card = card_from_num(i);
      deck->cards[i] = malloc(sizeof(*deck->cards[i]));
      if (deck->cards[i] == NULL)
      {
        fprintf(stderr, "Failed to allocate memory for card. Error: %d\n", errno);
        for (int j = 0; j < i; ++j)
        {
          free(deck->cards[j]);
        }
        free(deck->cards);
        free(deck);
        return NULL;
      }
      deck->cards[i]->suit = card.suit;
      deck->cards[i]->value = card.value;
    }
    deck->n_cards = DECK_SIZE;
    return deck;
}

void add_card_pointer_to_deck(deck_t *deck, card_t *p)
{
  card_t **cards = realloc(deck->cards, sizeof(*cards) * (deck->n_cards + 1));
  if (cards == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for larger deck of card pointers. Error: %d\n", errno);
    return;
  }
  deck->cards = cards;
  deck->cards[deck->n_cards] = p;
  ++deck->n_cards;
}

void add_card_to(deck_t * deck, card_t c)
{
/* Add the particular card to the given deck (which will
   involve reallocing the array of cards in that deck).
 */
  card_t *new_card = malloc(sizeof(*new_card));
  if (new_card == NULL)
  {
    fprintf(stderr, "Could not allocate memory for a new card. Error: %d\n", errno);
    return;
  }
  card_t **new_cards = realloc(deck->cards, sizeof(*new_cards) * (deck->n_cards + 1));
  if (new_cards == NULL)
  {
    fprintf(stderr, "Could not allocate memory for larger deck. Error: %d\n", errno);
    free(new_card);
    return;
  }
  new_card->suit = c.suit;
  new_card->value = c.value;
  deck->cards = new_cards;
  deck->cards[deck->n_cards] = new_card;
  ++deck->n_cards;
}

card_t * add_empty_card(deck_t * deck)
{
/* Add a card whose value and suit are both 0, and return a pointer
   to it in the deck.
   This will add an invalid card to use as a placeholder
   for an unknown card.
  */
  card_t *new_card = malloc(sizeof(*new_card));
  if (new_card == NULL)
  {
    fprintf(stderr, "Could not allocate memory for a new card. Error: %d\n", errno);
    return NULL;
  }
  card_t **new_cards = realloc(deck->cards, sizeof(*new_cards) * (deck->n_cards + 1));
  if (new_cards == NULL)
  {
    fprintf(stderr, "Could not allocate memory for larger deck. Error: %d\n", errno);
    free(new_card);
    return NULL;
  }
  new_card->suit = 0;
  new_card->value = 0;
  deck->cards = new_cards;
  deck->cards[deck->n_cards] = new_card;
  ++deck->n_cards;
  return new_card;
}

deck_t * make_deck_exclude(deck_t * excluded_cards)
{
 /*
   Create a deck that is full EXCEPT for all the cards
   that appear in excluded_cards.  For example,
   if excluded_cards has Kh and Qs, you would create
   a deck with 50 cards---all of them except Kh and Qs.
   You will need to use malloc to allocate this deck.
   (You will want this for the next function).
   Don't forget you wrote card_t card_from_num(unsigned c)
   in Course 2 and int deck_contains(deck_t * d, card_t c)
   in Course 3!  They might be useful here.
 */
 deck_t *new_deck = initialize_deck();
 if (new_deck == NULL)
 {
  return NULL;
 }
 card_t card;
 for (int i = 0; i < DECK_SIZE; ++i)
 {
  card = card_from_num(i);
  if (!deck_contains(excluded_cards, card))
  {
    add_card_to(new_deck, card);
  }
 }
 return new_deck;
}


deck_t * build_remaining_deck(deck_t ** hands, size_t n_hands)
{
/* This function takes an array of hands (remember
   that we use deck_t to represent a hand).  It then builds
   the deck of cards that remain after those cards have
   been removed from a full deck.  For example, if we have
   two hands:
      Kh Qs ?0 ?1 ?2 ?3 ?4
      As Ac ?0 ?1 ?2 ?3 ?4
   then this function should build a deck with 48
   cards (all but As Ac Kh Qs).  You can just build
   one deck with all the cards from all the hands
   (remember you just wrote add_card_to),
   and then pass it to make_deck_exclude.
*/
  deck_t *exclude = initialize_deck();
  if (exclude == NULL)
  {
    return NULL;
  }
  for (int i = 0; i < n_hands; ++i)
  {
    for (int j = 0; j < hands[i]->n_cards; ++j)
    {
      if (is_card_valid(*hands[i]->cards[j]))
      {
        print_card(*hands[i]->cards[j]);
        add_card_to(exclude, *hands[i]->cards[j]);
      }
    }
  }
  return make_deck_exclude(exclude);
}

void free_deck(deck_t * deck)
{
/*   Free the memory allocated to a deck of cards.
   For example, if you do
     deck_t * d = make_excluded_deck(something);
     free_deck(d);
   it should free all the memory allocated by make_excluded_deck.
   Once you have written it, add calls to free_deck anywhere you
   need to to avoid memory leaks.
*/
  if (deck == NULL) return;
  for (int i = 0; i < deck->n_cards; ++i)
  {
    free(deck->cards[i]);
  }
  free(deck->cards);
  free(deck);
}

void free_decks(deck_t **decks, size_t n_decks)
{
  if (decks == NULL) return;
  for (size_t i = 0; i < n_decks; ++i)
  {
    free_deck(decks[i]);
  }
  free(decks);
}

void add_hand_to_deck(deck_t *deck, deck_t *hand)
{
  card_t card;
  for (size_t i = 0; i < hand->n_cards; ++i)
  {
    card.suit = hand->cards[i]->suit;
    card.value = hand->cards[i]->value;
    add_card_to(deck, card);
  }
}
