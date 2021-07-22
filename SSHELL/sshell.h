#ifndef __SSHELL_H__
#define __SSHELL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>

#define MAXBUFSIZE  1024
#define MAXARGS     32
#define MAXPROCESS  32

typedef struct _cmd
{
    int argc;
    int pipe[2];
    char *inputfile;
    char *outputfile;
    char *argv[MAXARGS];
    struct _cmd *next;
    struct _cmd *prev;
} Command_t, *CommandRef_t;

enum
{
    ERR_INVAL_CMDLINE,
    ERR_CMD_NOTFOUND,
    ERR_NO_DIR,
    ERR_CNTOPEN_INPUTFILE,
    ERR_NO_INPUTFILE,
    ERR_CNTOPEN_OUTPUTFILE,
    ERR_NO_OUTPUTFILE,
    ERR_MISLOC_INPUT_REDIR,
    ERR_MISLOC_OUTPUT_REDIR,
    ERR_MISLOC_BACKGROUND,
    ERR_ACTIVEJOB_RUNNING,
    ERR_MISSING_COMMAND
};

#endif
