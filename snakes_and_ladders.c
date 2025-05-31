#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define SEED_INDEX 1
#define PATH_INDEX 2
#define BASE 10
#define ARGS_NUM 2

#define ARGS_NUM_ERROR_MESSAGE "Usage: the number of arguments should be 2.\n"

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder in case there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case there is one from this square
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  printf ("%s", error_msg);
  if (database != NULL)
  {
    free_database(database);
  }
  return EXIT_FAILURE;
}

static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc (sizeof (Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++)
      {
        free (cells[j]);
      }
      handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_frequencies_list(from_node, to_node,
                               markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell *) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
        add_node_to_frequencies_list(from_node, to_node,
                                 markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

static bool check_last (void *data)
{
  Cell *cell_data = (Cell *) data;
  return (cell_data->number == BOARD_SIZE);
}

static int comp_cell (void *first, void *second)
{
  Cell *first_cell = (Cell *) first;
  Cell *second_cell = (Cell *) second;
  return first_cell->number - second_cell->number;
}

static void *cpy_cell (void *data)
{
  Cell *cell_data = (Cell *) data;
  Cell *new_cell = malloc (sizeof (Cell));
  if (new_cell)
  {
    new_cell->number = cell_data->number;
    new_cell->ladder_to = cell_data->ladder_to;
    new_cell->snake_to = cell_data->snake_to;
  }
  return (void *) new_cell;
}

static void free_cell (void *data)
{
  Cell *cell_data = (Cell *) data;
  free (cell_data);
}

static void print_cell (void *data)
{
  Cell *cell_data = (Cell *) data;
  if (check_last (data))
  {
    printf ("[%d]", cell_data->number);
    return;
  }
  if (cell_data->ladder_to != EMPTY)
  {
    printf ("[%d]-ladder to %d -> ", cell_data->number,
            cell_data->ladder_to);
    return;
  }
  if (cell_data->snake_to != EMPTY)
  {
    printf ("[%d]-snake to %d -> ", cell_data->number,
            cell_data->snake_to);
    return;
  }
  printf ("[%d] -> ", cell_data->number);
}

static int get_path (char **argv, MarkovChain *markov_chain)
{
  int fill = 0;
  fill = fill_database (markov_chain);
  if (fill)
  {
    free_database(&markov_chain);
    return 1;
  }
  int tweets_num = strtol (argv[PATH_INDEX], NULL, BASE);
  MarkovNode *first_cell = markov_chain->database->first->data;
  for (int tweet = 0; tweet < tweets_num; tweet++)
  {
    printf ("Random Walk %d: ", tweet + 1);
    generate_tweet(markov_chain, first_cell, MAX_GENERATION_LENGTH);
    printf ("\n");
  }
  free_database(&markov_chain);
  return 0;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main (int argc, char *argv[])
{
  if ((argc - 1 != ARGS_NUM))
  {
    printf (ARGS_NUM_ERROR_MESSAGE);
    return EXIT_FAILURE;
  }
  srand ((unsigned int) strtol (argv[SEED_INDEX], NULL, BASE));
  MarkovChain *markov_chain = calloc(1, sizeof(MarkovChain));
  if(!markov_chain)
  {
    return EXIT_FAILURE;
  }
  markov_chain->comp_func = comp_cell;
  markov_chain->copy_func =cpy_cell;
  markov_chain->free_data = free_cell;
  markov_chain->print_func = print_cell;
  markov_chain->is_last = check_last;
  return get_path (argv, markov_chain);
}
