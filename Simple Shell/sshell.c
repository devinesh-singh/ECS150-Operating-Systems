/* Sources (also included in Report.md)
https://stackoverflow.com/questions/28613760/passing-argument-to-read-function-from-structures-like-char-struct
https://www.geeksforgeeks.org/isspace-in-c-and-its-application-to-count-whitespace-characters/
https://www.dreamincode.net/forums/topic/406981-running-background-process-in-c-using-fork/
*/

#include "sshell.h"

static int errflag;

static int back_count;
static int back_index;
static int back_pids[MAXPROCESS];
static char *back_cmds[MAXPROCESS];

void push_background(CommandRef_t cmd, int pid)
{
    int index = back_index++;
    back_index %= MAXPROCESS;

    int n = 0;
    for (int i = 0; i < cmd->argc; i++)
        n += strlen(cmd->argv[i]);

    n += cmd->argc;

    char *cmd_str = malloc(n + 1);
    for (int i = 0, j = 0; i < cmd->argc; i++)
    {
        int c = strlen(cmd->argv[i]);
        memcpy(cmd_str + j, cmd->argv[i], c);
        cmd_str[j + c] = ' ';
        j += c + 1;
    }

    ++back_count;
    cmd_str[n - 1] = '&';
    cmd_str[n] = '\0';
    back_cmds[index] = cmd_str;
    back_pids[index] = pid;
}

void print_error_message(int code)
{
    switch (code)
    {
    case ERR_MISSING_COMMAND:
        fprintf(stderr, "Error: missing command\n");
        break;

    case ERR_INVAL_CMDLINE:
        fprintf(stderr, "Error: invalid command line\n");
        break;

    case ERR_CMD_NOTFOUND:
        fprintf(stderr, "Error: command not found\n");
        break;

    case ERR_NO_DIR:
        fprintf(stderr, "Error: no such directory\n");
        break;

    case ERR_CNTOPEN_INPUTFILE:
        fprintf(stderr, "Error: cannot open input file\n");
        break;

    case ERR_NO_INPUTFILE:
        fprintf(stderr, "Error: no input file\n");
        break;

    case ERR_CNTOPEN_OUTPUTFILE:
        fprintf(stderr, "Error: cannot open output file\n");
        break;

    case ERR_NO_OUTPUTFILE:
        fprintf(stderr, "Error: no output file\n");
        break;

    case ERR_MISLOC_INPUT_REDIR:
        fprintf(stderr, "Error: mislocated input redirection\n");
        break;

    case ERR_MISLOC_OUTPUT_REDIR:
        fprintf(stderr, "Error: mislocated output redirection\n");
        break;

    case ERR_MISLOC_BACKGROUND:
        fprintf(stderr, "Error: mislocated background sign\n");
        break;

    case ERR_ACTIVEJOB_RUNNING:
        fprintf(stderr, "Error: active jobs still running\n");
        break;
    }
}

void shell_prompt()
{
    const char *prompt = "sshell$ ";
    fwrite(prompt, strlen(prompt), 1, stdout);
    fflush(stdout);
}

int readline(char *buffer, int n)
{
    char ch;
    int i = 0;

    fflush(stdin);
    while (fread(&ch, 1, 1, stdin) > 0 && ch != '\n')
    {
        buffer[i++] = ch;
        if (i == n)
            break;
    }

    buffer[i] = '\0';
    if (!isatty(STDIN_FILENO))
    {
        printf("%s\n", buffer);
        fflush(stdout);
    }

    return i;
}

CommandRef_t parse_commands(char *buffer, int size)
{
    if (size <= 0)
        return NULL;

    CommandRef_t cmd = (CommandRef_t)malloc(sizeof(Command_t));
    cmd->argc = 0;
    cmd->next = NULL;
    cmd->prev = NULL;
    cmd->inputfile = NULL;
    cmd->outputfile = NULL;

    int i, j = 0;
    char redirection = '\0';
    for (i = 0; i <= size && !errflag; i++)
    {
        char ch = buffer[i];

        if (isspace(ch) || ch == '\0' || ch == '|' || ch == '>' || ch == '<')
        {
            if (i > j)
            {
                buffer[i] = '\0';
                char *tmpstr = strdup(buffer + j);
                switch (redirection)
                {
                case '>':
                {
                    if (cmd->outputfile)
                    {
                        free(tmpstr);
                        tmpstr = NULL;
                        errflag = ERR_MISLOC_OUTPUT_REDIR;
                        print_error_message(errflag);
                    }
                    cmd->outputfile = tmpstr;
                }
                break;
                case '<':
                {
                    if (cmd->inputfile)
                    {
                        free(tmpstr);
                        tmpstr = NULL;
                        errflag = ERR_MISLOC_INPUT_REDIR;
                        print_error_message(errflag);
                    }
                    cmd->inputfile = tmpstr;
                }
                break;
                default:
                    cmd->argv[cmd->argc++] = tmpstr;
                    break;
                }
                redirection = '\0';
            }
            j = i + 1;
        }

        if (ch == '>' || ch == '<')
        {
            redirection = ch;
        }

        if (ch == '|')
        {
            cmd->next = parse_commands(buffer + i + 1, size - i - 1);
            if (cmd->next == NULL && !errflag)
            {
                errflag = ERR_MISSING_COMMAND;
                print_error_message(errflag);
            }
            else
                cmd->next->prev = cmd;
            break;
        }
    }

    if (redirection)
    {
        errflag = (redirection == '>' ? ERR_NO_OUTPUTFILE : ERR_NO_INPUTFILE);
        print_error_message(errflag);
    }

    cmd->argv[cmd->argc] = NULL;
    return cmd;
}

void free_all_commands(CommandRef_t cmd)
{
    CommandRef_t tmp;

    if (!cmd)
        return;

    while (cmd->prev != NULL)
        cmd = cmd->prev;

    while (cmd != NULL)
    {
        for (int i = 0; i < cmd->argc; i++)
            free(cmd->argv[i]);

        tmp = cmd;
        cmd = cmd->next;
        free(tmp);
    }
}

void print_command(CommandRef_t cmd)
{
    printf("%s", cmd->argv[0]);
    for (int i = 1; i < cmd->argc; i++)
        printf(" %s", cmd->argv[i]);
    printf("\n");
}

int run_background(CommandRef_t cmd)
{
    int n = cmd->argc;
    int m = strlen(cmd->argv[n - 1]);
    int flag = cmd->argv[n - 1][m - 1] == '&';
    if (flag)
        cmd->argv[n - 1][m - 1] = '\0';
    return flag;
}

void print_command_status(CommandRef_t cmd, int status)
{
    fprintf(stderr, "+ completed '%s", cmd->argv[0]);
    for (int i = 1; i < cmd->argc; i++)
        fprintf(stderr, " %s", cmd->argv[i]);
    fprintf(stderr, "' [%d]\n", status);
}

int execute_builtin_command(CommandRef_t cmd)
{
    char *cmd_name = cmd->argv[0];

    if (strcmp(cmd_name, "exit") == 0)
    {
        if (back_count)
        {
            errflag = ERR_ACTIVEJOB_RUNNING;
            print_error_message(errflag);
            return 1;
        }

        fprintf(stderr, "Bye...\n");
        exit(0);
    }
    else if (strcmp(cmd_name, "pwd") == 0)
    {
        char cwd[MAXBUFSIZE];
        getcwd(cwd, sizeof(cwd));
        fprintf(stdout, "%s\n", cwd);
        return 1;
    }
    else if (strcmp(cmd_name, "cd") == 0)
    {
        int status = chdir(cmd->argv[1]);
        if (status == 0)
            print_command_status(cmd, status);
        else
        {
            errflag = ERR_NO_DIR;
            print_error_message(errflag);
        }
        return 1;
    }

    return 0;
}

void execute_command(CommandRef_t cmd)
{
    int status;
    int input_fd = -1;
    int output_fd = -1;

    if (pipe(cmd->pipe) < 0)
    {
        perror("failed to initialize pipe");
        exit(-1);
    }

    int background = run_background(cmd);

    int pid = fork();
    if (pid == 0)
    {
        // Input/Output redirection handling
        if (cmd->inputfile)
            input_fd = open(cmd->inputfile, O_RDONLY, 0);

        if (cmd->outputfile)
            output_fd = open(cmd->outputfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        if ((cmd->inputfile && input_fd < 0) || (cmd->outputfile && output_fd < 0))
        {
            errflag = input_fd < 0 ? ERR_CNTOPEN_INPUTFILE : ERR_CNTOPEN_OUTPUTFILE;
            print_error_message(errflag);
            exit(0);
        }

        if (input_fd >= 0)
        {
            dup2(input_fd, 0);
            close(input_fd);
        }

        if (output_fd >= 0)
        {
            dup2(output_fd, 1);
            close(output_fd);
        }

        if (cmd->prev)
            dup2(cmd->prev->pipe[0], 0);

        if (cmd->next)
            dup2(cmd->pipe[1], 1);

        close(cmd->pipe[0]);

        // execute command
        execvp(cmd->argv[0], cmd->argv);
        exit(1);
    }
    else
    {
        if (!background)
        {
            waitpid(pid, &status, 0);
            if (status == 256)
            {
                errflag = ERR_CMD_NOTFOUND;
                print_error_message(errflag);
            }
            else
                print_command_status(cmd, status);
        }
        else
            push_background(cmd, pid);

        close(cmd->pipe[1]);
    }
}

void sigchld_handler(int sig)
{
    int status;
    int pid = wait(&status);
    if (pid < 0)
        return;

    int i = 0;
    while (i < MAXPROCESS && back_pids[i] != pid)
        i++;

    if (i == MAXPROCESS)
        return;

    if (status == 256)
    {
        errflag = ERR_CMD_NOTFOUND;
        print_error_message(errflag);
    }
    else
        fprintf(stderr, "+ completed '%s' [%d]\n", back_cmds[i], status);

    free(back_cmds[i]);
    back_pids[i] = -1;
    --back_count;
}

int main(int argc, char *argv[])
{
    int nbytes;
    CommandRef_t cmds, current;
    char line[MAXBUFSIZE + 1];

    signal(SIGCHLD, sigchld_handler);

    while (1)
    {
        errflag = 0;
        shell_prompt();

        nbytes = readline(line, MAXBUFSIZE);
        cmds = parse_commands(line, nbytes);

        if (errflag == 0)
        {
            current = cmds;
            while (current != NULL)
            {
                if (!execute_builtin_command(current))
                    execute_command(current);

                current = current->next;
            }
        }

        free_all_commands(cmds);
    }

    return 0;
}
