#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

#include "lexsyn.h"
#include "util.h"
// #include "token.h"
// #include "dynarray.h"

/* testing thlhj fldhshjhbgajk  */


/*--------------------------------------------------------------------*/
/* ish.c                                                              */
/* Original Author: Bob Dondero                                       */
/* Modified by : Park Ilwoo                                           */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/*--------------------------------------------------------------------*/


/* function declaration*/
static void shellHelper(const char *inLine, char **argv);

static void SIGQUIT_Handler_Quit(int iSig);
static void SIGQUIT_Handler_Alarm(int iSig);
static void SIGALRM_Handler(int iSig);

static void set_env(DynArray_T oTokens, char **argv);
static void unset_env(DynArray_T oTokens, char **argv);
static void cd(DynArray_T oTokens, char **argv);
static void run(DynArray_T oTokens, char **argv);
static void exit_function(DynArray_T oTokens, char **argv);


/*--------------------------------------------------------------------*/
/* int main(int argc, char **argv)
*/
/*--------------------------------------------------------------------*/
int main(int argc, char **argv) {
  /* TODO */

  /* ignore SIGINT */
  void (*pfRet)(int);
  pfRet = signal(SIGINT, SIG_IGN);
  assert(pfRet != SIG_ERR);

  /* set signal handler for SIGQUIT */
  pfRet = signal(SIGQUIT, SIGQUIT_Handler_Alarm);
  assert(pfRet != SIG_ERR);

  /* set signal handler for SIGALRM */
  pfRet = signal(SIGALRM, SIGALRM_Handler);
  assert(pfRet != SIG_ERR);

  /* shell */
  char acLine[MAX_LINE_SIZE + 2];
  while (1) {
    /* get the command */
    fprintf(stdout, "%% ");
    fflush(stdout);
    if (fgets(acLine, MAX_LINE_SIZE, stdin) == NULL) {
      printf("\n");
      exit(EXIT_SUCCESS);
    }

    /* execute command */
    shellHelper(acLine, argv);
  }
}


/*--------------------------------------------------------------------*/
/* static void shellHelper(const char *inLine, char **argv)
*/
/*--------------------------------------------------------------------*/
static void shellHelper(const char *inLine, char **argv) {
  DynArray_T oTokens;

  enum LexResult lexcheck;
  enum SyntaxResult syncheck;
  enum BuiltinType btype;

  oTokens = DynArray_new(0);
  if (oTokens == NULL) {
    errorPrint("Cannot allocate memory", FPRINTF);
    exit(EXIT_FAILURE);
  }


  lexcheck = lexLine(inLine, oTokens);
  switch (lexcheck) {
    case LEX_SUCCESS:
      if (DynArray_getLength(oTokens) == 0)
        return;

      /* dump lex result when DEBUG is set */
      dumpLex(oTokens);

      syncheck = syntaxCheck(oTokens);
      if (syncheck == SYN_SUCCESS) {
        btype = checkBuiltin(DynArray_get(oTokens, 0));
        /* TODO */
        switch(btype) {
          case B_SETENV:
            set_env(oTokens, argv);
            break;
          case B_USETENV:
            unset_env(oTokens, argv);
            break;
          case B_CD:
            cd(oTokens, argv);
            break;
          case B_EXIT:
            exit_function(oTokens, argv);
            break;
          case NORMAL:
            run(oTokens, argv);  // run the program
            break;
        }
      }

      /* syntax error cases */
      else if (syncheck == SYN_FAIL_NOCMD)
        errorPrint("Missing command name", FPRINTF);
      else if (syncheck == SYN_FAIL_MULTREDOUT)
        errorPrint("Multiple redirection of standard out", FPRINTF);
      else if (syncheck == SYN_FAIL_NODESTOUT)
        errorPrint("Standard output redirection without file name", FPRINTF);
      else if (syncheck == SYN_FAIL_MULTREDIN)
        errorPrint("Multiple redirection of standard input", FPRINTF);
      else if (syncheck == SYN_FAIL_NODESTIN)
        errorPrint("Standard input redirection without file name", FPRINTF);
      else if (syncheck == SYN_FAIL_INVALIDBG)
        errorPrint("Invalid use of background", FPRINTF);
      break;

    case LEX_QERROR:
      errorPrint("Unmatched quote", FPRINTF);
      break;

    case LEX_NOMEM:
      errorPrint("Cannot allocate memory", FPRINTF);
      break;

    case LEX_LONG:
      errorPrint("Command is too large", FPRINTF);
      break;

    default:
      errorPrint("lexLine needs to be fixed", FPRINTF);
      exit(EXIT_FAILURE);
  }
}


/*--------------------------------------------------------------------*/
/* static void SIGQUIT_Handler_Quit(int iSig)
*/
/*--------------------------------------------------------------------*/
static void SIGQUIT_Handler_Quit(int iSig) {
  exit(EXIT_SUCCESS);
}


/*--------------------------------------------------------------------*/
/* static void SIGQUIT_Handler_Alarm
*/
/*--------------------------------------------------------------------*/
static void SIGQUIT_Handler_Alarm(int iSig) {
  printf("\nType Ctrl-\\ again within 5 seconds to exit.\n");

  /* restore default action of SIGQUIT */
  void (*pfRet)(int);
  pfRet = signal(SIGQUIT, SIGQUIT_Handler_Quit);
  assert(pfRet != SIG_ERR);

  /* set an alarm of 5 seconds */
  alarm(5);
}


/*--------------------------------------------------------------------*/
/* static void SIGALRM_Handler(int iSig)
*/
/*--------------------------------------------------------------------*/
static void SIGALRM_Handler(int iSig) {
    /* set signal handler for SIGQUIT */
    void (*pfRet)(int);
    pfRet = signal(SIGQUIT, SIGQUIT_Handler_Alarm);
    assert(pfRet != SIG_ERR);}



/*--------------------------------------------------------------------*/
/* static void set_env(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void set_env(DynArray_T oTokens, char **argv) {
  if(DynArray_getLength(oTokens) != 2 && DynArray_getLength(oTokens) != 3 ) {
    fprintf(stderr, "%s: setenv takes one or two parameters\n", argv[0]);
    return;
  }

  char* name = ((struct Token *)DynArray_get(oTokens,1))->pcValue;
  char* value = ((struct Token *)DynArray_get(oTokens,2))->pcValue;

  DynArray_free(oTokens);
  setenv(name, value, 0);
}


/*--------------------------------------------------------------------*/
/* static void unset_env(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void unset_env(DynArray_T oTokens, char **argv) {
  if(DynArray_getLength(oTokens) != 2) {
    fprintf(stderr, "%s: setenv takes one parameter\n", argv[0]);
    return;
  }

  char* name = ((struct Token *)DynArray_get(oTokens,1))->pcValue;

  DynArray_free(oTokens);
  unsetenv(name);
}


/*--------------------------------------------------------------------*/
/* static void cd(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void cd(DynArray_T oTokens, char **argv) {
  char *path;

  /* get the path */
  if (DynArray_getLength(oTokens) == 1) {
    path = getenv("HOME");
  }
  else if (DynArray_getLength(oTokens) == 2) {
    path = ((struct Token *)DynArray_get(oTokens, 1))->pcValue;
  }
  else{
    fprintf(stderr, "%s: cd takes one parameter\n", argv[0]);
    DynArray_free(oTokens);
    return;
  }

  /* change directory */ 
  DynArray_free(oTokens);
  if (chdir(path) != 0) {
    perror(argv[0]);
  }
}


/*--------------------------------------------------------------------*/
/* static void exit_function(DynArray_T oTokens, char** argv)
*/
/*--------------------------------------------------------------------*/
static void exit_function(DynArray_T oTokens, char** argv) {
  if(DynArray_getLength(oTokens) != 1) {
    fprintf(stderr, "%s: exit does not take any parameters\n", argv[0]);
    return;
  }

  DynArray_free(oTokens);
  exit(EXIT_SUCCESS);
}


/*--------------------------------------------------------------------*/
/* static void run(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void run(DynArray_T oTokens, char **argv) {
  pid_t pid;
  int i;
  char *arguments[MAX_ARGS_CNT];
  
  /* make an array of arguments*/
  for (i = 0; i < DynArray_getLength(oTokens); i++) {
    arguments[i] = ((struct Token *)DynArray_get(oTokens, i))->pcValue;
  }
  arguments[i] = NULL;

  /* fork child process*/
  if((pid = fork()) < 0) {
    /* print forking error message */
    perror(argv[0]);
  }
  else if(pid == 0) { /* is child */
    /* restore default action of SIGINT & SIGQUIT */
    void (*pfRet)(int);
    pfRet = signal(SIGINT, SIG_DFL);
    assert(pfRet != SIG_ERR);
    pfRet = signal(SIGQUIT, SIG_DFL);
    assert(pfRet != SIG_ERR);

    /* invoke new program */
    if(execvp(arguments[0], arguments) < 0) {
      perror(arguments[0]);
      exit(EXIT_FAILURE);
    }
  }
  else { /* is parent */
    wait(NULL);

    /* ignore SIGINT */
    void (*pfRet)(int);
    pfRet = signal(SIGINT, SIG_IGN);
    assert(pfRet != SIG_ERR);

    /* set signal handler for SIGQUIT */
    pfRet = signal(SIGQUIT, SIGQUIT_Handler_Alarm);
    assert(pfRet != SIG_ERR);

    /* set signal handler for SIGALRM */
    pfRet = signal(SIGALRM, SIGALRM_Handler);
    assert(pfRet != SIG_ERR);

    DynArray_free(oTokens);
  }
}