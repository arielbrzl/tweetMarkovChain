#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

typedef struct NextNodeCounter NextNodeCounter;
typedef struct MarkovNode MarkovNode;
typedef struct MarkovChain MarkovChain;

typedef struct NextNodeCounter {
    struct MarkovNode *markov_node;
    int frequency;
} NextNodeCounter;

typedef struct MarkovNode
//remember to initialize size to 0!!
{
    char *data;
    struct NextNodeCounter *counter_list; //array of next_node_counter
    int size; //number of separate words that come after that word (sizeof
    // counter_list)
    int sum; //sum of all frequencies
    bool last_word;
    bool next_node_counter_allocated;
    bool next_word_counter_freed;
} MarkovNode;

typedef struct MarkovChain {
    struct LinkedList *database;
} MarkovChain;

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with,
 *                   if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain (MarkovChain **ptr_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool
add_node_to_counter_list (MarkovNode *first_node, MarkovNode *second_node);

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it
 * in the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node *get_node_from_database (MarkovChain *markov_chain, char *data_ptr);

/**
* If data_ptr in markov_chain, return it's markov_node. Otherwise, create new
 * markov_node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */
Node *add_to_database (MarkovChain *markov_chain, char *data_ptr);

/**
 * creates new MarkovNode and initialise its fields
 * @param data_ptr
 * @return
 */

MarkovNode *create_new_markov_node_ptr (char *data_ptr);

/**
 * @param data_ptr word given from corpus.
 * @return true if ends with '.', false otherwise
 */
bool is_last_word (char *data_ptr);
/**
 * in case the first node already has a counter_list and we want to enlarge it
 * there are two options:
 * either the next word is already in the counter list and then the function
 * merely increases the sum
 * or the second word is not present in the first node's counter list, and
 * at that case, we realloc new spot for  a next node counter and place that
 * word there.
 **/
bool enlarge_counter_list (MarkovNode *first_node, MarkovNode *second_node);

#endif /* _MARKOV_CHAIN_H */
