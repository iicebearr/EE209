#include <assert.h> /* to use assert() */
#include <stdio.h>
#include <stdlib.h> /* for strtol() */
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <ctype.h>
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
int IsOverflow(long result, long new_digit);
int IsUnderflow(long result, long new_digit);


/*------------------------------------------------------------------------*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  assert(pcSrc);

  const char *pcSrc_curr;
  char *pcDest_curr;
  pcSrc_curr = pcSrc;
  pcDest_curr = pcDest;

  while (*pcSrc_curr != '\0')
  {
    *pcDest_curr = *pcSrc_curr;
    pcDest_curr++; 
    pcSrc_curr++;
  }

  *pcDest_curr = '\0';

  return pcDest;
}

/*------------------------------------------------------------------------*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  /* TODO: fill this function */
  assert(pcS1);
  assert(pcS2);
  const char *pcS1_curr;
  const char *pcS2_curr;
  pcS1_curr = pcS1;
  pcS2_curr = pcS2;

  while (1)
  {
    if ((*pcS1_curr) < (*pcS2_curr))
      return -1;
    else if ((*pcS1_curr) > (*pcS2_curr))
      return 1;
    else if ((*pcS1_curr == '\0') && (*pcS2_curr == '\0'))
      return 0;

    pcS1_curr++;
    pcS2_curr++;
  }
}

/*------------------------------------------------------------------------*/
char *StrFindChr(const char* pcHaystack, int c)
{
  /* TODO: fill this function */
  assert(pcHaystack);
  const char *pcHaystack_curr;
  pcHaystack_curr = pcHaystack;

  while (1)
  {
    if (*pcHaystack_curr == c)
      return (char*)pcHaystack_curr;
    else if (*pcHaystack_curr == '\0')
      return NULL;

    pcHaystack_curr++;
  }    
}


/*------------------------------------------------------------------------*/
char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
  /* TODO: fill this function */
  assert(pcHaystack);
  assert(pcNeedle);
  const char *pcStrt;
  const char *pcNeedle_curr;
  const  char *pcHaystack_curr;
  pcHaystack_curr = pcHaystack;

  if (*pcNeedle == '\0')
    return (char*)pcHaystack;
    
  while (1)
  {
    if (*pcHaystack_curr == *pcNeedle)
    { 
      pcStrt = pcHaystack_curr;
      pcNeedle_curr = pcNeedle;

      while (1)
      {
        if (*pcNeedle_curr == '\0')
          return (char*)pcStrt;
        else if ((*pcHaystack_curr) != (*pcNeedle_curr))
          break;

        pcHaystack_curr++;
        pcNeedle_curr++;
      }
    }

    else if (*pcHaystack_curr == '\0')
      return NULL;

    else
      pcHaystack_curr++;
  }    
}

/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
  /* TODO: fill this function */
  assert(pcDest);
  assert(pcSrc);
  char *pcStrt;
  pcStrt = pcDest + StrGetLength(pcDest);

  StrCopy(pcStrt, pcSrc);

  return pcDest;
}

/*------------------------------------------------------------------------*/
long int StrToLong(const char *nptr, char **endptr, int base)
{
  /* handle only when base is 10 */
  if (base != 10) return 0;
  
  /* TODO: fill this function */
  assert(nptr);
  const char *nptr_curr;

  long num_digit = 0; // number of digits in string
  long sign = 1; // sign of long integer value
  long result = 0; // long integer value

  /* assign *endptr if endptr not NULL */
  if ((endptr != NULL))
  {
    *endptr = NULL; // initiallize *endptr to NULL
    
    // check for invalid character
    nptr_curr = nptr;
    while ((*nptr_curr != '\0') && isspace(*nptr_curr))
      nptr_curr++;
    
    while (*nptr_curr != '\0')
    {
      if (!isdigit(*nptr_curr))
      {
        *endptr = (char*)nptr_curr;
        break;      
      }
      num_digit++;
      nptr_curr++;
    }

    // check if no digits at all
    if (num_digit == 0)
      *endptr = (char*)nptr;
  }


  /* convert string to long integer */
  nptr_curr = nptr;
  while ((*nptr_curr != '\0') && isspace(*nptr_curr))
    nptr_curr++;

  // get sign of long integer value
  if (*nptr_curr == '+')
  {
    sign = 1;
    nptr_curr++;
  }
  else if (*nptr_curr == '-') 
  {
    sign = -1;
    nptr_curr++;
  }

  // get the long integer value
  while (*nptr_curr != '\0')
  {
    if (!isdigit(*nptr_curr))
      break;

    long new_digit = sign*((long)(*nptr_curr -'0'));

    // check for overflow, underflow
    if ((sign == 1) && (IsOverflow(result, new_digit)))
      return LONG_MAX;
    else if ((sign == -1) && (IsUnderflow(result, new_digit)))
      return LONG_MIN;      
    else
      result = result*base + new_digit; // add new digit to result

    nptr_curr++;
  }

  return result;
}

/*------------------------------------------------------------------------*/
int StrCaseCompare(const char *pcS1, const char *pcS2)
{
  /* TODO: fill this function */
  assert(pcS1);
  assert(pcS2);
  const char *pcS1_curr;
  const char *pcS2_curr;
  pcS1_curr = pcS1;
  pcS2_curr = pcS2;

  int case_diff = ('a'-'A'); 

  while (1)
  {
    if ((isalpha(*pcS1_curr)) && (isalpha(*pcS2_curr)))
    {
      if ((*pcS1_curr == *pcS2_curr)
        || ((*pcS1_curr) == (*pcS2_curr + case_diff))
        || ((*pcS1_curr) == (*pcS2_curr - case_diff)))
      {
        pcS1_curr++;
        pcS2_curr++;
      }
      else if ((*pcS1_curr) < (*pcS2_curr))
        return -1;
      else if ((*pcS1_curr) > (*pcS2_curr))
        return 1;
    }
    
    else
    {
      if ((*pcS1_curr) < (*pcS2_curr))
        return -1;
      else if ((*pcS1_curr) > (*pcS2_curr))
        return 1;
      else if ((*pcS1_curr == '\0') && (*pcS2_curr == '\0'))
        return 0;
      else
      {
        pcS1_curr++;
        pcS2_curr++;
      }    
    }
  }
}

/*------------------------------------------------------------------------*/
int IsOverflow(long result, long new_digit)
{
  if (result > (LONG_MAX/10))
    return 1;
  else if (result == (LONG_MAX/10) && (new_digit > (LONG_MAX%10)))
    return 1;
  else
    return 0;
}

/*------------------------------------------------------------------------*/
int IsUnderflow(long result, long new_digit)
{
  if (result < (LONG_MIN/10))
    return 1;
  else if (result == (LONG_MIN/10) && (new_digit < (LONG_MIN%10)))
    return 1;
  else
    return 0;
}