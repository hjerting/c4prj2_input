#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int card_ptr_comp(const void * vp1, const void * vp2)
/*
 * A comparison function to pass to quicksort.
 * Quicksort sorts into ascending order. Descending order required so return
 *      something < 0  if card1 > card2
 *      0              if card1 == card2
 *      something > 0  if card1 < card2
 * If two cards have the same value, compare them by suit in the same order as
 * the enum suit_t:
 *      club < diamond < heart < spade
 */
{
  const card_t * const * cp1 = vp1;
  const card_t * const * cp2 = vp2;
  int difference = (*cp2)->value - (*cp1)->value;
  return difference != 0 ? difference : (*cp2)->suit - (*cp1)->suit;
}

suit_t flush_suit(deck_t * hand) 
/* Function that determines if a hand is a flush (has at least 5 cards of one 
 * suit) exists. If so, it returns the suit of the cards comprising the flush. 
 * If not, it returns NUM_SUITS.
 * 
 * For example:
 * Given Ks Qs 0s 9h 8s 7s, it would return SPADES.  
 * Given Kd Qd 0s 9h 8c 7c, it would return NUM_SUITS.
 */
{
  int card_suits[NUM_SUITS] = { 0 };
  suit_t suit;
  size_t n_cards = hand->n_cards;
  card_t **cards = hand->cards;
  for (size_t i = 0; i < n_cards; ++i)
  {
    suit = cards[i]->suit;
    ++card_suits[suit];
    if (card_suits[suit] > 4)
    {
      return suit;
    }
  }
  return NUM_SUITS;
}

unsigned get_largest_element(unsigned * arr, size_t n)
/* This function returns the largest element in an array of unsigned integers.
 */
{
  if (n < 1) /* Make sure array is not empty */
  {
    printf("Cannot get_largest_element of an array with 0 or less elements!\n");
    exit(1);
  }
  unsigned max = arr[0];
  for (size_t i = 1; i < n; ++i)
  {
    if (arr[i] > max) max = arr[i];
  }
  return max;
}

size_t get_match_index(unsigned * match_counts, size_t n,unsigned n_of_akind)
/* This function returns the index in the array (match_counts) whose value is 
 * n_of_akind. The array has n elements. It returns the LOWEST index whose 
 * value is  n_of_akind if there are more than one.
 */
{
  for (size_t i = 0; i < n; ++i)
  {
    if (match_counts[i] == n_of_akind)
    {
      return i;
    } 
  }
  /* There is guaranteed to be a match with n_of_a_kind, so one should never 
   * get here, so abort. */
  exit(1);
  return 0; /* Required to meet return type. */
}

ssize_t  find_secondary_pair(
      deck_t * hand, unsigned * match_counts, size_t match_idx)
/* When you have a hand with 3 of a kind or a pair, you will want to look and 
 * see if there is another pair to make the hand into a full house or two pairs. 
 * This function takes in the hand, the match counts from before, and the index 
 * where the original match (3 of a kind or pair) was found. It finds the index 
 * of a card meeting the following conditions:
 * - Its match count is > 1 [so there is at least a pair of them]
 * - The card's value is not the same as the value of the card at match_idx 
 *   (so it is not part of the original three of a kind/pair)
 * - It is the lowest index meeting the first two conditions (which will be the 
 *   start of that pair, and the highest value pair other  than the original 
 *   match).
 * If no such index is found, the function returns -1.
 */
{
  card_t **cards = hand->cards;
  size_t n_cards = hand->n_cards;
  unsigned value = cards[match_idx]->value;
  for (size_t i = 0; i < n_cards; ++i)
  {
    if (cards[i]->value != value && match_counts[i] > 1)
    {
      return i;
    }
  }
  return -1;
}


int is_n_length_straight_at(deck_t * hand, size_t index, suit_t fs, int n)
/* This function determines if there is a straight of length n, starting at 
 * index (and only  starting at index) in the given hand. If fs is NUM_SUITS, 
 * then it looks for any straight. If fs is some other value, then it looks for 
 * a straight flush in the specified suit. This function returns:
 *   0  if no straight was found at that index
 *   1  if a straight of length n was found at that index
 */
{
  if (n == 0)
  {
    return 1;
  }
  card_t **cards = hand->cards;
  size_t n_cards = hand->n_cards;
  int difference;
  size_t i = index + 1;
  while (i < n_cards)
  {
    difference = cards[index]->value - cards[i]->value;
    if (difference > 1) return 0;
    if (difference == 1 && (fs == NUM_SUITS || fs == cards[i]->suit))
      {
        return (is_n_length_straight_at(hand, i, fs, n - 1));
      }
    ++i;
  }
  return 0;
}

int is_ace_low_straight_at(deck_t * hand, size_t index, suit_t fs)
/* Function to see if an ace low straight is found at index. If fs = NUM_SUITS
 * then it looks for any straight, otherwise it looks for one in the suit 
 * specified. It check for a 5. It then checks for an ace. And then for a 
 * straight of length 4.
 */
{
  card_t **cards = hand->cards;
  size_t n_cards = hand->n_cards;
  if (index >= n_cards || cards[index]->value != 14)
  {
    return 0; // Index not an ace
  }
  size_t i = index;
  while (i < n_cards && ((cards[i]->value > 5) ||
        (cards[i]->value == 5 && fs != NUM_SUITS && 
        cards[i]->suit != fs)))
  {
    ++i; // Card value > 5 or card value is 5 but wrong suit
  }
  if (i >= n_cards || cards[i]->value != 5)
  {
    return 0; // 5 not found
  }
  return is_n_length_straight_at(hand, i, fs, 3);
}

int is_straight_at(deck_t * hand, size_t index, suit_t fs)
/* This function determines if there is a straight starting at index (and only 
 * starting at index) in the given hand. If fs is NUM_SUITS, then it looks for 
 * any straight. If fs is some other value, then it looks for a straight flush 
 * in the specified suit. This function returns:
 *   -1 if an Ace-low straight was found at that index 
 *       (and that index is the Ace)
 *   0  if no straight was found at that index
 *   1  if any other straight was found at that index
 *
 */
{
  if (fs != NUM_SUITS && hand->cards[index]->suit != fs)
  {
    return 0; // Wrong suit
  }
  if (hand->cards[index]->value < 5) 
  {
    return 0;
  }
  if (is_n_length_straight_at(hand, index, fs, 4))
  {
    return 1;
  }
  if (is_ace_low_straight_at(hand, index, fs))
  {
    return -1;
  }
  return 0;
}

int card_in_card_array(card_t **cards, size_t size, card_t *card)
{
  for (int i = 0; i < size; ++i)
  {
    if (cards[i] == card) return 1;
  }
  return 0;
}

hand_eval_t build_hand_from_match(
      deck_t * hand, unsigned n, hand_ranking_t what, size_t idx)
/* Function that constructs a hand_eval_t ans value with the hand ranking and
 * the five cards that make up the hand.
 */
{
  hand_eval_t ans;
  ans.ranking = what;
  card_t **cards = hand->cards;
  card_t *start_card = cards[idx];
  unsigned value = start_card->value;
  suit_t suit = start_card->suit;
  size_t index = idx;
  size_t i = 0;
  if (what == STRAIGHT || what == STRAIGHT_FLUSH)
  {
    while(i < n)
    {
      if (cards[index]->value == value)
      {
        if (what == STRAIGHT || cards[index]->suit == suit)
        { 
          ans.cards[i] = cards[index];
          ++i;
          ++value;
        }
      }
      ++index;
    }
  }
  else if (what == FLUSH)
  {
    while(i < n)
    {
      if (cards[i]->suit == suit)
      {
        ans.cards[i] = cards[index];
        ++i;
      }
      ++index;
    }
  }
  else {
    while (i < n)
    {
      ans.cards[i] = cards[index];
      ++i;
      ++index;
    }
  }
  index = 0;
  while (i < 5)
  {
    if (!card_in_card_array(ans.cards, i, cards[index]))
    {
      ans.cards[i] = cards[index];
      ++i;
    }
    ++index;
  }
  return ans;
}

int compare_hands(deck_t * hand1, deck_t * hand2)
/* Function that compares 2 hands. It returns a positive number
if hand 1 is better, 0 if the hands tie, and a negative number
if hand 2 is better.
*/
{
  qsort(hand1->cards, hand1->n_cards, sizeof(hand1->cards[0]), card_ptr_comp);
  qsort(hand2->cards, hand2->n_cards, sizeof(hand2->cards[0]), card_ptr_comp);
  hand_eval_t eval1 = evaluate_hand(hand1);
  hand_eval_t eval2 = evaluate_hand(hand2);
  hand_ranking_t rank1 = eval1.ranking;
  hand_ranking_t rank2 = eval2.ranking;
  if (rank1 != rank2) return rank2 - rank1;
  card_t **cards1 = eval1.cards;
  card_t **cards2 = eval2.cards;
  for (int i = 0; i < 5; ++i)
  {
    unsigned val1 = cards1[i]->value;
    unsigned val2 = cards2[i]->value;
    if (val1 != val2) return val1 - val2;
  } 
  return 0;
}

//You will write this function in Course 4.
//For now, we leave a prototype (and provide our
//implementation in eval-c4.o) so that the
//other functions we have provided can make
//use of get_match_counts.
unsigned * get_match_counts(deck_t * hand)
{
/* You will find its prototype after the code you wrote
   in Course 3, and before the functions we
   provided for you.  Replace the prototype with
   your implementation.

   Given a hand (deck_t) of cards, this function
   allocates an array of unsigned ints with as
   many elements as there are cards in the hand.
   It then fills in this array with
   the "match counts" of the corresponding cards.
   That is, for each card in the original hand,
   the value in the match count array
   is how many times a card of the same
   value appears in the hand.  For example,
   given
     Ks Kh Qs Qh 0s 9d 9c 9h
   This function would return
     2  2  2  2  1  3  3  3
   because there are 2 kings, 2 queens,
   1 ten, and 3 nines.
*/
  unsigned *arr = malloc(sizeof(*arr) * hand->n_cards);
  int count, value;
  for (size_t i = 0; i < hand->n_cards; ++i)
  {
    arr[i] = 0;
  }
  for (size_t i = 0; i < hand->n_cards; ++i)
  {
    if (arr[i] == 0)
    {
      value = hand->cards[i]->value;
      count = 1;
      for (size_t j = i + 1; j < hand->n_cards; ++j)
      {
        if (hand->cards[j]->value == value)
        {
          ++count;
        }
      }
      arr[i] = count;
      for (size_t j = i + 1; j < hand->n_cards; ++j)
      {
        if (hand->cards[j]->value == value)
        {
          arr[j] = count;
        }
      }
    }
  }
  return arr;
}

// We provide the below functions.  You do NOT need to modify them
// In fact, you should not modify them!

//This function copies a straight starting at index "ind" from deck "from".
//This copies "count" cards (typically 5).
//into the card array "to"
//if "fs" is NUM_SUITS, then suits are ignored.
//if "fs" is any other value, a straight flush (of that suit) is copied.
void copy_straight(card_t ** to, deck_t *from, size_t ind, suit_t fs, size_t count) {
  assert(fs == NUM_SUITS || from->cards[ind]->suit == fs);
  unsigned nextv = from->cards[ind]->value;
  size_t to_ind = 0;
  while (count > 0) {
    assert(ind < from->n_cards);
    assert(nextv >= 2);
    assert(to_ind <5);
    if (from->cards[ind]->value == nextv &&
	(fs == NUM_SUITS || from->cards[ind]->suit == fs)){
      to[to_ind] = from->cards[ind];
      to_ind++;
      count--;
      nextv--;
    }
    ind++;
  }
}


//This looks for a straight (or straight flush if "fs" is not NUM_SUITS)
// in "hand".  It calls the student's is_straight_at for each possible
// index to do the work of detecting the straight.
// If one is found, copy_straight is used to copy the cards into
// "ans".
int find_straight(deck_t * hand, suit_t fs, hand_eval_t * ans) {
  if (hand->n_cards < 5){
    return 0;
  }
  for(size_t i = 0; i <= hand->n_cards -5; i++) {
    int x = is_straight_at(hand, i, fs);
    if (x != 0){
      if (x < 0) { //ace low straight
	assert(hand->cards[i]->value == VALUE_ACE &&
	       (fs == NUM_SUITS || hand->cards[i]->suit == fs));
	ans->cards[4] = hand->cards[i];
	size_t cpind = i+1;
	while(hand->cards[cpind]->value != 5 ||
	      !(fs==NUM_SUITS || hand->cards[cpind]->suit ==fs)){
	  cpind++;
	  assert(cpind < hand->n_cards);
	}
	copy_straight(ans->cards, hand, cpind, fs,4) ;
      }
      else {
	copy_straight(ans->cards, hand, i, fs,5);
      }
      return 1;
    }
  }
  return 0;
}


//This function puts all the hand evaluation logic together.
//This function is longer than we generally like to make functions,
//and is thus not so great for readability :(
hand_eval_t evaluate_hand(deck_t * hand) {
  suit_t fs = flush_suit(hand);
  hand_eval_t ans;
  if (fs != NUM_SUITS) {
    if(find_straight(hand, fs, &ans)) {
      ans.ranking = STRAIGHT_FLUSH;
      return ans;
    }
  }
  unsigned * match_counts = get_match_counts(hand);
  unsigned n_of_a_kind = get_largest_element(match_counts, hand->n_cards);
  assert(n_of_a_kind <= 4);
  size_t match_idx = get_match_index(match_counts, hand->n_cards, n_of_a_kind);
  ssize_t other_pair_idx = find_secondary_pair(hand, match_counts, match_idx);
  free(match_counts);
  if (n_of_a_kind == 4) { //4 of a kind
    return build_hand_from_match(hand, 4, FOUR_OF_A_KIND, match_idx);
  }
  else if (n_of_a_kind == 3 && other_pair_idx >= 0) {     //full house
    ans = build_hand_from_match(hand, 3, FULL_HOUSE, match_idx);
    ans.cards[3] = hand->cards[other_pair_idx];
    ans.cards[4] = hand->cards[other_pair_idx+1];
    return ans;
  }
  else if(fs != NUM_SUITS) { //flush
    ans.ranking = FLUSH;
    size_t copy_idx = 0;
    for(size_t i = 0; i < hand->n_cards;i++) {
      if (hand->cards[i]->suit == fs){
	ans.cards[copy_idx] = hand->cards[i];
	copy_idx++;
	if (copy_idx >=5){
	  break;
	}
      }
    }
    return ans;
  }
  else if(find_straight(hand,NUM_SUITS, &ans)) {     //straight
    ans.ranking = STRAIGHT;
    return ans;
  }
  else if (n_of_a_kind == 3) { //3 of a kind
    return build_hand_from_match(hand, 3, THREE_OF_A_KIND, match_idx);
  }
  else if (other_pair_idx >=0) {     //two pair
    assert(n_of_a_kind ==2);
    ans = build_hand_from_match(hand, 2, TWO_PAIR, match_idx);
    ans.cards[2] = hand->cards[other_pair_idx];
    ans.cards[3] = hand->cards[other_pair_idx + 1];
    if (match_idx > 0) {
      ans.cards[4] = hand->cards[0];
    }
    else if (other_pair_idx > 2) {  //if match_idx is 0, first pair is in 01
      //if other_pair_idx > 2, then, e.g. A A K Q Q
      ans.cards[4] = hand->cards[2];
    }
    else {       //e.g., A A K K Q
      ans.cards[4] = hand->cards[4];
    }
    return ans;
  }
  else if (n_of_a_kind == 2) {
    return build_hand_from_match(hand, 2, PAIR, match_idx);
  }
  return build_hand_from_match(hand, 0, NOTHING, 0);
}
