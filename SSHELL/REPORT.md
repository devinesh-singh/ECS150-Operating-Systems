# Report on P1: Simple Shell

Project by Devin Singh and Raj Singh Bhatia

## Summary of P1

Using system calls that UNIX-like operating systems usually offer, including
syscalls belong to process, files, pipes, and signals, we created a simple
shell.

### High-level Design Choices

Some design choices we made in the process of building the shell included how we
wanted to store our data from the commands given as well as how to get the input
from the user and initially store the data in a buffer. We decided that to store
information for each command we would store each command individually, not as
the multiple commands you would expect in a redirected or piped set of commands
to the shell. After storing them, we would process the redirected process after
performing the neccessary process for the first process in the command.

### Implementation

To take care of error processing, we used an enum to hold all the possible error
cases. We used a struct to hold the commands individually. Also, we used
pointers within our structs to link between the previous and next commands in
order to use them appropriately for piping commands. We seperated built in and
the other commands since, based on the phases suggested, they were for the most
part simpler and required less overall arguments in comparison to the other
arguments. Additionally, the background processes were stored seperatey in their
own arrays in order to process the commands seperately.

### Testing

Alongside using the given testing shell script, we tested our implementation by
writing down edge cases that we thought would affect the process we were trying
to run such as these two calls to change the directory:
"cd dir_test"
"cd dir_test/"

### Sources

All sources used are also available as links at the top of sshell.c. Below is a
list of each the sources used:

* [Passing arguments to read function from struct][1]
* [Use of space and counting whitespace][2]
* [Working with background processes][3]

[1]: (https://bit.ly/2OSfTKa)
[2]: (https://bit.ly/2IW6qgV)
[3]: (https://bit.ly/2MIVODg)