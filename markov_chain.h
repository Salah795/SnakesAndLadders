#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE "Allocation failure: Failed to allocate new memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/

typedef void (*print_function)(void*);
typedef int (*comp_function)(void*, void*);
typedef void (*free_data)(void*);
typedef void* (*copy_function)(void*);
typedef bool (*is_last)(void*);

/***************************/



/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNode {
    void *data;
    struct MarkovNodeFrequency *frequencies_list;
    int follow_num;
    int mnodef_capacity;
} MarkovNode;

typedef struct MarkovNodeFrequency {
    MarkovNode *markov_node;
    int frequency;
} MarkovNodeFrequency;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    print_function print_func;

    // pointer to a func that gets 2 pointers of generic data type(same one) and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    comp_function comp_func;

    // a pointer to a function that gets a pointer of generic data type and frees it.
    // returns void.
    free_data free_data;

    // a pointer to a function that  gets a pointer of generic data type and returns a newly allocated copy of it
    // returns a generic pointer.
    copy_function copy_func;

    //  a pointer to function that gets a pointer of generic data type and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    is_last is_last;
} MarkovChain;

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * This function creates new markov node with data=data_ptr, and add
 * the node that indicates to this markov node to the markov chain.
 * @param markov_chain pointer to object from type MarkovChain.
 * @param data_ptr pointer for the new data to add.
 * @return the new node if it succeeded, NULL if it's not.
 */
Node *new_node (MarkovChain *markov_chain, void *data_ptr);

/**
 * create new object from type LinkedList and return a pointer to it.
 * @return pointer to the new linked list.
 */
LinkedList *get_database ();

/**
 * free all the initialized memory for the given in Heap.
 * @param node pointer to node in markov chain.
 * @param markov_chain pointer to object of type MarkovChain.
 */
void free_node (Node *node, MarkovChain *markov_chain);

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

#endif /* MARKOV_CHAIN_H */
