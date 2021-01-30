//-----------------------------------------------------------------------------
//
// Program that allows you to play a game
// of blackjack against the computer as a dealer
//
// Author: Bakir Haljevac 
//-----------------------------------------------------------------------------
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECK_SIZE 52
#define ALLOC_SIZE 50
#define PATH_LENGTH 100
#define NUM_CARDS 13
#define OPTION_INPUT_LENGTH 20
#define FILE_NAME_LENGTH 10
#define ARGUMENTS_ERROR -1
#define MEMORY_ERROR -2
#define FILE_ERROR -3

typedef struct _Card_ 
{
  char* image_;
  int points_;
} Card;

//-----------------------------------------------------------------------------
///
/// The Fisher-Yates Shuffle algorithm to mix(shuffle) the deck.
/// For every 'i' in range (0, size): swap element[i] and element 
/// [random_i] where 'random_i' is a pseudo-random number in range [0, i).
///
/// @param deck The deck to shuffle.
/// @param size Size of a deck.
/// @param random_seed The number to initialize pseudo-random 
///        number generator.
///
//
void FisherYates(Card* deck, int size, int random_seed) 
{
  srand(random_seed);
  for (int i = size - 1; i > 0; i--) 
  {
    int swap_index = rand() % (i + 1);
    Card tmp = deck[i];
    deck[i] = deck[swap_index];
    deck[swap_index] = tmp;
  }
}

//-----------------------------------------------------------------------------
///
/// Writes player's or dealer's cards and score to stdout
///
/// @param cards The cards for printing.
/// @param length The number of cards to be shown.
/// @param score The score to be shown.
/// @param width Width of single card image.
/// @param height Height of single card image.
/// @param player Value that can be 1(player's cards) or 0(dealer's cards).
///
//
void showCards(Card* cards, int length, int score,
 int width, int height, int player)
{
  printf(player == 1 ? "YOUR CARDS:\n\n" : "DEALERS CARDS:\n\n");
  printf("____________________________________________________________\n");
  int offset = 0;
  for (int j = 0; j < height; j++) 
  {
    for (int i = 0; i < length; i++) 
    {
      char* img = cards[i].image_;
      img += offset;
      while (*img != '\n') 
      {
        printf("%c", *img);
        img++;
      }
      printf("  ");
    }
    printf("\n");
    offset += width;
  }
  printf("score:%d\n\n", score);
  printf("____________________________________________________________\n");
}

//-----------------------------------------------------------------------------
///
/// Transfers @amount of cards from @cards deck to @receiver deck.
/// Increases receiver's @score for the value of the cards.
/// Updates @card_count and @receiver_count.
///
/// @param cards The deck from which the cards are dealt.
/// @param receiver The deck to which the cards are dealt.
/// @param card_count The number of cards in first deck.
/// @param receiver_count The number of cards in receiver deck.
/// @param score The receiver's score.
/// @param amount The number of cards to be dealt to receiver.
///
//
void giveCards(Card* cards, Card* receiver, int* card_count,
 int* receiver_count, int* score, int amount) 
{
  for (int i = 0; i < amount; i++) 
  {
    receiver[*receiver_count] = cards[*card_count];
    if (cards[*card_count].points_ == 11) //if it's ace
    { 
      int points_ace = *score > 10 ? 1 : 11;
      *score += points_ace;
    }
    else 
    {
      *score += cards[*card_count].points_;
    }
    (*receiver_count)++;
    (*card_count)++;
  }
}

//-----------------------------------------------------------------------------
///
/// Prints error message and terminates the program with error code.
///
/// @param executable The name of executable program.
/// @return int The error code.
///
//
int argumentsError(char* executable) 
{
  printf("usage: %s <input_folder>\n", executable);
  return ARGUMENTS_ERROR;
}

//-----------------------------------------------------------------------------
///
/// Prints error message and terminates the program with error code.
///
/// @return int The error code.
///
//
int memoryError() 
{
  printf("[ERR] Out of memory.\n");
  return MEMORY_ERROR;
}

//-----------------------------------------------------------------------------
///
/// Prints error message and terminates the program with error code.
///
/// @return int The error code.
///
//
int fileError() 
{
  printf("[ERR] Invalid File(s).\n");
  return FILE_ERROR;
}

//-----------------------------------------------------------------------------
///
/// Frees(deallocates) used space on the heap.
///
/// @param card_images The allocated memory used for card images.
/// @param size The allocated space size.
///
//
void deallocateMemory(char** card_images, int size) 
{
  for (int i = 0; i < size; i++) 
  {
    free(card_images[i]);
  }
}

//------------------------------------------------------------------------------
///
/// The main program.
/// Reads card images from files conatained in input map(second argument)
/// and makes a deck. The cards from deck are dealt to dealer and player 
/// and game of blackjack starts.
///
/// @param argc Number of arguments (should be 2 or 3)
/// @param argv The executable name, input map and number 
///        for generating random seed(optional)
/// @return zero if program ends without errors,
///         for unexpected program end, see error codes on the top
//
int main(int argc, char** argv) 
{
  if (argc < 2 || argc > 3) 
  {
    return argumentsError(argv[0]);
  }

  char* input_path = argv[1];
  if (input_path[strlen(input_path) - 1] != '/') 
  {
    strcat(input_path, "/");
  }

  int seed = time(NULL);
  char* rest;
  if (argc == 3) 
  {
    seed = strtol(argv[2], &rest, 10);
  }

  Card cards[DECK_SIZE];
  char* file_names[] = { 
    "ace.txt", "king.txt", "queen.txt", "jack.txt", "10.txt", 
    "9.txt", "8.txt", "7.txt", "6.txt", 
    "5.txt", "4.txt", "3.txt", "2.txt"
  };
  int points[] = { 11, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2 };
  char* card_images[NUM_CARDS] = { NULL };

  FILE* card_file;
  int c; //to read chars from file

  char file_path[PATH_LENGTH];
  strcpy(file_path, input_path);

  int image_height = 0;
  int image_width = 0;

  int size = ALLOC_SIZE;
  int card_count = 0;

  for (int i = 0; i < NUM_CARDS; i++) 
  { 
    char file_to_open[PATH_LENGTH + FILE_NAME_LENGTH];
    strcpy(file_to_open, file_path);
    strcat(file_to_open, file_names[i]);

    card_file = fopen(file_to_open, "r");
    if (card_file == NULL) 
    {
      deallocateMemory(card_images, i + 1);
      return fileError();
    }

    int nch = 0; //num of chars in file
    int nln = 0; //num of lines in file
    int lnlen = -1; //line length
    int currlnlen = 0; //current line length

    card_images[i] = malloc(size); //allocate buffer to store file content
    if (card_images[i] == NULL) 
    {
      deallocateMemory(card_images, i + 1);
      return memoryError();
    }
    
    while ((c = getc(card_file)) != EOF) 
    {
      if (nch >= size - 1) //time to reallocate
      { 
        size *= 2;
        card_images[i] = realloc(card_images[i], size);
        if (card_images[i] == NULL) 
        {
          deallocateMemory(card_images, i + 1);
          return memoryError();
        } 
      }

      //add new character and update lengths
      card_images[i][nch++] = c;
      currlnlen++;
      if (c == '\n') 
      {
        nln++;
        if (lnlen == -1) 
        {
          lnlen = currlnlen;
        }
        else if (currlnlen != lnlen) 
        {
          deallocateMemory(card_images, i + 1);
          return fileError();
        }
        currlnlen = 0;
      }
    }

    if (i == 0) 
    {
      image_height = nln;
      image_width = lnlen;
    }
    else if (image_height != nln || image_width != lnlen) 
    {
      deallocateMemory(card_images, i + 1);
      return fileError();
    }
    
    //add 4 cards of current image to the deck
    for (int k = 0; k < 4; k++) 
    {
      Card card = { card_images[i], points[i]};
      cards[card_count++] = card;
    }

    fclose(card_file);
  }

  //THE GAME STARTS...

  //shuffle cards
  FisherYates(cards, DECK_SIZE, seed);

  Card dealer[DECK_SIZE];
  Card player[DECK_SIZE];
  int dealer_count = 0;
  int player_count = 0;
  int dealer_score = 0;
  int player_score = 0;
  card_count = 0;

  giveCards(cards, player, &card_count, &player_count, &player_score, 2);
  giveCards(cards, dealer, &card_count, &dealer_count, &dealer_score, 2);

  showCards(dealer, 1, dealer[0].points_, image_width, image_height, 0);
  showCards(player, player_count, player_score, image_width, image_height, 1);

  if (player_score == 21) 
  {
    printf("BLACKJACK! ");
    showCards(dealer, 2, dealer_score, image_width, image_height, 0);
    if (dealer_score != 21) 
    {
      printf("YOU WIN!");
    }
    else 
    {
      printf("BLACKJACK! PUSH!");
    }
    deallocateMemory(card_images, NUM_CARDS);
    return 0;
  }

  int players_turn = 1; //player starts first

  while(1) 
  {
    if (!players_turn) 
    {
      printf("DEALERS TURN\n");
      showCards(dealer, 2, dealer_score, image_width, image_height, 0);
      if (dealer_score == 21 && dealer_count == 2) 
      {
        printf("BLACKJACK! YOU LOOSE!");
        break;
      }
      while (dealer_score < player_score) 
      {
        printf("DEALER GETS ANOTHER CARD..\n");
        giveCards(cards, dealer, &card_count, &dealer_count, &dealer_score, 1);
        showCards(dealer, dealer_count, dealer_score,
         image_width, image_height, 0);
      }
      if (dealer_score == 21) 
      {
        if (player_score == 21) 
        {
          printf("PUSH!");
        }
        else 
        {
          printf("YOU LOOSE!");
        }
        break;
      }
      if (dealer_score > 21) 
      {
        printf("BUST! YOU WIN!");
        break;
      }
      players_turn = 1;
    }
    else //players turn
    {
      char option[OPTION_INPUT_LENGTH];
      printf("HIT (h) or STAND (s)\n");
      scanf("%s", option);
      if (strcmp(option, "h") == 0) 
      {
        giveCards(cards, player, &card_count, &player_count, &player_score, 1);
        showCards(player, player_count, player_score,
         image_width, image_height, 1);
        if (player_score == 21) 
        {
          players_turn = 0;
        }
        else if (player_score > 21) 
        {
          printf("BUST! YOU LOOSE!");
          break;
        }
      }
      else if (strcmp(option, "s") == 0) 
      {
        players_turn = 0;
      }
    }
  }

  deallocateMemory(card_images, NUM_CARDS);

  return 0;
}
