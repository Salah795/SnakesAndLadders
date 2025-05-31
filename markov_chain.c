#include "markov_chain.h"
#include <string.h>

Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *node = get_node_from_database (markov_chain, data_ptr);
  if (node)
  {
    return node;
  }
  return new_node (markov_chain, data_ptr);
}

Node *new_node (MarkovChain *markov_chain, void *data_ptr)
{
  MarkovNode *markov_node = malloc (sizeof (MarkovNode));
  if (!markov_node)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  markov_node->data = markov_chain->copy_func (data_ptr);
  if (!markov_node->data)
  {
    free (markov_node);
    return NULL;
  }
  markov_node->frequencies_list = NULL;
  markov_node->follow_num = 0;
  markov_node->mnodef_capacity = 0;
  if (!markov_chain->database)
  {
    markov_chain->database = get_database ();
    if (!markov_chain->database)
    {
      markov_chain->free_data (markov_node->data);
      free (markov_node); //???
      return NULL;
    }
  }
  if (add (markov_chain->database, markov_node))
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    markov_chain->free_data (markov_node->data);
    free (markov_node); //??
    return NULL;
  }
  return markov_chain->database->last;
}

LinkedList *get_database ()
{
  return calloc (1, sizeof (LinkedList));
}

Node *
get_node_from_database (MarkovChain *markov_chain, void *data_ptr) //checked
{
  if (!markov_chain->database)
  {
    return NULL;
  }
  Node *curr_node = markov_chain->database->first;
  while (curr_node)
  {
    if (!markov_chain->comp_func (curr_node->data->data, data_ptr))
    {
      return curr_node;
    }
    curr_node = curr_node->next;
  }
  return NULL;
}

bool add_node_to_frequencies_list (MarkovNode *first_node, MarkovNode
//checked
*second_node, MarkovChain *markov_chain)
{
  if (!(first_node->data && second_node->data))
  {
    return false;
  }
  for (int index = 0; index < first_node->follow_num; index++)
  {
    if (!markov_chain->comp_func ((first_node->frequencies_list
                                   + index)->markov_node->data,
                                  second_node->data))
    {
      (first_node->frequencies_list + index)->frequency++;
      return true;
    }
  }
  if (first_node->follow_num + 1 > first_node->mnodef_capacity)
  {
    first_node->frequencies_list = realloc (first_node->frequencies_list,
                                            (first_node->mnodef_capacity +
                                             1) *
                                            sizeof (MarkovNodeFrequency));
    if (!first_node->frequencies_list)
    {
      return false;
    }
    first_node->mnodef_capacity++;
  }
  first_node->frequencies_list[first_node->follow_num].markov_node =
      second_node;
  first_node->frequencies_list[first_node->follow_num].frequency = 1;
  first_node->follow_num++;
  return true;
}

void free_database (MarkovChain **ptr_chain)
{
  if ((*ptr_chain)->database)
  {
    Node *curr_node = (*ptr_chain)->database->first, *next_node = NULL;
    while (curr_node)
    {
      next_node = curr_node->next;
      free_node (curr_node, *ptr_chain);
      curr_node = next_node;
    }
    free ((*ptr_chain)->database);
  }
  free (*ptr_chain);
  *ptr_chain = NULL;
}

void free_node (Node *node, MarkovChain *markov_chain) //checked
{
  markov_chain->free_data (node->data->data);
  free (node->data->frequencies_list);
  free (node->data);
  free (node);
}

Node *get_node_by_index (MarkovChain *markov_chain, int index) //checked
{
  Node *curr_node = markov_chain->database->first;
  for (int i = 0; i < index; i++)
  {
    curr_node = curr_node->next;
  }
  return curr_node;
}

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number maximal number to return (not including).
 * @return Random number.
 */
int get_random_number (int max_number)
{
  return rand () % max_number;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain) //checked
{
  int words_num = 0;
  Node *curr_node = markov_chain->database->first;
  while (curr_node)
  {
    words_num++;
    curr_node = curr_node->next;
  }
  int new_index = get_random_number (words_num);
  curr_node = get_node_by_index (markov_chain, new_index);
  while (markov_chain->is_last (curr_node->data->data))
  {
    new_index = get_random_number (words_num);
    curr_node = get_node_by_index (markov_chain, new_index);
  }
  return curr_node->data;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr) //checked
{
  int words_num = 0;
  for (int index = 0; index < state_struct_ptr->follow_num; index++)
  {
    words_num += (state_struct_ptr->frequencies_list + index)->frequency;
  }
  if (!words_num)
  {
    return NULL;
  }
  int new_index = get_random_number (words_num);
  int run_index = -1;
  MarkovNodeFrequency *curr_f = state_struct_ptr->frequencies_list;
  while (run_index < words_num)
  {
    run_index += curr_f->frequency;
    if (run_index >= new_index)
    {
      return curr_f->markov_node;
    }
    curr_f += 1;
  }
  return NULL;
}

void generate_tweet (MarkovChain *markov_chain, MarkovNode *  //checked
first_node, int max_length)
{
  if (!first_node)
  {
    first_node = get_first_random_node (markov_chain);
  }
  MarkovNode *next_node = first_node;
  for (int index = 1; index <= max_length; index++)
  {
    if (!next_node)
    {
      return;
    }
    if (markov_chain->is_last (next_node->data) || index == max_length)
    {
      markov_chain->print_func (next_node->data);
      return;
    }
    markov_chain->print_func (next_node->data);
    next_node = get_next_random_node (next_node);
  }
}