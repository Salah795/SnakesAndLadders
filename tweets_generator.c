#include <stdio.h>
#include "markov_chain.h"
#include <string.h>

#define MAX_SENTENCE 1000

#define ARGS_NUM_1 3
#define ARGS_NUM_2 4

#define SEED_INDEX 1
#define TWEETS_NUM_INDEX 2
#define FILE_PATH_INDEX 3
#define WORDS_TO_READ_INDEX 4

#define READ_ALL_FILE (-1)

#define BASE 10

#define MAX_TWEET 20

#define ARGS_NUM_ERROR_MESSAGE "Usage: the number of arguments should be 3 \
or 4.\n"
#define FILE_PATH_ERROR "Error: the given file is not valid.\n"

static bool end_of_sentence (void *data)
{
  char *string_data = (char *) data;
  return !strcmp (&string_data[strlen (string_data) - 1], ".");
}

static void print_data (void *data)
{
  char *string_data = (char *) data;
  if (end_of_sentence (data))
  {
    printf ("%s", string_data);
  }
  else
  {
    printf ("%s ", string_data);
  }
}

static int comp_data (void *first, void *second)
{
  char *string_first = (char *) first;
  char *string_second = (char *) second;
  return strcmp (string_first, string_second);
}

static void *cpy_func (void *data)
{
  char *string_data = (char *) data;
  char *new_data = calloc (1, strlen (string_data) + 1);
  if (new_data)
  {
    strcpy (new_data, string_data);
  }
  return (void *) new_data;
}

static void free_data_func (void *data)
{
  char *string_data = (char *) data;
  free (string_data);
}

static int fill_database (FILE *fp, int words_to_read,
                          MarkovChain *markov_chain)
{
  char text[MAX_SENTENCE];
  int words_counter = 1;
  while (fgets (text, MAX_SENTENCE, fp) &&
         (words_to_read == READ_ALL_FILE || words_counter <= words_to_read))
  {
    char *data = strtok (text, " \n\r");
    Node *previous_node = NULL;
    while ((data != NULL) &&
           (words_to_read == READ_ALL_FILE || words_counter <= words_to_read))
    {
      Node *now_node = add_to_database (markov_chain, data);
      if (now_node != NULL)
      {
        if ((previous_node != NULL)
            && !end_of_sentence (previous_node->data->data))
        {
          bool add_to_database = add_node_to_frequencies_list (
              previous_node->data,
              now_node->data, markov_chain);
          if (add_to_database == false)
          {
            return 1;
          }
        }
        data = strtok (NULL, " \n\r");
        previous_node = now_node;
        words_counter++;
      }
      else
      {
        return 1;
      }
    }
  }
  return 0;
}

static int get_tweets (FILE *file_ptr, char **argv, MarkovChain *markov_chain,
                       bool with_words_to_read)
{
  int fill = 0;
  if (with_words_to_read)
  {
    fill = fill_database (file_ptr, (int)
        strtol (argv[WORDS_TO_READ_INDEX],
                NULL, BASE), markov_chain);
  }
  else
  {
    fill = fill_database (file_ptr,
                          READ_ALL_FILE, markov_chain);
  }
  if (fill)
  {
    fclose (file_ptr);
    free_database (&markov_chain);
    return 1;
  }
  int tweets_num = strtol (argv[TWEETS_NUM_INDEX], NULL, BASE);
  for (int tweet = 0; tweet < tweets_num; tweet++)
  {
    printf ("Tweet %d: ", tweet + 1);
    generate_tweet (markov_chain, NULL, MAX_TWEET);
    printf ("\n");
  }
  fclose (file_ptr);
  free_database (&markov_chain);
  return 0;
}

int main (int argc, char *argv[])
{
  if ((argc - 1 != ARGS_NUM_1) && (argc - 1 != ARGS_NUM_2))
  {
    printf (ARGS_NUM_ERROR_MESSAGE);
    return EXIT_FAILURE;
  }
  FILE *file_ptr = fopen (argv[FILE_PATH_INDEX], "r");
  if (!file_ptr)
  {
    printf (FILE_PATH_ERROR);
    return EXIT_FAILURE;
  }
  srand ((unsigned int) strtol (argv[SEED_INDEX], NULL, BASE));
  MarkovChain *markov_chain = calloc (1, sizeof (MarkovChain));
  markov_chain->copy_func = cpy_func;
  markov_chain->free_data = free_data_func;
  markov_chain->comp_func = comp_data;
  markov_chain->is_last = end_of_sentence;
  markov_chain->print_func = print_data;
  if (argc - 1 == ARGS_NUM_1)
  {
    return get_tweets (file_ptr, argv, markov_chain, false);
  }
  if (argc - 1 == ARGS_NUM_2)
  {
    return get_tweets (file_ptr, argv, markov_chain, true);
  }
  return EXIT_FAILURE;
}