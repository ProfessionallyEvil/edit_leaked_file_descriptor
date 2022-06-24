# edit_leaked_file_descriptor
An all-in-one tool to edit a leaked file descriptor in SetUID binaries.

## Links to Blog
Blog: TBD

## Overview
This repo is used to provides C code to help exploit a leaked or inherited file handle in a SetUID drop-privileges process or child process.  This tool will

 * Enumerate the /proc/self/fd folder for interesting fds that are open
 * List them in a menu to select the target easily
 * use lseek() and read() to copy the file contents to `/tmp/si_fd_edit`
 * Open the `/tmp/si_fd_edit` in nano for editing
 * Upon exiting the editor, it will ask if you want to overwrite the original file with the edits
 * If you select yes, it will lseek() to the beginning and write your modified file to the original

## Building the Tool with the Supplied MakeFile
This tool can be built by simply running the `make` make command.

## Known Issues
 * If the edit results in the edited file being smaller that the original, there will be trailing garbage from the original file left over.
