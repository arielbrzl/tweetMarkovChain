#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"
#define MAX_LINE_LEN 1000
#define FLAG -1
#define DELIMITERS " \r\n\t"
#define PATH_FILE_ERROR "Error: path file is not valid"
#define ARGS_ERROR "Usage: you should insert 3 or 4 args exactly"
#define ALLOC_ERROR "Error: failed due to allocation error"
#define DECIMAL 10
#define READING_MODE "r"
#define MAX_TWEET_LEN 20
#define MAX_WORD_LEN 100
#define FILE_POSITION 4
#define CORRCET_NUM_OF_ARGS 5
#define INCORRECT_NUM_OF_ARGS 4
/**
 * checks whether arguments are valid
 * @return 0 if user wants to read the entire file,
 *        -1 if arguments error
 *        num of words to read if specified
 */
int input_helper (int argc, char *argv[])
{
  if (argc == CORRCET_NUM_OF_ARGS)
    {
      return strtol (argv[FILE_POSITION], NULL, DECIMAL);
    }
  if (argc == INCORRECT_NUM_OF_ARGS)

    {
      return 0;
    }
  fprintf (stdout, ARGS_ERROR);
  return FLAG;
}
/**
 * creates markov chain, linkedlist, initialises everything
 */
MarkovChain *create_markov_chain ()
{
  LinkedList *database = malloc (sizeof (LinkedList));
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (database == NULL || markov_chain == NULL)
    { return NULL; }
  database->first = NULL, database->last = NULL, database->size = 0;
  markov_chain->database = database;
  return markov_chain;
}

FILE *open_file (char **argv)
{
  FILE *fp = fopen (argv[3], READING_MODE);
  if (fp == NULL)
    {
      return NULL;
    }
  return fp;
}
/**
 * prints an error message, frees the markov chain and exits
 */
int error_out (char *str, MarkovChain *markov_chain)
{
  fprintf (stdout, "%s", str);
  free_markov_chain (&markov_chain);
  markov_chain = NULL;
  return EXIT_FAILURE;
}
/**
 * uses generate random sequence func to print all the tweets
 */
void print_tweets (MarkovChain *markov_chain, char *argv[])
{
  int num_of_tweets = strtol (argv[2], NULL, DECIMAL);
  for (int i = 1; i <= num_of_tweets; i++)
    {
      fprintf (stdout, "Tweet %d: ", i);
      generate_random_sequence (markov_chain, NULL, MAX_TWEET_LEN);
      fprintf (stdout, "\n");
    }
}
/**
 * this function fills the database.
 * if the user wants to read the entire file words_to_read will be 0, and
 * the counter i will not increase.
 * the func uses two while loops to ensure that the file doesnt end
 * (fgets=NULL and that we didnt cross the line restrictions)
 *
 */
int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char buffer[MAX_LINE_LEN];
  int i = (words_to_read == 0) ? FLAG : 0; //read the entire file?

  while ((fgets (buffer, MAX_LINE_LEN, fp) != NULL) && i < words_to_read)
    {
      char *first_str = strtok (buffer, DELIMITERS);
      if (first_str == NULL) //reached end of line or end of file
        { continue; }
      Node *first = add_to_database (markov_chain, first_str);
      i = (words_to_read == 0) ? i : i + 1;
      if (first == NULL) // alloc didn't work
        { return FLAG; }
      Node *second;
      while (i < words_to_read)
        {
          char *second_str = strtok (NULL, DELIMITERS);
          if (second_str == NULL) //reached end of line or end of file
            {
              break;
            }
          second = add_to_database (markov_chain, second_str);
          if (second == NULL) //alloc didn't succeed
            { return FLAG; }
          bool ok = add_node_to_counter_list (first->data, second->data);
          if (!ok)
            { return FLAG; }
          first = second;
          i = (words_to_read == 0) ? i : i + 1;
        }
    }
  return 0;
}

int main (int argc, char *argv[])
{
  int words_to_read = input_helper (argc, argv);
  if (words_to_read == FLAG)
    { return EXIT_FAILURE; }
  MarkovChain *markov_chain = create_markov_chain ();
  FILE *fp = open_file (argv);
  if (fp == NULL)
    {
      return error_out (PATH_FILE_ERROR, markov_chain);
    }
  int fill_database_succeeded = fill_database (fp,
                                               words_to_read, markov_chain);
  if (fill_database_succeeded == FLAG)
    { return error_out (ALLOC_ERROR, markov_chain); }
  fclose (fp);
  srand (strtol (argv[1], NULL, DECIMAL));
  print_tweets (markov_chain, argv);
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}