/* mshell - a job manager */

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jobs.h"

#define BOLD "\033[00;01m"
#define NORM "\033[00;00m"

void do_help() {
    printf("available commands are:\n");
    printf(" exit - cause the shell to exit\n");
    printf(BOLD "\t exit\n" NORM);
    printf(" jobs - display status of jobs in the current session\n");
    printf(BOLD "\t jobs\n" NORM);
    printf(" fg   - run a job identified by its pid or job id in the foreground\n");
    printf(BOLD "\t fg " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" bg   - run a job identified by its pid or job id in the background\n");
    printf(BOLD "\t bg " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" stop - stop a job identified by its pid or job id\n");
    printf(BOLD "\t stop " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" kill - kill a job identified by its pid or job id\n");
    printf(BOLD "\t kill " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" help - print this message\n");
    printf(BOLD "\t help\n" NORM);
    printf("\n");
    printf("ctrl-z and ctrl-c can be used to send a SIGTSTP and a SIGINT, respectively\n\n");
}

/* treat_argv - Determine pid or jobid and return the associated job structure */
struct job_t *treat_argv(char **argv) {
    struct job_t *jobp = NULL;

    /* Ignore command if no argument */
    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return NULL;
    }

    /* Parse the required PID or %JID arg */
    if (isdigit((int) argv[1][0])) {
        pid_t pid = atoi(argv[1]);
        if (!(jobp = jobs_getjobpid(pid))) {
            printf("(%d): No such process\n", (int) pid);
            return NULL;
        }
    } else if (argv[1][0] == '%') {
        int jid = atoi(&argv[1][1]);
        if (!(jobp = jobs_getjobjid(jid))) {
            printf("%s: No such job\n", argv[1]);
            return NULL;
        }
    } else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return NULL;
    }

    return jobp;
}


/* do_bg - Execute the builtin bg command */
void do_bg(char **argv) {
    struct job_t *j;

    if (verbose)
        printf("do_bg : entering\n");

    j = treat_argv(argv);
    j->jb_state = BG;
    kill(-(j->jb_pid), SIGCONT);

    if (verbose)
        printf("do_bg : exiting\n");

    return;
}

/* waitfg - Block until process pid is no longer the foreground process */
void waitfg(pid_t pid) {
    while(jobs_fgpid() == pid)
        sleep(1);

    return;
}

/* do_fg - Execute the builtin fg command */
void do_fg(char **argv) {
    struct job_t *j;

    if (verbose)
        printf("do_fg : entering\n");

    j = treat_argv(argv);
    j->jb_state = FG;
    kill(-(j->jb_pid), SIGCONT);

    waitfg(j->jb_pid);

    if (verbose)
        printf("do_fg : exiting\n");

    return;
}

/* do_stop - Execute the builtin stop command */
void do_stop(char **argv) {
    struct job_t *j;

    if (verbose)
        printf("do_fg : entering\n");

    j = treat_argv(argv);
    j->jb_state = ST;
    kill(-(j->jb_pid), SIGSTOP);

    if (verbose)
        printf("do_fg : exiting\n");

    return;
}

/* do_kill - Execute the builtin kill command */
void do_kill(char **argv) {
    struct job_t *j;

    if (verbose)
        printf("do_fg : entering\n");

    j = treat_argv(argv);
    kill(-(j->jb_pid), SIGKILL);

    if (verbose)
        printf("do_fg : exiting\n");

    return;
}

/* do_exit - Execute the builtin exit command */
void do_exit() {

    if (verbose)
        printf("do_fg : entering\n");

    exit(EXIT_SUCCESS);

    if (verbose)
        printf("do_fg : exiting\n");

    return;
}

/* do_jobs - Execute the builtin fg command */
void do_jobs() {
   if (verbose)
        printf("do_fg : entering\n");

    jobs_listjobs();

    if (verbose)
        printf("do_fg : exiting\n");

    return;
}
