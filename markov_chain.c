#include "linked_list.h"
#include <stdio.h>  // For printf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool
#include "markov_chain.h"
#include <string.h>
#define MAX_TWEET_LEN 20
#define MAX_WORD_LEN 100

int get_random_number (int max_number)
{
  return rand () % max_number;
}

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{ //first I want to get a random number out of the number of words I have
  while (true)
    {
      int num = get_random_number (markov_chain->database->size);
      Node *curr_node = markov_chain->database->first;
      for (int i = 0; i < num; i++)
        {
          curr_node = curr_node->next;
        }
      if (curr_node->data->last_word == false)
        {
          return curr_node->data;
        }
    }
}

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  if (state_struct_ptr->last_word == true)
    {
      return NULL;
    }
  int sum = state_struct_ptr->sum; //get the total number of occurrences
  int random_number = get_random_number (sum);

  NextNodeCounter *current_word = state_struct_ptr->counter_list;
  for (int i = 0; i < state_struct_ptr->size; i++)
    {
      if (random_number < (current_word + i)->frequency)
        { return (current_word + i)->markov_node; }
      random_number -= (current_word + i)->frequency;
    }
  return NULL;
}

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with,
 *                   if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  if (first_node == NULL)
    {
      first_node = get_first_random_node (markov_chain);
    }
  fprintf (stdout, "%s ", first_node->data);
  int printed_words = 1;
  bool printed_last_word = false;
  MarkovNode *head = first_node;
  while (printed_words <= max_length && !printed_last_word)
    {
      head = get_next_random_node (head);
      printed_last_word = head->last_word;
      fprintf (stdout, "%s", head->data);
      if (printed_last_word == false)
        {
          fprintf (stdout, " ");
        }
      printed_words++;
    }
}
/**
 * in case the first node already has a counter_list and we want to enlarge it
 * there are two options:
 * either the next word is already in the counter list and then the function
 * merely increases the sum
 * or the second word is not present in the first node's counter list, and
 * at that case, we realloc new spot for  a next node counter and place that
 * word there.
 **/

bool enlarge_counter_list (MarkovNode *first_node, MarkovNode *second_node)
{
  if (strcmp (second_node->data,
              (first_node->counter_list)[0].markov_node->data) == 0)
    {
      (first_node->counter_list)[0].frequency += 1;
      first_node->sum += 1;
      return true;
    }
  for (int i = 0; i < first_node->size; i++)
    {

      if (strcmp (second_node->data,
                  (first_node->counter_list)[i].markov_node->data) == 0)
        {
          (first_node->counter_list)[i].frequency += 1;
          first_node->sum += 1;
          return true;
        }
    }
  NextNodeCounter *temp = realloc (first_node->counter_list,
                                   ((first_node->size) + 1)
                                   * sizeof (NextNodeCounter));
  if (temp == NULL)
    { return false; }
  temp[first_node->size] = (NextNodeCounter)
      {second_node, 1};
  (first_node->size) += 1, (first_node->sum) += 1;
  first_node->counter_list = temp;
  return true;
}

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 **/

bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode *second_node)
{
  if (first_node->last_word == true)
    {
      first_node->counter_list = NULL;
      first_node->size += 1;
      first_node->sum += 1;
      return true;
    }
  if ((first_node->counter_list != NULL)
      && (first_node->next_node_counter_allocated == true))
    {
      return enlarge_counter_list (first_node, second_node);
    }
  //counter_list = NULL: I will create a new Nextnodecounter
  if ((first_node->counter_list = malloc (sizeof (NextNodeCounter)))
      == NULL)
    { return false; }
  first_node->next_node_counter_allocated = true;
  first_node->next_word_counter_freed = false;
  first_node->counter_list->frequency = 1;
  first_node->counter_list->markov_node = second_node;
  first_node->size += 1, first_node->sum += 1;
  return true;
}

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it
 * in the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */

Node *get_node_from_database (MarkovChain *markov_chain, char *data_ptr)
{
  Node *head = markov_chain->database->first;
  if (head == NULL || data_ptr == NULL)
    { return NULL; }

  while (head != markov_chain->database->last)
    {
      if (strcmp (data_ptr, head->data->data) == 0)
        {
          return head;
        }
      head = head->next;
    }
  if (strcmp (data_ptr, head->data->data) == 0)
    {
      return head;
    }
  return NULL;
}

/**
 * @param data_ptr word given from corpus.
 * @return true if ends with '.', false otherwise
 */

bool is_last_word (char *data_ptr)
{
  size_t len = strlen (data_ptr);
  if (strcmp (&(data_ptr[len - 1]), ".") == 0)
    {
      return true;
    }
  return false;
}

/**
 * creates new MarkovNode and initialise its fields
 * @param data_ptr
 * @return
 */

MarkovNode *create_new_markov_node_ptr (char *data_ptr)
{
  MarkovNode *new_markov_node_ptr = malloc (sizeof (MarkovNode));
  char *new_data = malloc (MAX_WORD_LEN * sizeof (char));
  if (new_data == NULL || new_markov_node_ptr == NULL || data_ptr == NULL)
    { return NULL; }

  strcpy (new_data, data_ptr);
  new_markov_node_ptr->data = new_data;
  new_markov_node_ptr->size = 0, new_markov_node_ptr->sum = 0;
  new_markov_node_ptr->last_word = is_last_word (new_data);
  new_markov_node_ptr->next_node_counter_allocated = false;
  new_markov_node_ptr->next_word_counter_freed = false;

  return new_markov_node_ptr;
}

/**
* If data_ptr in markov_chain, return it's markov_node. Otherwise, create new
 * markov_node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */

Node *add_to_database (MarkovChain *markov_chain, char *data_ptr)
{
  //todo: every time initialise new markov_node_pointer
  Node *in_database = get_node_from_database (markov_chain, data_ptr);
  if (in_database != NULL)
    {
      return in_database;
    }
  MarkovNode *new_markov_node_ptr = create_new_markov_node_ptr (data_ptr);
  if (new_markov_node_ptr == NULL)
    {
      return NULL;
    }
  int success = add (markov_chain->database, &new_markov_node_ptr);
  if (success == 1)
    { return NULL; }
  markov_chain->database->last->data = new_markov_node_ptr;
  return markov_chain->database->last;
}

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */

void free_markov_chain (MarkovChain **ptr_chain)
{
  Node *head = (*ptr_chain)->database->first;
  Node *temp = NULL;
  while (head != NULL)
    {
      temp = head->next;
      if ((head->data->counter_list != NULL) &&
          (head->data->next_word_counter_freed == false) &&
          head->data->last_word == false)
        {
          free (head->data->counter_list); //NextNodeCounter
          head->data->counter_list = NULL;
        }
      head->data->next_word_counter_freed = true;
      free (head->data->data); //String
      head->data->data = NULL, head->data->counter_list = NULL,
          head->data->size = 0, head->data->sum = 0;
      free (head->data); //MarkovNode
      head->data = NULL;
      free (head); //MarkovChain
      head = temp;
    }
  free ((*ptr_chain)->database); //LinkedList
  (*ptr_chain)->database = NULL;
  free (*ptr_chain); //MarkovChain
  *ptr_chain = NULL;
}