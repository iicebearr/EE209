/*--------------------------------------------------------------------*/
/* ish.c
   - Original Author: Bob Dondero 
   - Co Authored by 20210013 Eungjoe Kang, 20210421 Ingyu Youn
   - Parses and executes the commmands inputted to stdin              */
/*--------------------------------------------------------------------*/
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


/* function declaration*/
static void shellHelper(const char *inLine, char **argv);

static void SIGQUIT_Handler_Quit(int iSig);
static void SIGQUIT_Handler_Alarm(int iSig);
static void SIGALRM_Handler(int iSig);

static void set_env(DynArray_T oTokens, char **argv);
static void unset_env(DynArray_T oTokens, char **argv);
static void cd_function(DynArray_T oTokens, char **argv);
static void run(DynArray_T oTokens, char **argv);
static void exit_function(DynArray_T oTokens, char **argv);

static int getPipeTokenIdx(DynArray_T oTokens);
static void oTokens_free(DynArray_T oTokens);

/*--------------------------------------------------------------------*/
/* int main(int argc, char **argv)
   - main function of the program
   - recieves lines from the user
   - runs the commands given by the user                              */
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


  /* buffer for shell command line */
  char acLine[MAX_LINE_SIZE + 2];

  /* get commands from the .ishrc file and execute */
  char oldpath[PATH_MAX]; // store original working directory
  if (getcwd(oldpath, sizeof(oldpath)) == NULL) {
    perror(argv[0]); // print getcwd error message
  }     

  char* homepath = getenv("HOME"); // change working directory to HOME
  if (chdir(homepath) != 0) {
    perror(argv[0]); // print chdir error message
  }

  FILE *fp = fopen(".ishrc","r");
  if(fp != NULL) { // if file .ishrc exists
    while (1) {
      if (fgets(acLine, MAX_LINE_SIZE, fp) == NULL) { // if no more command
        break; // go get the command from stdin
      }

      // andd '\n' at the end of the last command
      int cmd_len = strlen(acLine);
      if (acLine[cmd_len-1] != '\n') {
        acLine[cmd_len] = '\n'; 
        acLine[cmd_len+1] = '\0';
      }

      fprintf(stdout, "%% %s", acLine); // print command line
      fflush(stdout);
      shellHelper(acLine, argv); // execute command
    }
    fclose(fp);
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
   - inputted inline, a char pointer to the command line
   - lexically analyzes and syntatically analyzes the line
   - executes the command in the command line                         */
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
        // if command is a builtin function, call corresponding function
        // if not, run the program                                    
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
            cd_function(oTokens, argv);
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
   - signal handler for SIGQUIT
   - terminates process                                               */
/*--------------------------------------------------------------------*/
static void SIGQUIT_Handler_Quit(int iSig) {
  exit(EXIT_SUCCESS);
}


/*--------------------------------------------------------------------*/
/* static void SIGQUIT_Handler_Alarm
   - signal handler for SIGQUIT
   - changes the signal handler for SIGQUIT to SIGQUIT_Handler_Quit
   - prints the message "Type Ctrl-\ again within 5 seconds to exit."
     to the standard output stream 
   - set an alarm of 5 seconds                                        */
/*--------------------------------------------------------------------*/
static void SIGQUIT_Handler_Alarm(int iSig) {
  printf("\nType Ctrl-\\ again within 5 seconds to exit.\n");

  /* change the action of SIGQUIT to terminate process */
  void (*pfRet)(int);
  pfRet = signal(SIGQUIT, SIGQUIT_Handler_Quit);
  assert(pfRet != SIG_ERR);

  /* set an alarm of 5 seconds */
  alarm(5);
}


/*--------------------------------------------------------------------*/
/* static void SIGALRM_Handler(int iSig)
   - signal handler for SIGALARM
   - changes the signal handler for SIGQUIT to SIGQUIT_Handler_Alarm  */
/*--------------------------------------------------------------------*/
static void SIGALRM_Handler(int iSig) {
    /* set signal handler for SIGQUIT */
    sigset_t mask, prev;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask, &prev); // block SIGQUIT signals

    void (*pfRet)(int);
    pfRet = signal(SIGQUIT, SIGQUIT_Handler_Alarm);
    assert(pfRet != SIG_ERR);

    sigprocmask(SIG_SETMASK, &prev, 0); // restore to previous mask
}


/*--------------------------------------------------------------------*/
/* static void set_env(DynArray_T oTokens, char **argv)
   - recieves oTokens and argument array as input
   - create enviroment variables according to oTokens
   - overwrite environment variable values if variables exist         */
/*--------------------------------------------------------------------*/
static void set_env(DynArray_T oTokens, char **argv) {
  if(DynArray_getLength(oTokens) != 2 && DynArray_getLength(oTokens) != 3 ) {
    fprintf(stderr, "%s: setenv takes one or two parameters\n", argv[0]);
    oTokens_free(oTokens);
    return;
  }

  char* name = ((struct Token *)DynArray_get(oTokens,1))->pcValue;
  char* value = ((struct Token *)DynArray_get(oTokens,2))->pcValue;

  setenv(name, value, 1);
  oTokens_free(oTokens);
}


/*--------------------------------------------------------------------*/
/* static void unset_env(DynArray_T oTokens, char **argv)
   - recieves oTokens and argument array as input
   - deletes the environment variable                                 */
/*--------------------------------------------------------------------*/
static void unset_env(DynArray_T oTokens, char **argv) {
  if(DynArray_getLength(oTokens) != 2) {
    fprintf(stderr, "%s: setenv takes one parameter\n", argv[0]);
    oTokens_free(oTokens);
    return;
  }

  char* name = ((struct Token *)DynArray_get(oTokens,1))->pcValue;

  unsetenv(name);
  oTokens_free(oTokens);
}


/*--------------------------------------------------------------------*/
/* static void cd_function(DynArray_T oTokens, char **argv)
   - recieves oTokens and argument array as input
   - changes the working directory to which specified by oTokens,
     or to the HOME directory if it is omitted                        */
/*--------------------------------------------------------------------*/
static void cd_function(DynArray_T oTokens, char **argv) {
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
    oTokens_free(oTokens);
    return;
  }

  /* change directory */ 
  if (chdir(path) != 0) {
    perror(argv[0]);
  }
  oTokens_free(oTokens);
}


/*--------------------------------------------------------------------*/
/* static void exit_function(DynArray_T oTokens, char** argv)
   - recieves oTokens and argument array as input
   - terminates program with exit status 0(EXIT_SUCCESS)              */
/*--------------------------------------------------------------------*/
static void exit_function(DynArray_T oTokens, char** argv) {
  if(DynArray_getLength(oTokens) != 1) {
    fprintf(stderr, "%s: exit does not take any parameters\n", argv[0]);
    oTokens_free(oTokens);
    return;
  }

  oTokens_free(oTokens);
  printf("\n");
  exit(EXIT_SUCCESS);
}


/*--------------------------------------------------------------------*/
/* static void run_no_pipe(DynArray_T oTokens, char **argv)
   - recieves oTokens and argument array as input
   - runs the command given by oTokens
   - for commands with no pipe                                        */
/*--------------------------------------------------------------------*/
static void run_no_pipe(DynArray_T oTokens, char **argv) {

  pid_t pid;
  int i;
  char *arguments[MAX_ARGS_CNT];

  fflush(NULL);
  if((pid = fork()) < 0) {
    perror(argv[0]); // print forking error message
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
        int fd;
        if((fd = open(arguments[i+1], O_RDONLY)) < 0) {
          perror(argv[0]);  // print open error message - no such file
          oTokens_free(oTokens);
          exit(EXIT_FAILURE);
        }
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
    if(execvp(arguments[0], arguments) < 0) {
      perror(arguments[0]);  // print execvp error message
      oTokens_free(oTokens);
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

    // free oTokens
    oTokens_free(oTokens);
    return;
  }
}

/*--------------------------------------------------------------------*/
/* static int getPipeTokenIdx(DynArray_T oTokens)
   - recieves oTokens as input
   - outputs index of the token that contains "|"
   - if no such token exists, outputs -1    */ 
/*--------------------------------------------------------------------*/
static int getPipeTokenIdx(DynArray_T oTokens) {
  int i = 0;
  while ((i < DynArray_getLength(oTokens))
    &&(((struct Token *)DynArray_get(oTokens, i))->eType != TOKEN_PIPE)) {
    i++;
  }
  
  if (i == DynArray_getLength(oTokens))
    return -1; // no pipe token
  else
    return i; // pipe token index
}

/*--------------------------------------------------------------------*/
/* static void run(DynArray_T oTokens, char **argv)
   - recieves oTokens and argument array as input
   - runs the command given by oTokens                                */
/*--------------------------------------------------------------------*/
static void run(DynArray_T oTokens, char **argv) {

  int i, j;
  int fds[2];

  i = getPipeTokenIdx(oTokens);
  if (i < 0) { // command contains no pipe
    run_no_pipe(oTokens, argv);
    return;
  }
  else { // command contains pipe
    // free token containing '|'
    freeToken((struct Token *)DynArray_get(oTokens, i), NULL);

    // divide oTokens for left/right command with respect to pipe
    DynArray_T oTokens_left = DynArray_new(0);
    DynArray_T oTokens_right = DynArray_new(0);

    for (j = 0; j < i; j++) {
      DynArray_add(oTokens_left, DynArray_get(oTokens, j));
    }
    for (j = i+1; j < DynArray_getLength(oTokens); j++) {
      DynArray_add(oTokens_right, DynArray_get(oTokens, j));
    }
    
    // free original dynamic array containing the tokens
    DynArray_free(oTokens);

    // create pipe
    pipe(fds);

    // redirect stdout
    int stdout_temp = dup(1);
    dup2(fds[1],1);
    close(fds[1]);
    
    // run left command
    run_no_pipe(oTokens_left, argv);

    // restore stdout
    dup2(stdout_temp, 1);
    close(stdout_temp); 

    // redirect stdin
    int stdin_temp = dup(0);
    dup2(fds[0],0);
    close(fds[0]);

    // run right command
    run(oTokens_right, argv);

    // restore stdin
    dup2(stdin_temp, 0);
    close(stdin_temp);

    return;    
  }
}


/*--------------------------------------------------------------------*/
/* static void oTokens_free(DynArray_T oTokens)

  inputted oTokens, frees all data associated with oToken 
 */
/*--------------------------------------------------------------------*/
static void oTokens_free(DynArray_T oTokens) {
  for (int i = 0; i < DynArray_getLength(oTokens); i++) {
    if(DynArray_get(oTokens, i) != NULL)
      freeToken(DynArray_get(oTokens, i), NULL);
  }
  DynArray_free(oTokens);
}
