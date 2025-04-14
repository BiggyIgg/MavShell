Description
In this assignment you will write your own shell program, Mav shell (msh), similar to bourne shell (bash), c-shell (csh), or korn shell (ksh). It will accept commands, fork a child process and execute those commands. The shell, like csh or bash, will run and accept commands until the user exits the shell

Requirements
Your program will print out a prompt of msh> when it is ready to accept input. It must read a line of input and, if the command given is a supported shell command, it shall execute the command and display the output of the command.
main

If the command is not supported your shell shall print the invalid command followed by “: Command not found.”
main

After each command completes, your program shall print the msh> prompt and accept another line of input.

Your shell will exit with status zero if the command is “quit” or “exit”.

If the user types a blank line, your shell will, quietly and with no other output, print another prompt and accept a new line of input.

main

Your version of Mav shell shall support up to 10 command line parameters in addition to the command.

Your shell shall support and execute any command entered. Any command in /bin, /usr/bin/, /usr/local/bin/ and the current working directory is to be considered valid for testing.

Mav shell shall be implemented using fork(), wait() and one of the exec family of functions. Your Mav shell shall not use system(). Use of system() will result in a grade of 0.

Your shell shall support the cd command to change directories. Your shell must handle cd ..

main

Your shell shall save the last 50 commands and command line parameters

You shall print the history log, excluding blank line entries when the user types history

msh> history
[1] ls
[2] mkdir foo
[3] cd foo
[4] cd ..
[5] history
The user can re-run any commnd in the history by typing !# where # is the number of the command to rerun.

Your shell shall support redirection. The syntax [process] > [file] tells your shell to redirect the process’s standard output to a file.

You do not need to support redirection for shell built-in commands. You do not need to support stderr redirection or appending to files (e.g. [process] >> [file]). You can assume that there will always be spaces around special characters < and >. See popen.c in Code-Examples

Your shell shall support pipes between programs. You can assume there will always be spaces around the special character | . See pipe.c in Code-Examples

Your program shall block the SIGINT and SIGTSTP signals. See sigint.c in Code-Examples

Tabs or spaces shall be used to indent the code. Your code must use one or the other. All indentation must be consistent.

No line of code shall exceed 100 characters.

All code must be well commented. This means descriptive comments that tell the intent of the code, not just what the code is executing.

Keep your curly brace placement consistent. If you place curly braces on a new line , always place curly braces on a new end. Don’t mix end line brace placement with new line brace placement.

Remove all extraneous debug output before submission. The only output shall be the output of the commands entered or the shell prompt.
