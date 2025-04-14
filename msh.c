// NAME: ISHANA KHANDAKAR ID: 1002127576
//The MIT License (MIT)
//
// Copyright (c) 2023 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens
                           // so we need to define what delimits our tokens.
                           // In this case  white space
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 128 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12 // Mav shell currently only supports 12 arguments

// created function call for handling sig. Once caught, print msh and continue
static void handle_signal(int sig)
{
}
int main()
{

  char *command_string = (char *)malloc(MAX_COMMAND_SIZE);
  // prior to entering the while loop, we must initialize history
  char *history[50];
  int history_index = 0;
  for (int i = 0; i < 50; i++)
  {
    history[i] = (char *)malloc(255); // calling malloc to allocate 255 bytes of memory
    memset(history[i], 0, 255);       // fill with all nulls so that no matter how long my command is
                                      // my command will be null terminated.
  }

  struct sigaction act;
  // Zero out the sigaction struct
  memset(&act, '\0', sizeof(act));
  // Set the handler to use the function handle_signal()
  act.sa_handler = &handle_signal;

  if (sigaction(SIGINT, &act, NULL) < 0) // stops "ctrl -C"
  {
    perror("sigaction: ");
    return 1;
  }
  if (sigaction(SIGTSTP, &act, NULL) < 0) // stops "ctrl -Z"
  {
    perror("sigaction: ");
    return 1;
  }
  while (1)
  {
    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(command_string, MAX_COMMAND_SIZE, stdin))
      ;

    // Below we are comparing a defined value WHITESPACE with command line
    //  This is so if the user presses enter with no entries, the prompt will repeat.
    if (strspn(command_string, WHITESPACE) == strlen(command_string))
    {
      continue;
    }
    // save off the history by calling memcopy into history index
    // subtract 1 from strlen so the end line is not saved accidentally
    memcpy(history[history_index++], command_string, strlen(command_string) - 1);
    if (history_index >= 50)
      history_index = 0;

    if (command_string[0] == '!')
    {
      // overwrite command string with history
      int index = atoi(&command_string[1]);
      memcpy(command_string, history[index], 255); // index bc we are using the whole string of history
    }
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      token[i] = NULL;
    }

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;

    char *working_string = strdup(command_string);

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    int pipe_loc = 0;
    int redir_loc = 0;

    while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))

    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);

      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      if (token[i] && strcmp(token[i], "|") == 0)
        pipe_loc = i;
      if (token[i] && strcmp(token[i], ">") == 0)
        redir_loc = i;
      if (token[i] && strcmp(token[i], "<") == 0)
        redir_loc = i;
    }

    // This section checks the input for built in functions that
    //  will be hard coded. These are the functions include
    // cd, exit, quit, and history
    // prior to exec call for the other unknowns

    if (strcmp(token[0], "cd") == 0)
    {

      if (token[1] != NULL) // checking to see if cd will run
      {

        if (chdir(token[1]) != 0)
        {
          perror("cd failed");
        }
      }
    }
    else if (strcmp(token[0], "exit") == 0)
    {
      exit(EXIT_SUCCESS);
    }
    else if (strcmp(token[0], "quit") == 0)
    {
      exit(EXIT_SUCCESS);
    }
    else if (strcmp(token[0], "history") == 0)
    {
      for (int i = 0; i < 50; i++) // prints the entire history when requested
      {
        if (history[i][0] != 0)
        {
          printf("[%d] %s \n", i, history[i]);
        }
      }
    }
    // checking exec for other commands that were not hard coded.
    // this will also clear invalid commands, and handle < and |

    else if (pipe_loc)
    {

      // pipe code
      int pfds[2];

      pipe(pfds);
      pid_t cpid = fork();
      if (!cpid)
      {
        close(STDOUT_FILENO); /* close normal stdout */
        dup(pfds[1]);         /* make stdout same as pfds[1] */
        close(pfds[0]);
        token[pipe_loc] = NULL;
        execvp(token[0], &token[0]);
      }
      else
      {
        pid_t cpid2 = fork(); // we fork again since the parent process contains everything
        if (!cpid2)           // this separate fork is to prevent the exec from closing the initial process(msh)
        {
          close(STDIN_FILENO); /* close normal stdin */
          dup(pfds[0]);        /* make stdin same as pfds[0] */
          close(pfds[1]);
          execvp(token[pipe_loc + 1], &token[pipe_loc + 1]);
        }
        else
        {
          close(pfds[0]);
          close(pfds[1]);
          int status;
          waitpid(cpid2, &status, 0);
          waitpid(cpid, &status, 0);
        }
      }
    }
    else if (redir_loc)
    {
      pid_t redirpid = fork();
      if (redirpid == 0)
      {
        int fd;
        if (strcmp(token[redir_loc], ">") == 0)
        {
          fd = open(token[redir_loc + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
          if (fd < 0)
          {
            perror("Can't open output file.");
            exit(0);
          }
          dup2(fd, STDOUT_FILENO);

          // Trim off the > output part of the command
        }
        else if (strcmp(token[redir_loc], "<") == 0)
        {
          fd = open(token[redir_loc + 1], O_RDONLY);
          if (fd < 0)
          {
            perror("Can't open input file.");
            exit(0);
          }
          dup2(fd, STDIN_FILENO); // 0 means standard input
        }
        close(fd);
        token[redir_loc] = NULL;

        execvp(token[0], &token[0]);
      }
      else if (redirpid == -1)
      {
        // an error happened.
        perror("fork failed: ");
        exit(EXIT_FAILURE);
      }
      else
      {
        int status;
        wait(&status);
      }
    }
    else
    {
      // fork for exec functions
      pid_t pid = fork();

      if (pid == -1)
      {
        // check for error
        perror("fork failed: ");
        exit(EXIT_FAILURE);
      }
      else if (pid == 0)
      {
        // Check if we are in the child process.
        int ret = execvp(token[0], &token[0]);
        if (ret == -1)
        {
          printf("%s: Command not found.\n", token[0]);
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        int status;
        wait(&status); // making parent wait for child process
      }
    }

    // Cleanup allocated memory
    for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      if (token[i] != NULL)
      {
        free(token[i]);
      }
    }

    free(head_ptr);
  }

  free(command_string);

  return 0;
  // e1234ca2-76f3-90d6-0703ac120004
}
