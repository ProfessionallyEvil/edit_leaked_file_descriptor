#ifndef H_PROC
#define H_PROC
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "constants.h"

typedef struct t_fd_info{
    int fd;
    char *path;
    struct t_fd_info *next;
} t_fd_info;

typedef struct list{
    int count;
    t_fd_info *head;
    t_fd_info *cur;
} t_list;

void get_proc_fd_info_count(t_list *list);

void insertFdNodeToList(t_list *list, int fd, char *path);
t_fd_info *get_record_at_offset(t_list *list, int offset);
void dump_list(t_list *list);
void debug_record(t_fd_info *record);
void free_list(t_list *list);
#endif
