#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define COMMAND_NOTUSED(V) ((void) V)

typedef enum command_type {
    FULL_MATCH = 0,
    FIXED_PREFIX = 1
} command_type;

enum command_retrun_value {
    COMMAND_OK = 0,
    COMMAND_ERROR = -1
};

typedef int32_t (*handler_t)(char *,int, char **);

typedef struct command_info {
    char *cmd;
    u_int32_t cmd_len;
    u_int32_t type;
    handler_t handler;
    struct command_info *next;
} command_info, *command_info_ptr;

typedef struct command_service {
    command_info_ptr command_list;
    command_info_ptr end_command;
    u_int32_t command_count;
} command_service, *command_service_ptr;

/* init a new command service */
command_service_ptr command_service_create();
/* register a new command and a handler to execute the task accordingly */
int32_t command_service_register_handler(command_service_ptr, char *cmd, size_t cmd_len, command_type cmd_type, handler_t handler); 
/* execute the command */
int32_t command_service_run(command_service_ptr service, char *cmd,int argc, char **argv);
/* destory a command service, release the memory alloced */
void command_service_destory(command_service_ptr service);
#endif /* End definition of COMMAND_H */
