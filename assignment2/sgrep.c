#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

/*
 * Fill out your own functions here (If you need)
 */
int IsRepetition (const char *pcStrt, const char *pcEnd)
{
  const char *pcTemp;
  pcTemp = pcStrt;

  while (pcTemp != pcEnd)
  {
    if (*pcTemp != *pcStrt)
      return 0;
    pcTemp++;
  }
  
  return 1;
}

int PatternInBuf(const char *buf, const char *pattern)
{
  static int num_frag = 1;
  
  char *asterisk_ptr;
  asterisk_ptr = StrFindChr(pattern, '*');

  if (asterisk_ptr == NULL)
  {
    char *patt_buf;
    patt_buf = StrFindStr(buf, pattern);

    if (patt_buf == NULL)
      return 0;
    else if (num_frag == 1)
      return 1;
    else if (IsRepetition(buf, patt_buf))
      return 1;
    else
      return 0;
  }
  else
  {
    num_frag++;

    char *patt_frag;
    char *patt_frag_end;
    char frag[MAX_STR_LEN];
    patt_frag = frag;

    StrCopy(patt_frag, pattern);
    int patt_frag_len = asterisk_ptr - pattern;
    patt_frag_end = patt_frag + patt_frag_len;
    *patt_frag_end = '\0';

    char *patt_frag_buf;
    patt_frag_buf = StrFindStr(buf, patt_frag);

    if(patt_frag_buf == NULL)
      return 0;
    else if (IsRepetition(buf, patt_frag_buf))
      {
        char *new_buf;
        char *new_patt;
        new_buf = patt_frag_buf + StrGetLength(patt_frag);
        new_patt = (char*)pattern + StrGetLength(patt_frag);
        PatternInBuf(new_buf, new_patt);
        return 0;
      }
    else
      return 0;
  }
}
/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void
PrintUsage(const char* argv0)
{
  const static char *fmt =
	  "Simple Grep (sgrep) Usage:\n"
	  "%s pattern [stdin]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* SearchPattern()
   Your task:
   1. Do argument validation
   - String or file argument length is no more than 1023
   - If you encounter a command-line argument that's too long,
   print out "Error: pattern is too long"

   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes,
   print out "Error: input line is too long"
   - Error message should be printed out to standard error (stderr)

   3. Check & print out the line contains a given string (search-string)

   Tips:
   - fgets() is an useful function to read characters from file. Note
   that the fget() reads until newline or the end-of-file is reached.
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int
SearchPattern(const char *pattern)
{
  char buf[MAX_STR_LEN + 2];

  /*
   *  TODO: check if pattern is too long
   */
  size_t pattern_len = StrGetLength(pattern) + 1;
  if (pattern_len > MAX_STR_LEN)
  {
    fprintf(stderr, "Error: pattern is too long\n");
    return FALSE;
  }


  /* Read one line at a time from stdin, and process each line */
  while (fgets(buf, sizeof(buf), stdin)) {

    /* TODO: check the length of an input line */
    /* TODO: fill out this function */
    size_t line_len = StrGetLength(buf) + 1;
    if (line_len > MAX_STR_LEN)
    {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }

    if (PatternInBuf(buf, pattern))
      printf("%s\n", buf);
  }

  return TRUE;
}
/*-------------------------------------------------------------------*/
int
main(const int argc, const char *argv[])
{
  /* Do argument check and parsing */
  if (argc < 2) {
	  fprintf(stderr, "Error: argument parsing error\n");
	  PrintUsage(argv[0]);
	  return (EXIT_FAILURE);
  }

  return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}
