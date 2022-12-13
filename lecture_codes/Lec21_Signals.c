#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

// signal: a notification of an event
// how signal is sent and handled 
//      1. Exception occurs(interrupt, trap, fault, abort)
//      2. Context switched to OS
//      3. "OS sends signal" to application process
//      4. When application process "regains CPU" it handles it
//      5. (Optionally) Application process resumes where it left off

// Ctrl-c: 2/SIGINT, terminate
// Ctrl-z: 20/SIGTSTP, terminal stop
// Ctrl-\: 3/SIGQUIT, terminate


/* Ctrl-c: 2/SIGINT */
// interrupt occurs
// context switches to OS
// OS sends 2/SIGINT to application program
// Default action is "terminate"


/* Ctrl-z: 20/SIGTSTP */
// Default action is “stop until next 18/SIGCONT”


/* Ctrl-\: 3/SIGQUIT */
// Default action is “terminate”


/* illegal memory reference: 11/SIGSEGV */ 
// Fault occurs
// Context switches to OS
// OS sends 11/SIGSEGV to application process
// Default action for is "terminate"


/* signal handling */
// A program can install a signal handler
// to change action of (almost) any signal type

// "9/SIGKILL and 19/SIGSTOP" are unchangeable
// 9/SIGKILL: Default action is “terminate”
// 19/SIGSTOP: Default action is “stop until next 18/SIGCONT”

// sighandler_t signal(int iSig, sighandler_t pfHandler)
static void myHandler(int iSig) {
    printf("In myHandler with argument %d\n", iSig);
}

// While executing a handler for a signal of type x,
// all signals of type x are blocked automatically

int main(int argc, char **argv)
{
    /* installing a signal handler */
    // sighandler_t signal(int iSig, sighandler_t pfHandler)
    //      Returns the old handler on success, SIG_ERR on error
    void (*pfRet)(int);
    pfRet = signal(SIGINT, myHandler);
    assert(pfRet != SIG_ERR);
    
    /* Signals via kill command */
    // $kill -signal pid
    // send signal to process with process ID pid
    // ex) $kill -2 1234. $kill -INT 1234

    // sends 15/SIGTERM signal if signal type not specified
    // Default action for 15/SIGTERM is “terminate”

    /* Signals via functions */
    // int raise(int iSig)
    //      Commands OS to send a signal of type iSig to "current" process
    //      Returns 0 to indicate success, non-0 to indicate failure
    int iRet_raise = raise(SIGINT);
    assert(iRet_raise == 0);

    // int kill(pid_t iPid, int iSig)
    //      Sends a iSig signal to the process whose id is iPid
    pid_t iPid = getpid();
    int iRet_kill = kill(iPid, SIGINT);
    assert(iRet_kill == 0);


    /* predefined signal handler: SIG_IGN, SIG_DFL */
    // SIG_IGN:  ignore signal
    signal(SIGINT, SIG_IGN);
    raise(SIGINT);

    // SIG_DFL: restore default action
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);


    /* signal blocking */
    // int sigprocmask(int iHow,
    //                 const sigset_t *psSet,
    //                 sigset_t *psOldSet);
    // psSet: Pointer to a signal set
    // iHow: SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK
    //       SIG_SETMASK installs psSet as signal mask
    // While executing a handler for a signal of type x,
    // all signals of type x are blocked automatically

}

