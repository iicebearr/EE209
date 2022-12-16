#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <linux/limits.h>

#include "lexsyn.h"
#include "util.h"


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

static void oTokenfree(DynArray_T oTokens);

/*--------------------------------------------------------------------*/
/* int main(int argc, char **argv)
*/
/*--------------------------------------------------------------------*/
int main(int argc, char **argv) {
  /* TODO */

  /* make sure SIGINT, SIGQUIT, and SIGALRM signals are not blocked */
  sigset_t sSet;
  sigemptyset(&sSet);
  sigaddset(&sSet, SIGINT);
  sigaddset(&sSet, SIGQUIT);
  sigaddset(&sSet, SIGALRM);
  sigprocmask(SIG_UNBLOCK, &sSet, NULL);

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

  /* get commands from the .ishrc file and execute */
  char oldpath[PATH_MAX]; // store original working directory
  if (getcwd(oldpath, sizeof(oldpath)) == NULL) {
    perror(argv[0]);
  }     

  char* homepath = getenv("HOME"); // change working directory to HOME
  if (chdir(homepath) != 0) {
    perror(argv[0]);
  }

  FILE *fp;
  if((fp = fopen(".ishrc","r")) != NULL) { // if file .ishrc exists
    while (1) {
      if (fgets(acLine, MAX_LINE_SIZE, fp) == NULL) { // if no more command
        break; // go get the command from stdin
      }
      
      int cmd_len = strlen(acLine);
      if (acLine[cmd_len-1] == '\n') {
        acLine[cmd_len-1] = '\0'; // get rid of '\n' at the end of each command
      }

      fprintf(stdout, "%% %s\n", acLine); // print command line
      fflush(stdout);
      shellHelper(acLine, argv); // execute command
    }
  } 

  if (chdir(oldpath) != 0) { // change working directory back to original
    perror(argv[0]);
  }

  /* get the command from stdin and execute */
  while (1) {
    fprintf(stdout, "%% ");
    fflush(stdout);
    if (fgets(acLine, MAX_LINE_SIZE, stdin) == NULL) { // if input is NULL
      printf("\n");
      exit(EXIT_SUCCESS); // exit
    }
    shellHelper(acLine, argv); // execute command
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

  errorPrint(argv[0], SETUP); // set ishname

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
          default:
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
    assert(pfRet != SIG_ERR);
}



/*--------------------------------------------------------------------*/
/* static void set_env(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void set_env(DynArray_T oTokens, char **argv) {
  if(DynArray_getLength(oTokens) != 2 && DynArray_getLength(oTokens) != 3 ) {
    fprintf(stderr, "%s: setenv takes one or two parameters\n", argv[0]);
    oTokenfree(oTokens);
    return;
  }

  char* name = ((struct Token *)DynArray_get(oTokens,1))->pcValue;
  char* value = ((struct Token *)DynArray_get(oTokens,2))->pcValue;

  setenv(name, value, 1);
  oTokenfree(oTokens);
}


/*--------------------------------------------------------------------*/
/* static void unset_env(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void unset_env(DynArray_T oTokens, char **argv) {
  if(DynArray_getLength(oTokens) != 2) {
    fprintf(stderr, "%s: setenv takes one parameter\n", argv[0]);
    oTokenfree(oTokens);
    return;
  }

  char* name = ((struct Token *)DynArray_get(oTokens,1))->pcValue;

  unsetenv(name);
  oTokenfree(oTokens);
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
    oTokenfree(oTokens);
    return;
  }

  /* change directory */ 
  if (chdir(path) != 0) {
    perror(argv[0]);
  }
  oTokenfree(oTokens);
}


/*--------------------------------------------------------------------*/
/* static void exit_function(DynArray_T oTokens, char** argv)
*/
/*--------------------------------------------------------------------*/
static void exit_function(DynArray_T oTokens, char** argv) {
  if(DynArray_getLength(oTokens) != 1) {
    fprintf(stderr, "%s: exit does not take any parameters\n", argv[0]);
    oTokenfree(oTokens);
    return;
  }

  oTokenfree(oTokens);
  printf("\n");
  exit(EXIT_SUCCESS);
}


/*--------------------------------------------------------------------*/
/* static void run(DynArray_T oTokens, char **argv)
*/
/*--------------------------------------------------------------------*/
static void run(DynArray_T oTokens, char **argv) {

  pid_t pid;
  int i = 0, j;
  int fds[2];
  char *arguments[MAX_ARGS_CNT];

  /* pipe */
  while ((i < DynArray_getLength(oTokens))
       &&(((struct Token *)DynArray_get(oTokens, i))->eType != TOKEN_PIPE)) {
    i++;
  }

  if (i < DynArray_getLength(oTokens)) {
    DynArray_T oTokens_left = DynArray_new(0);
    DynArray_T oTokens_right = DynArray_new(0);

    // make array of arguments for left/right command
    for (j = 0; j < i; j++) {
      DynArray_add(oTokens_left, DynArray_get(oTokens, j));
    }
    for (j = i+1; j < DynArray_getLength(oTokens); j++) {
      DynArray_add(oTokens_right, DynArray_get(oTokens, j));
    }

    // pipe
    pipe(fds);

    int id = fork();
    if (id == 0) { /* is child */
      close(fds[0]);
      dup2(fds[1],1);
      close(fds[1]);
      run(oTokens_left, argv); // run left command
      exit(EXIT_SUCCESS);
    }
    else { /* is parent */
      waitpid(id, NULL, 0);
      close(fds[1]);
      dup2(fds[0],0);
      close(fds[0]);
      //fprintf(stderr, "d - running o_right\n");
      run(oTokens_right, argv); // run right command
      //fprintf(stderr, "right run success\n");
      return;
    }

    //fprintf(stderr, "e\n");
  }


  /* base case where there is no pipe */

  /* fork child process*/
  if((pid = fork()) < 0) {
    /* print forking error message */
    perror(argv[0]);
    return;
  }
  else if(pid == 0) { /* is child */
    /* restore default action of SIGINT & SIGQUIT */
    void (*pfRet)(int);
    pfRet = signal(SIGINT, SIG_DFL);
    assert(pfRet != SIG_ERR);
    pfRet = signal(SIGQUIT, SIG_DFL);
    assert(pfRet != SIG_ERR);

    /* make an array of arguments*/
    for (i = 0; i < DynArray_getLength(oTokens); i++) {
      arguments[i] = ((struct Token *)DynArray_get(oTokens, i))->pcValue;
    }
    arguments[i] = NULL;

    /* redirection */
    for (i = 0; i < DynArray_getLength(oTokens); i++) {
      // stdin redirection
      if (((struct Token *)DynArray_get(oTokens, i))->eType == TOKEN_REDIN) {
        int fd = open(arguments[i+1], O_RDONLY);
        close(0);
        dup(fd);
        close(fd);
      } 
      // stdout redirection
      if (((struct Token *)DynArray_get(oTokens, i))->eType == TOKEN_REDOUT) {
        int fd = open(arguments[i+1], O_RDWR | O_CREAT | O_TRUNC, 0600);
        close(1);
        dup(fd);
        close(fd);
      } 
    }

    /* invoke program */
    //fprintf(stderr, "argument is: %s\n", arguments[0]);
    if(execvp(arguments[0], arguments) < 0) {
      perror(arguments[0]);
      oTokenfree(oTokens);
      exit(EXIT_FAILURE);
    }
  }
  else { /* is parent */
    waitpid(pid, NULL, 0);

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

    oTokenfree(oTokens);
    return;
  }
}

/*--------------------------------------------------------------------*/
/* static void oTokenfree(DynArray_T oTokens)
*/
/*--------------------------------------------------------------------*/
static void oTokenfree(DynArray_T oTokens) {
  for (int i = 0; i < DynArray_getLength(oTokens); i++) {
    freeToken(DynArray_get(oTokens, i), NULL);
  }
  DynArray_free(oTokens);
}