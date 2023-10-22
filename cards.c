#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "cards.h"

int is_card_valid(card_t card)
{
	return (card.value >= 2 && card.value <= VALUE_ACE) && (card.suit >= 0 && card.suit < NUM_SUITS);
}

void assert_card_valid(card_t c)
{
	is_card_valid(c);
}

const char * ranking_to_string(hand_ranking_t r) {
	switch (r)
	{
		case STRAIGHT_FLUSH:
			return "STRAIGHT_FLUSH";
		case FOUR_OF_A_KIND:
			return "FOUR_OF_A_KIND";
		case FULL_HOUSE:
			return "FULL_HOUSE";
		case FLUSH:
			return "FLUSH";
		case STRAIGHT:
			return "STRAIGHT";
		case THREE_OF_A_KIND:
			return "THREE_OF_A_KIND";
		case TWO_PAIR:
			return "TWO_PAIR";
		case PAIR:
			return "PAIR";
		case NOTHING:
			return "NOTHING";
	}
	return "Error, invalid rank";
}

char value_letter(card_t c) {
	switch (c.value)
	{
		case VALUE_ACE:
			return 'A';
		case VALUE_KING:
			return 'K';
		case VALUE_QUEEN:
			return 'Q';
		case VALUE_JACK:
			return 'J';
		case 10:
			return '0';
		default:
			if (c.value >= 2 && c.value <= 9)
				return c.value + '0';
	}
	return '~';
}

char suit_letter(card_t c) {
	switch (c.suit)
	{
		case SPADES:
			return 's';
		case HEARTS:
			return 'h';
		case DIAMONDS:
			return 'd';
		case CLUBS:
			return 'c';
		default:
			return '~';
	}
	return '~';
}

void print_card(card_t c) {
	printf("%c%c", value_letter(c), suit_letter(c));
}

int value_to_int(char letter)
{
	switch (letter)
	{
		case 'A':
			return VALUE_ACE;
		case 'K':
			return VALUE_KING;
		case 'Q':
			return VALUE_QUEEN;
		case 'J':
			return VALUE_JACK;
		case '0':
			return 10;
		default:
			if (letter >= '2' && letter <= '9')
				return letter - '0';
	}
	return '\0';
}

int suit_to_int(char letter)
{
	switch (letter)
	{
		case 's':
			return SPADES;
		case 'h':
			return HEARTS;
		case 'd':
			return DIAMONDS;
		case 'c':
			return CLUBS;
	}
	return 1000;
}

card_t card_from_letters(char value_let, char suit_let) {
  card_t temp;
  temp.value = value_to_int(value_let);
  temp.suit = suit_to_int(suit_let);
  assert_card_valid(temp);
  return temp;
}

card_t card_from_num(unsigned c) {
  card_t temp;
  temp.value = c % 13 + 2;
  temp.suit = c / 13;
  assert_card_valid(temp);
  return temp;
}
