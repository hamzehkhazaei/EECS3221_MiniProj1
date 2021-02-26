#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

// Simple shell program
// Author: Amer Alshoghri (214992291)
// While all the code written is my own, I verbally collaborated with student Julia Paglia for this assignment as well as used StackOverFlow for debugging purposes

// Constants
#define MAX_LINE 80 /* The maximum length command */

// Prototypes:
void ParentProcess(int pid, int w);
void ChildProcess(char *command, char **args, char *line, char *last_line, char **last_args);
char *read_line(void);
void split_args(char *line, char **args);
void detect_remove_amp(char *line, int *w);
void change_directory(char **args);
int args_reroute(char **args, int *file_desc);
void detect_remove_reroute(char *line);


// Main Routine
int main(void) {
    int should_run = 1;                 // Flag to determine when to exit program  
    char *args[MAX_LINE/2 + 1];         // Command line arguments 
    char *line;                         // String representation of the input argument
    char *last_line;                    // String representation of input arg used for history
    int pid;                            // PID of forked process
    int w = 0;                          // Should the parent process wait? 0=> yes, 1=> no
    char *last_args[MAX_LINE/2 + 1];    // Last arguments used for history
    int file_desc = -1;                 // File descriptor placeholder
    int reroute = -1;                   // Is there a reroute? 1=> stdout, 0=>stdin
    

    // Main loop
    while (should_run){

        // Save stdin / stdout as file descriptors in case a redirection is needed
        int saved_stdin = dup(0);
        int saved_stdout = dup(1);

        // Parent should wait for child
        // if previous iteration did not set w to 1
        // if (w == 0){
        //     wait(NULL);
        // }
        
        // Print shell prompt and wait for input
        // Ensure stdin/stdout are flushed before prompting user
        printf("mysh:~$ ");
        fflush(stdout);
        fflush(stdin);
        line = read_line();

        // Check if there exists an '&' in args array
        // If there is, set wait variable 'w' to 1 (Don't wait)
        if ( strchr(line, '&') != NULL )
        {
            detect_remove_amp(line, &w);
        } else 
        {
            w = 0;
        }
        
        // Parse arguments from string into array args
        split_args(line, args);

        // Skip iteration if args contains nothing
        if (args[0] == NULL)
        {
            continue;
        }

        // Parse for a > or < and reroute i/o if needed
        reroute = args_reroute(args, &file_desc);

        // If reroute procedure was successful - 
        // adjust argument line to remove  i.e. " <..." and re-parse arg line
        if (reroute >= 0)
        {
            detect_remove_reroute(line);
            split_args(line, args);
        }

        // Handle cd, exit commands (as child can't)
        if (strcmp(args[0], "cd") == 0)
        {
            change_directory(args);
            continue;
        } else if (strcmp(args[0], "exit") == 0)
        {
            fprintf(stderr, "User exiting shell...\n");
            exit(0);
        }

        //flush any output then fork processes
        fflush(stdout);
        fflush(stdin);
        pid = fork();

        // Child Process
        if (pid == 0){
            if (w == 1) setpgid(0, 0);
            ChildProcess(args[0], args, line, last_line, last_args);
            fflush(stdout);
            should_run = 0; //Child Process Completed. Stop running.
            exit(1);

        // Parent process
        } else if (pid > 0){
            // Run parent process
            ParentProcess(pid, w);
            // Save last line in memory for history feature
            if (sizeof(args) != 0 && sizeof(line) != 0 && (strcmp(line, "!!\n") != 0))
            {
                last_line = (char*) malloc(MAX_LINE);
                memcpy(last_args, args, MAX_LINE);
                memcpy(last_line, line, MAX_LINE);
            }
 
        // Attempted fork failed - exit
        } else 
        {
            should_run = 0;
            exit(1);
        }

        // Close file descriptor in use, if any
        // Restore stdin/stdout if reroute was used
        fflush(stdout);
        fflush(stdin);
        dup2(saved_stdout, 1); close(saved_stdout);
        dup2(saved_stdin, 0); close(saved_stdin);
        fflush(stdout);
        fflush(stdin);

    }
    return 0;
}

// Reroute I/O based on args array
// TODO, file_Desc is not needed here
int args_reroute(char **args, int *file_desc)
{
    int i = 0;
    int d;

    while (args && args[i] != NULL)
    {
        if (strcmp(args[i], ">") == 0)
        {
            //printf("Writing stdout to new file %s\n", args[i+1]);
            *file_desc = open(args[i+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            d = dup2(*file_desc, 1);
            close(*file_desc);

            if (d < 0) {printf("Error with dup2 %d \n", d);}
            return 1;

        } else if (strcmp(args[i], "<") == 0)
        {
            //printf("Reading stdin from new file %s\n", args[i+1]);
            *file_desc = open(args[i+1], O_RDONLY);
            d = dup2(*file_desc, 0);
            close(*file_desc);

            if (d < 0) {printf("Error with dup2 %d \n", d);}
            return 0;
        }
        i++;
    }
    return -1;
}

// Change the current working directory using args array
void change_directory(char **args)
{
    int r = -1;

    if (strcmp(args[0], "cd") == 0 && args[1] == NULL)
    {
        r = chdir(getenv("HOME"));
    } else 
    {
        //printf("changing dir to %s\n", args[1]);
        r = chdir(args[1]);
    }
    if (r < 0)
    {
        printf("Error: could not change directory to %s", args[1]);
    }
}

// Delete '&' occurrences from a string, and change w to 1 if a '&' was found
void detect_remove_amp(char *line, int *w)
{
    char *start, *finish;
    start = line;
    finish = line;

    // Set wait to default value (0 => parent should wait)
    *w = 0;

    // Set the 1st pointer to the 2nd pointer
    // If the 2nd pointer = '&' do not move it forward
    // Next iteration, the 1st pointer will skip it
    while(*start != '\0')
    {
        *start = *finish;
        if (*finish != '&')
        {
            finish++;
        } else 
        {
            //printf("Found an &\n");
            *w = 1;
        }
        *start++;
    }
    *finish = '\0';
}

// Delete everything in a string including < or > and onwards
void detect_remove_reroute(char *line)
{
    char *start, *finish;
    start = line;
    finish = line;
    int found = 0;

    // Set the 1st pointer to the 2nd pointer
    // If the 2nd pointer = '>' do not move it forward
    // In every following iteration, the start iteration will move only.
    while(*start != '\0')
    {
        *start = *finish;
        
        // stdout case
        if (*finish != '>' && *finish != '<' && found == 0)
        { 
            finish++;
        }else 
        {   
            found = 1;
        }
        *start++;
    }
    *finish = '\0';
}

// Child process method
void ChildProcess(char *command, char **args, char *line, char *last_line, char **last_args)
{
    int r = -1;

    // Handle !! history command
    if (strcmp(command, "!!") == 0)
    {
        if (last_line == NULL || sizeof(last_args) == 0)
        {
            printf("No commands in history\n");
        } else
        {
            printf("%s\n", last_line);
            //execvp(last_args[0], last_args);
            memcpy(args, last_args, MAX_LINE);
            memcpy(line, last_line, MAX_LINE);
        }
    }
    // PIPE PROCEDURE
    if ( strchr(line, '|') != NULL )
    {
        char *pipe_in_args[MAX_LINE / 2 + 1];  
        char *pipe_out_args[MAX_LINE / 2 + 1];
        char *pipe_in = strdup(line);
        char *pipe_out;
        int pipe_arr[2];
        int pid;

        // Token will point to first element of pipe (STDIN)
        pipe_in = strtok(line, "|");

        // Token will point to last element of pipe (STDOUT)
        pipe_out = strtok(NULL, "|");

        // Create pipe
        if (pipe(pipe_arr) == -1)
        {
            printf("Pipe Failed to create\n");
            exit(1);
        }

        // Fork
        pid = fork();

        // Grandchild process:
        if (pid == 0)
        {
            close(1);           // Close stdout
            dup(pipe_arr[1]);   // Pipe write to pipe_arr[1] instead of stdout
            close(pipe_arr[0]); // Close both indices in pipe array
            close(pipe_arr[1]);

            // Execute left part of command and exit
            split_args(pipe_in, pipe_in_args);
            r = execvp(pipe_in_args[0], pipe_in_args);
            if (r < 0) printf("ERROR: Invalid command before pipe\n");
            exit(0);
        }

        //Create second grandchild
        pid = fork();

        if (pid == 0)
        {
            close(0);           // Close stdin
            dup(pipe_arr[0]);   // Pipe read from pipe_arr[0] instead of stdin
            close(pipe_arr[0]); // Close both pipe_arr indices
            close(pipe_arr[1]);

            // Execute right part of command and exit
            split_args(pipe_out, pipe_out_args);
            r = execvp(pipe_out_args[0], pipe_out_args);
            if (r < 0) printf("ERROR: Invalid command after pipe\n");
            exit(0);
        }

        // Parent process closing pipe file descriptors
        close(pipe_arr[0]);
        close(pipe_arr[1]);

        // Wait for both children to exit
        wait(NULL);
        wait(NULL);

    // Handle regular command
    } else 
    {
        r = execvp(args[0], args);
        fflush(stdout);

        if (r < 0)
        {
            fprintf(stderr, "ERROR: Invalid command returned code %d\n", r);
            exit(1);
        }
    }  
}

// Parent Process method
void ParentProcess(int pid, int w){
    if (w == 0){
        //printf("Parent Waiting!\n");
        int *status;
        waitpid(pid, status, 0);
    }
}

// Read a line from STDIN
char *read_line(void){
    char *myline = NULL; 
    ssize_t buffer = MAX_LINE * 4; // 80 chars * 4 bits per char
    getline(&myline, &buffer, stdin);
    return myline;
}

// Parses input line string into a string array args
void split_args(char *input_line, char **args)
{
    char *line = (char*) malloc(MAX_LINE);
    memcpy(line, input_line, MAX_LINE);
    //printf("New line: <%s>\n", line);
    // Read pointer until EOL
     while (*line != '\0')
     { 
        // Iterate pointer through any whitespace characters
        while (*line == ' ' || *line == '\n')
        {
            *line = '\0';     /* replace white spaces with null char*/
            *line++;
        }
            
        if (*line) /* pointer goes to beginning of command => add to args */
        {
            *args = line;
            *args++;
        }

        while (*line != '\0' && *line != ' ' && *line != '\n')
        {
            line++;    /* Move memory address forward until whitespace reached again */
        }
    }
    *args = '\0';                 /* place EOL in argument array */
}