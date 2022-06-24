#include "proc.h"

void get_proc_fd_info_count(t_list *list) {
  char full_file_path[PATH_MAX];
  char resolved_path[PATH_MAX];
  char *fd_path = "/proc/self/fd";
  struct dirent * dir;
  
  puts(" [\033[34;1m*\033[0m] Scanning the /proc/self/fd directory...");

  // Open the directory
  DIR * d = opendir(fd_path);
  if (d==NULL) return;

  while ((dir = readdir(d)) != NULL) {
    // Ensure it's a symlink, and not 1, 2, 3
    if (dir->d_type == DT_LNK && 
        strcmp(dir->d_name, "0") && 
        strcmp(dir->d_name, "1") && 
        strcmp(dir->d_name, "2")) {

      // Clear the buffers.
      memset(full_file_path, 0, PATH_MAX);
      memset(resolved_path, 0, PATH_MAX);

      // Get the full path to in a buffer.
      snprintf(full_file_path, PATH_MAX-1, "%s/%s", fd_path, dir->d_name);

      // Reolve the symlink
      readlink(full_file_path, resolved_path, PATH_MAX);

      // Make sure the resolved symlink isn't pointing
      // to the /proc/ folder.  If it is not, then add
      // it to our list.
      if (strncmp("/proc/", resolved_path, 6)) {
        insertFdNodeToList(list, atoi(dir->d_name), resolved_path);
      }
    }
  }
  
  // Close the directory
  closedir(d);
}

void insertFdNodeToList(t_list *list, int fd, char *path) {
  /********************************************
   * Create a new t_fd_info struct and add it to
   * the list.
   ********************************************/
  // Allocate the fd_info record memory.
  t_fd_info *record = (t_fd_info*) malloc(sizeof(t_fd_info));

  // Set the fd_info record data.
  record->fd = fd;
  record->path = (char*) malloc(strlen(path)+1);
  strcpy(record->path, path);
  record->next = NULL;

  // If this is the first struct being added, set it's
  // pointer as the head.
  if (list->head == NULL) {
    list->head = record;
  }

  // If the current pointer is NULL set it to this record.
  if (list->cur == NULL ){
    list->cur = record;
  } else {
    // Otherwise, update the next pointer on the current record,
    // then set the current pointer to the new record.
    list->cur->next = record;
    list->cur = record;
  }

  // Finally increment the list count.
  list->count++;
}

t_fd_info *get_record_at_offset(t_list *list, int offset) {
  /********************************************
   * Get a pointer to a record in the list at
   * an offset, or a NULL pointer if things go
   * wrong.
   ********************************************/
  int i = 0;
  t_fd_info *ret = NULL;

  // return NULL pointer if the list is empty or the offset is out
  // of range.
  if (!list->count || offset < 0 || offset >= list->count) {
    return ret;
  }

  // Loop through the list till we get the pointer to
  // the record we are interested in.
  ret = list->head;
  while (i != offset) {
    ret = ret->next;
    i++;
  }
  return ret;
}

void free_list(t_list *list) {
  /********************************************
   * Free all the nodes in a list and reset
   * the list values.
   ********************************************/
  t_fd_info *record = NULL;
  t_fd_info *next = NULL;
  char *allocated_str;

  // Make sure the list is:
  //    - Not a NULL pointer
  //    - The list isn't empty
  if (list == NULL || !list->count) return;

  record = list->head;
  while (record != NULL) {
    // Grab the next pointer record before processing.
    next = record->next;

    // The path string was allocated, so let's handle that.
    allocated_str = record->path;
    if (allocated_str != NULL){
      free(allocated_str);
    }

    // Zero out the values of the record and free it.
    record->fd = 0;
    record->path = NULL;
    record->next = NULL;
    free(record);

    // progress to the next record.
    record = next;
  }

  // Reset the list to empty.
  list->count = 0;
  list->head = NULL;
  list->cur = NULL;
}

void dump_list(t_list *list) {
  int i = 1;
  t_fd_info *offset = NULL;

  // Return if the list is empty.
  if (!list->count) {
    puts(" [*] No records in list.");
    return;
  }

  offset = list->head;
  while (i < list->count+1) {
    //debug_record(offset);
    printf(" %3d) %d => %s\n", i, offset->fd, offset->path);
    offset = offset->next;
    i++;
  }
}

void debug_record(t_fd_info *record) {
  if (record == NULL) return;
  printf("\n\t--| Record %p | ---\n", record);
  printf(" [*] record->fd: %d\n", record->fd);
  printf(" [*] record->path: [%p] => '%s'\n", record->path, record->path);
  printf(" [*] record->next: %p\n", record->next);
}