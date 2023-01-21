/**
 * Simple shell interface starter kit program.
 * Operating System Concepts
 * Project 1
 */

#include <stdio.h>
#include <unistd.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int main(void)
{
    char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    int should_run = 1;

    while (should_run){
        printf("mysh:~$ ");
        fflush(stdout);

        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */
          should_run = 0;
    }

    return 0;
}

