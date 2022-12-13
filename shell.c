#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[1024];
char delimiters[] = " \t\r\n=|";
extern char **environ;
pid_t child_pid;
void exitSignalHandler(int sigint) {
    printf("\n");
    kill(child_pid, SIGKILL);
}
void timerHandler(int signum) {
  kill(child_pid, SIGKILL);
}

int main() {
    // Stores the string typed into the command line.
char command_line[MAX_COMMAND_LINE_LEN];

// Stores the tokenized command line input.
char *arguments[MAX_COMMAND_LINE_ARGS];

// Loop indefinitely until the user exits the shell.
while (true) {
  // Print the shell prompt.
  printf("%s>", prompt);
  fflush(stdout);

  // Read input from stdin and store it in command_line. If there's an error,
  // exit immediately.
  if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
    fprintf(stderr, "fgets error");
    exit(0);
  }

  // If the user input was EOF (ctrl+d), exit the shell.
  if (feof(stdin)) {
    printf("\n");
    fflush(stdout);
    fflush(stderr);
    return 0;
  }

  // Tokenize the command line input into the arguments array.
  char *token = strtok(command_line, delimiters);
  int i = 0;
  while (token != NULL) {
    arguments[i] = token;
    i++;
    token = strtok(NULL, delimiters);
  }

  // Check the first argument to determine the command to execute.
 

 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
      
      int argc = 0;
      arguments[0] = strtok(command_line, delimiters);
      while(arguments[argc] != NULL) {
         argc++;
        arguments[argc] = strtok(NULL, delimiters);
        }
			  // 0. Modify the prompt to print the current working directory
			  
			if (strcmp(arguments[0], "pwd") == 0){
        printf("%s\n", prompt);
      }
      
      else if (strcmp(arguments[0], "env") == 0){
        int i = 0;
        while(environ[i] != NULL) {
          printf("%s\n",environ[i]);
          i++;
        }
       
      }
      
      else if (strcmp(arguments[0], "echo") == 0){
        int i = 1;
        while(arguments[i] != NULL) {
          if(*arguments[i] == '$') {
            printf("%s\n", getenv((arguments[i] + 1)));
          }
          else {
            printf("%s\n", arguments[i]);
          }
          i++;
        }
      }
      
      else if (strcmp(arguments[0], "cd") == 0){
        chdir(arguments[1]);
      }
      
      else if (strcmp(arguments[0], "setenv") == 0){
        if (setenv(arguments[1], arguments[2], 1) != 0) {
          perror("Error");
        };
      }
      
      else if (strcmp(arguments[0], "exit") == 0){
        exit(0);
      }
      
      else
      {
        int status;
        pid_t wpid;
        int pipefd[2];
        int background_process = 0;
        int pipeProcess = 0;
        if (strcmp(arguments[argc - 1], "&") == 0){
            arguments[argc - 1] = NULL;
            background_process = 1;
          }
         if (strcmp(arguments[0], "cat") == 0){
          pipeProcess = 1;
          pipe(pipefd);
        }

        child_pid = fork();
        if (child_pid < 0) {
          perror("Error");
        }
        
        else if (child_pid == 0) {
          signal(SIGINT, exitSignalHandler);
          if (pipeProcess) {
            char *args1[] = {arguments[0], arguments[1], NULL};
            dup2(pipefd[0], 0);
            close(pipefd[1]);
            if(execvp(arguments[0], args1) == -1){
                perror("Error");
              }
          }
         
          else {
            if(execvp(arguments[0], arguments) == -1){
                perror("Error");
              }
        
          }
          exit(1);
        }
        else {
  // Check if a background process is not running
  if (!background_process) {
    // Set up signal handlers for SIGINT and SIGALRM
    signal(SIGINT, exitSignalHandler);
    signal(SIGALRM, timerHandler);

    // Check if a pipe process is running
    if (pipeProcess) {
      // Set up arguments for the pipe process
      char *args2[] = {arguments[2], arguments[3], NULL};
      // Duplicate the write end of the pipe to stdout
      dup2(pipefd[1], 1);
      // Close the read end of the pipe
      close(pipefd[0]);
      // Execute the pipe process
      execvp(arguments[2], args2);
    }
    // Set up an alarm for 10 seconds
    alarm(10);
    // Wait for the child process to exit or be signaled
    do {
      wpid = waitpid(child_pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
}
    // This should never be reached.
    return -1;
}
